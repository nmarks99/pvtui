#include <pv/caProvider.h>
#include <pva/client.h>

#include <unordered_map>
#include <variant>

constexpr int PVGROUP_PRECISION = 4;

static inline std::unordered_map<std::string, pvac::ClientChannel> construct_chan_map(pvac::ClientProvider &provider,
                                                                   const std::vector<std::string> &pv_list) {
    std::unordered_map<std::string, pvac::ClientChannel> map_out;
    for (const auto &name : pv_list) {
        map_out.insert({name, provider.connect(name)});
    }
    return map_out;
}

using MonitorPtr = std::variant<int*, double*, std::string*>;

struct PVGroup {

    // Constructor creates ClientChannels for each requested PV
    PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_list)
        : channels(construct_chan_map(provider, pv_list)) {}

    // create a monitor that updates the given variable
    template <typename T>
    bool create_monitor(const std::string &pv_name, T &var) {
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

    // Update the variables associated with each monitor
    void update();

    std::unordered_map<std::string, pvac::ClientChannel> channels;
    std::unordered_map<std::string, std::pair<pvac::MonitorSync, MonitorPtr>> monitors;
};
