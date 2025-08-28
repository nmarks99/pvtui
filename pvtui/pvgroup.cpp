#include <pvtui/pvgroup.hpp>
#include <regex>

constexpr int DEFAULT_PRECISION = 4;

void ConnectionMonitor::connectEvent(const pvac::ConnectEvent &event) {
    if (event.connected) {
        connected_ = true;
    } else {
        connected_ = false;
    }
}

bool ConnectionMonitor::connected() const { return connected_; }

PVHandler::PVHandler(pvac::ClientProvider &provider, const std::string &pv_name)
    : channel(provider.connect(pv_name)), monitor_(channel.monitor(this)),
      connection_monitor_(std::make_unique<ConnectionMonitor>()), name(pv_name) {
    channel.addConnectListener(connection_monitor_.get());
}

void PVHandler::monitorEvent(const pvac::MonitorEvent &evt) {
    switch (evt.event) {
    case pvac::MonitorEvent::Data:
        while (monitor_.poll()) {
            this->get_monitored_variable(monitor_.root.get());
        }
        break;
    case pvac::MonitorEvent::Disconnect:
        break;
    case pvac::MonitorEvent::Fail:
        break;
    case pvac::MonitorEvent::Cancel:
        break;
    }
}

bool PVHandler::connected() const { return connection_monitor_->connected(); }

void PVHandler::get_monitored_variable(const epics::pvData::PVStructure *pfield) {
    namespace pvd = epics::pvData;

    // get the display precision
    int precision = DEFAULT_PRECISION;
    auto display_struct = pfield->getSubField<pvd::PVStructure>("display");
    if (display_struct) {
        auto format_field = display_struct->getSubField<pvd::PVString>("format");
        if (format_field) {
            std::string prec_str = format_field->get();
            std::regex fmt_regex(R"(F\d+\.(\d+))");
            std::smatch match;
            if (std::regex_match(prec_str, match, fmt_regex) && match.size() == 2) {
                precision = std::stoi(match[1]);
            }
        }
    }

    for (auto mon_ptr : monitor_var_ptrs_) {
        std::visit(
            [&](auto ptr) {
                using PtrType = std::decay_t<decltype(ptr)>;
                this->new_data = true;
                if constexpr (std::is_same_v<PtrType, int *>) {
                    if (ptr) {
                        if (auto val_field = pfield->getSubFieldT<pvd::PVScalar>("value")) {
                            *ptr = val_field->getAs<int>();
                        };
                    }
                } else if constexpr (std::is_same_v<PtrType, double *>) {
                    if (ptr) {
                        if (auto val_field = pfield->getSubFieldT<pvd::PVDouble>("value")) {
                            *ptr = val_field->getAs<double>();
                        };
                    }
                } else if constexpr (std::is_same_v<PtrType, std::string *>) {
                    // it can be useful to be able to dump any value to a string
                    if (ptr) {
                        std::string type_str = pfield->getStructure()->getField("value")->getID();
                        if (type_str == "string") {
                            if (auto val_field = pfield->getSubField<pvd::PVString>("value")) {
                                *ptr = val_field->getAs<std::string>();
                            }
                        } else if (type_str == "byte[]") {
                            pvd::shared_vector<const signed char> vals =
                                pfield->getSubFieldT<pvd::PVByteArray>("value")->view();
                            auto last_ind =
                                std::find_if(vals.rbegin(), vals.rend(), [](const signed char c) {
                                    return std::isalnum(static_cast<unsigned char>(c));
                                });
                            *ptr = std::string(vals.begin(), last_ind.base());
                        } else {
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(precision);
                            if (auto val_field = pfield->getSubField("value")) {
                                val_field->dumpValue(oss);
                                *ptr = oss.str();
                            }
                        }
                    }
                } else if constexpr (std::is_same_v<PtrType, PVEnum *>) {
                    if (ptr) {
                        pvd::shared_vector<const std::string> choices =
                            pfield->getSubFieldT<pvd::PVStringArray>("value.choices")->view();
                        int index = pfield->getSubFieldT<pvd::PVInt>("value.index")->getAs<int>();
                        if (choices.size() > index) {
                            ptr->index = index;
                            ptr->choice = choices.at(index);
                            if (ptr->choices.size() != choices.size()) {
                                ptr->choices.resize(choices.size());
                            }
                            std::copy(choices.begin(), choices.end(), ptr->choices.begin());
                        }
                    }
                } else if constexpr (std::is_same_v<PtrType, std::vector<double> *>) {
                    if (ptr) {
                        pvd::shared_vector<const double> vals =
                            pfield->getSubFieldT<pvd::PVDoubleArray>("value")->view();
                        if (ptr->size() != vals.size()) {
                            ptr->resize(vals.size());
                        }
                        std::copy(vals.begin(), vals.end(), ptr->begin());
                    }
                } else if constexpr (std::is_same_v<PtrType, std::vector<int> *>) {
                    if (ptr) {
                        pvd::shared_vector<const int> vals =
                            pfield->getSubFieldT<pvd::PVIntArray>("value")->view();
                        if (ptr->size() != vals.size()) {
                            ptr->resize(vals.size());
                        }
                        std::copy(vals.begin(), vals.end(), ptr->begin());
                    }
                } else if constexpr (std::is_same_v<PtrType, std::vector<std::string> *>) {
                    if (ptr) {
                        pvd::shared_vector<const std::string> vals =
                            pfield->getSubFieldT<pvd::PVStringArray>("value")->view();
                        if (ptr->size() != vals.size()) {
                            ptr->resize(vals.size());
                        }
                        std::copy(vals.begin(), vals.end(), ptr->begin());
                    }
                } else {
                    // Unsupported pointer type which is not in MonitorPtr variant
                    new_data = false;
                }
            },
            mon_ptr);
    }
}

bool PVHandler::data_available() {
    if (new_data) {
        new_data = false;
        return true;
    } else {
        return false;
    }
}

PVGroup::PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_names)
    : provider_(provider) {
    for (const auto &name : pv_names) {
        this->add(name);
    }
}

PVGroup::PVGroup(pvac::ClientProvider &provider) : provider_(provider) {}

void PVGroup::add(const std::string &pv_name) {
   if (!pv_map.count(pv_name)) {
        pv_map.emplace(pv_name, std::make_unique<PVHandler>(provider_, pv_name));
    }
}

PVHandler &PVGroup::get_pv(const std::string &pv_name) {
    auto it = pv_map.find(pv_name);
    if (it == pv_map.end()) {
        throw std::runtime_error(pv_name + " not registered in PVGroup");
    }
    return *it->second;
}

PVHandler &PVGroup::operator[](const std::string &pv_name) { return this->get_pv(pv_name); }

bool PVGroup::data_available() {
    bool new_data = false;
    for (auto &[_, pv] : pv_map) {
        new_data |= pv->data_available();
    }
    return new_data;
}
