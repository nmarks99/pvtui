#include <pv/caProvider.h>
#include <pva/client.h>

#include <unordered_map>
#include <variant>

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

    PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_list)
        : channels(construct_chan_map(provider, pv_list)) {}

    bool create_monitor(const std::string &pv_name, double &var);

    void update();

    std::unordered_map<std::string, pvac::ClientChannel> channels;
    std::unordered_map<std::string, std::pair<pvac::MonitorSync, double*>> monitors;
};
