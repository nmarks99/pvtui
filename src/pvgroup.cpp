#include "pvgroup.hpp"

bool PVGroup::create_monitor(const std::string &pv_name, int &var) {
    if (channels.count(pv_name)) {
        if (!monitors.count(pv_name)) {
            // create monitor if there isn't one
            monitors[pv_name].first = channels.at(pv_name).monitor();
        }
        // if there is, set the variable which is updated
        monitors[pv_name].second = &var;
        return true;
    } else {
        return false;
    }
}

bool PVGroup::create_monitor(const std::string &pv_name, double &var) {
    if (channels.count(pv_name)) {
        if (!monitors.count(pv_name)) {
            // create monitor if there isn't one
            monitors[pv_name].first = channels.at(pv_name).monitor();
        }
        // if there is, set the variable which is updated
        monitors[pv_name].second = &var;
        return true;
    } else {
        return false;
    }
}


bool PVGroup::create_monitor(const std::string &pv_name, std::string &var) {
    if (channels.count(pv_name)) {
        if (!monitors.count(pv_name)) {
            // create monitor if there isn't one
            monitors[pv_name].first = channels.at(pv_name).monitor();
        }
        // if there is, set the variable which is updated
        monitors[pv_name].second = &var;
        return true;
    } else {
        return false;
    }
}

void PVGroup::update() {
    for (auto &[pv_name, pair] : monitors) {
        auto &monitor = pair.first;
        auto &monitor_ptr = pair.second;
        if (pair.first.test()) {
            switch (pair.first.event.event) {
            case pvac::MonitorEvent::Data:
                while (pair.first.poll()) {
                    std::visit([&](auto ptr) {
                        using PtrType = std::decay_t<decltype(ptr)>;
                        if constexpr (std::is_same_v<PtrType, int*>) {
                            if (ptr) {
                                *ptr = monitor.root.get()->getSubFieldT<epics::pvData::PVScalar>("value")->getAs<int>();
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, double*>) {
                            if (ptr) {
                                *ptr = monitor.root.get()->getSubFieldT<epics::pvData::PVDouble>("value")->get();
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, std::string*>) {
                            if (ptr) {
                                *ptr = monitor.root.get()->getSubFieldT<epics::pvData::PVString>("value")->get();
                                // *ptr = monitor.root.get()->getSubFieldT<epics::pvData::PVString>("value")->get();
                            }
                        }
                    }, monitor_ptr);
                }
                break;
            case pvac::MonitorEvent::Cancel:
                break;
            case pvac::MonitorEvent::Disconnect:
                break;
            case pvac::MonitorEvent::Fail:
                break;
            }
        }
    }
}
