#include "pvgroup.hpp"
#include <iostream>
#include <memory>
#include <variant>

constexpr int DEFAULT_PRECISION = 4;

std::string operator+(const PVAny& lhs, const std::string& rhs) {
    return lhs.value + rhs;
}

std::string operator+(const std::string& lhs, const PVAny& rhs) {
    return lhs + rhs.value;
}

void ConnectionMonitor::connectEvent(const pvac::ConnectEvent &event) {
    if (event.connected) {
        connected_ = true;
    } else {
        connected_ = false;
    }
}

bool ConnectionMonitor::connected() const { return connected_; }

ProcessVariable::ProcessVariable(pvac::ClientProvider &provider, const std::string &pv_name)
    : channel(provider.connect(pv_name)), monitor_(channel.monitor()),
      connection_monitor_(std::make_unique<ConnectionMonitor>()), name(pv_name) {
    channel.addConnectListener(connection_monitor_.get());
}

bool ProcessVariable::connected() const { return connection_monitor_->connected(); }

void ProcessVariable::get_monitored_variable(const epics::pvData::PVStructure *pfield) {
    namespace pvd = epics::pvData;
    std::visit(
        [&](auto ptr) {
            using PtrType = std::decay_t<decltype(ptr)>;

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
                    } else {
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(DEFAULT_PRECISION);
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
                    pvd::shared_vector<const int> vals = pfield->getSubFieldT<pvd::PVIntArray>("value")->view();
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
            } else if constexpr (std::is_same_v<PtrType, PVAny*>) {
                if (ptr) {
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(ptr->prec);
                    if (auto val_field = pfield->getSubField(ptr->subfield)) {
                        val_field->dumpValue(oss);
                        ptr->value = oss.str();
                    }
                }
            }
        },
        monitor_var_ptr_);
}

bool ProcessVariable::update() {
    bool new_data = false;
    if (std::holds_alternative<std::monostate>(this->monitor_var_ptr_)) {
        return new_data;
    }

    if (monitor_.test()) {
        switch (monitor_.event.event) {
        case pvac::MonitorEvent::Data:
            new_data = true;
            while (monitor_.poll()) {
                auto pfield = monitor_.root.get();
                this->get_monitored_variable(pfield);
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

    return new_data;
}

PVGroup::PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_names) : provider_(provider) {
    for (const auto &name : pv_names) {
        pv_map.emplace(name, ProcessVariable(provider, name));
    }
}

PVGroup::PVGroup(pvac::ClientProvider &provider) : provider_(provider) {}

std::string fill_macros(const std::string &instr, const std::unordered_map<std::string,std::string> &macros_dict) {
    std::string out = instr;
    size_t ind = 0;
    for (auto &[k, v] : macros_dict) {
	std::string pholder = "$(" + k + ")";
	while ((ind = out.find(pholder)) != std::string::npos) {
	    out.replace(ind, 4, v);
	}
    }
    return out;
}

void PVGroup::add(const std::string &pv_name) {
    if (pv_map.count(pv_name)) {
        throw std::runtime_error("PV " + pv_name + " already registered in PVGroup");
    } else {
        pv_map.emplace(pv_name, ProcessVariable(provider_, pv_name));
    }
}

std::string PVGroup::add(const std::string &pv_name, const std::unordered_map<std::string, std::string> &macros_dict) {
    auto name = fill_macros(pv_name, macros_dict);
    this->add(name);
    return name;
}

ProcessVariable &PVGroup::get_pv(const std::string &pv_name) {
    if (!pv_map.count(pv_name)) {
        throw std::runtime_error(pv_name + " not registered in PVGroup");
    }
    return pv_map.at(pv_name);
}

ProcessVariable& PVGroup::operator[](const std::string &pv_name) {
    return this->get_pv(pv_name);
}

bool PVGroup::update() {
    bool new_data = false;
    for (auto &[_, pv] : pv_map) {
        new_data |= pv.update();
    }
    return new_data;
}
