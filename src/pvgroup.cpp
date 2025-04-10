#include "pvgroup.hpp"

#include <charconv>
#include <sstream>

// FIX: won't work on enum types
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

                        // get the value field and dump it into a ostringstream
                        std::ostringstream oss;
                        auto pfield = monitor.root.get()->getSubField("value");
                        if (pfield) {
                            pfield->dumpValue(oss);
                        }

                        std::string val_str = oss.str();
                        if constexpr (std::is_same_v<PtrType, int*>) {
                            if (ptr) {
                                int val_int = 0;
                                auto res = std::from_chars(val_str.data(), val_str.data()+val_str.size(), val_int);
                                if (res.ec == std::errc()) {
                                    *ptr = val_int;
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, double*>) {
                            if (ptr) {
                                double val_double = 0.0;
                                auto res = std::from_chars(val_str.data(), val_str.data()+val_str.size(), val_double);
                                if (res.ec == std::errc()) {
                                    *ptr = val_double;
                                }
                            }
                        }
                        else if constexpr (std::is_same_v<PtrType, std::string*>) {
                            if (ptr) {
                                *ptr = val_str;
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
