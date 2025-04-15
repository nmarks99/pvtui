#pragma once
#include <memory>
#include <pv/caProvider.h>
#include <pva/client.h>

#include <unordered_map>
#include <variant>

constexpr int PVGROUP_PRECISION = 4;

// // Helper function to contruct a map of ClientChannel's
// static inline std::unordered_map<std::string, pvac::ClientChannel> construct_chan_map(pvac::ClientProvider &provider,
                                                                   // const std::vector<std::string> &pv_list) {
    // std::unordered_map<std::string, pvac::ClientChannel> map_out;
    // for (const auto &name : pv_list) {
        // map_out.insert({name, provider.connect(name)});
    // }
    // return map_out;
// }
//

// Types of variables which can be set to be updated by a monitor
using MonitorPtr = std::variant<std::monostate, int*, double*, std::string*>;

class ConnectionMonitor : public pvac::ClientChannel::ConnectCallback {
  public:
    ConnectionMonitor() {}
    virtual ~ConnectionMonitor() override = default;

    virtual void connectEvent(const pvac::ConnectEvent& event) override final;

    bool connected() const;

  private:
    bool connected_ = false;
};


enum class PVType {
    PVTYPE_STRING,
    PVTYPE_DOUBLE,
    PVTYPE_INT,
    PVTYPE_ENUM,
};

struct ProcessVariable {
  public:
    ProcessVariable(pvac::ClientProvider &provider, const std::string &pv_name);

    bool connected() const;

    pvac::ClientChannel channel;
    std::string name;

  private:
    pvac::MonitorSync monitor;
    MonitorPtr monitor_var_ptr;
    std::shared_ptr<ConnectionMonitor> connection_monitor;

    friend class PVGroup;
};


struct PVGroup {
  public:
    PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_list);

    template <typename T>
    void set_monitor(const std::string &pv_name, T &var) {
        ProcessVariable& pv = this->get(pv_name);
        pv.monitor_var_ptr = &var;
    }

    ProcessVariable& get(const std::string &pv_name);

    void update();

  private:
    std::unordered_map<std::string, ProcessVariable> pv_map;
};
