#include "pvgroup.hpp"
#include <variant>


void ConnectionMonitor::connectEvent(const pvac::ConnectEvent& event) {
    if (event.connected) {
        connected_ = true;
    } else {
        connected_ = false;
    }
}

bool ConnectionMonitor::connected() const {
    return connected_;
}


ProcessVariable::ProcessVariable(pvac::ClientProvider &provider, const std::string &pv_name) :
    channel(provider.connect(pv_name)),
    monitor(channel.monitor()),
    connection_monitor(std::make_shared<ConnectionMonitor>()),
    name(pv_name)
{
    channel.addConnectListener(connection_monitor.get());
}

bool ProcessVariable::connected() const {
    return connection_monitor->connected();
}


PVGroup::PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_names) {
    for (const auto &name : pv_names) {
        pv_map.emplace(name, ProcessVariable(provider, name));
    }
}

ProcessVariable& PVGroup::get(const std::string &pv_name) {
    if (!pv_map.count(pv_name)) {
        throw std::runtime_error(pv_name + " not registered in PVGroup");
    }
    return pv_map.at(pv_name);
}

void PVGroup::update() {

    using epics::pvData::PVDoubleArray;
    using epics::pvData::PVIntArray;
    using epics::pvData::PVStringArray;
    using epics::pvData::PVInt;
    using epics::pvData::PVScalar;
    using epics::pvData::PVDouble;
    using epics::pvData::PVString;
    using epics::pvData::shared_vector;

    for (auto &[pv_name, pv] : pv_map) {
        auto &monitor = pv.monitor;
        auto &monitor_ptr = pv.monitor_var_ptr;
        if (std::holds_alternative<std::monostate>(monitor_ptr)) {
            continue;
        }
        if (monitor.test()) {
            switch (monitor.event.event) {

            case pvac::MonitorEvent::Data:
                while (monitor.poll()) {

                    std::visit([&](auto ptr) {
                        using PtrType = std::decay_t<decltype(ptr)>;
                        auto pfield = monitor.root.get();

                        if constexpr (std::is_same_v<PtrType, int*>) {
                            if (ptr) {
                                if (auto val_field = pfield->getSubFieldT<PVScalar>("value")) {
                                    *ptr = val_field->getAs<int>();
                                };
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, double*>) {
                            if (ptr) {
                                if (auto val_field = pfield->getSubFieldT<PVDouble>("value")) {
                                    *ptr = val_field->getAs<double>();
                                };
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, std::string*>) {
                            // it can be useful to be able to dump any value to a string
                            if (ptr) {
                                std::ostringstream oss;
                                if (auto val_field = pfield->getSubField("value")) {
                                    val_field->dumpValue(oss);
                                    *ptr = oss.str();
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, PVEnum*>) {
                            if (ptr) {
                                shared_vector<const std::string> choices = pfield->getSubFieldT<PVStringArray>("value.choices")->view();
                                int index = pfield->getSubFieldT<PVInt>("value.index")->getAs<int>();
                                if (choices.size() > index) {
                                    ptr->index = index;
                                    ptr->choice = choices.at(index);
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, std::vector<double>*>) {
                            if (ptr) {
                                shared_vector<const double> vals = pfield->getSubFieldT<PVDoubleArray>("value")->view();
                                if (ptr->size() != vals.size()) {
                                    ptr->resize(vals.size());
                                }
                                for (int i = 0; i < vals.size(); i++) {
                                    ptr->at(i) = vals.at(i);
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, std::vector<int>*>) {
                            if (ptr) {
                                shared_vector<const int> vals = pfield->getSubFieldT<PVIntArray>("value")->view();
                                if (ptr->size() != vals.size()) {
                                    ptr->resize(vals.size());
                                }
                                for (int i = 0; i < vals.size(); i++) {
                                    ptr->at(i) = vals.at(i);
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, std::vector<std::string>*>) {
                            if (ptr) {
                                shared_vector<const std::string> vals = pfield->getSubFieldT<PVStringArray>("value")->view();
                                if (ptr->size() != vals.size()) {
                                    ptr->resize(vals.size());
                                }
                                for (int i = 0; i < vals.size(); i++) {
                                    ptr->at(i) = vals.at(i);
                                }
                            }
                        }
                    }, monitor_ptr);
                }
                break;


            case pvac::MonitorEvent::Cancel:
                std::cout << "Monitor Cancel: " << monitor.name() << std::endl;
                break;


            case pvac::MonitorEvent::Disconnect:
                std::cout << "Monitor Disconnect: " << monitor.name() << std::endl;
                break;


            case pvac::MonitorEvent::Fail:
                std::cout << "Monitor Fail: " << monitor.name() << std::endl;
                break;


            }
        }
    }
}
