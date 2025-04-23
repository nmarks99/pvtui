#pragma once
#include <memory>
#include <pv/caProvider.h>
#include <pva/client.h>

#include <unordered_map>
#include <variant>

// Represents a typical "enum" (mbbo/mbbi) with an integer index and string name
struct PVEnum {
    int index = 0;
    std::string choice = "";
};

// Types of variables which can be set to be updated by a monitor
using MonitorPtr = std::variant<std::monostate, std::string *, int *, double *, std::vector<std::string> *,
                                std::vector<int> *, std::vector<double> *, PVEnum *>;

class ConnectionMonitor : public pvac::ClientChannel::ConnectCallback {
  public:
    ConnectionMonitor() {}
    virtual ~ConnectionMonitor() override = default;

    virtual void connectEvent(const pvac::ConnectEvent &event) override final;

    bool connected() const;

  private:
    bool connected_ = false;
};

struct ProcessVariable {
  public:
    pvac::ClientChannel channel;
    std::string name;

    ProcessVariable(pvac::ClientProvider &provider, const std::string &pv_name);

    bool connected() const;

    void update();

    template <typename T>
    void set_monitor(T &var) {
        monitor_var_ptr_ = &var;
    }

  private:
    pvac::MonitorSync monitor_;
    MonitorPtr monitor_var_ptr_;
    std::unique_ptr<ConnectionMonitor> connection_monitor_;

    void get_monitored_variable(const epics::pvData::PVStructure *pfield);

};

struct PVGroup {
  public:
    PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_list);

    template <typename T> void set_monitor(const std::string &pv_name, T &var) {
        ProcessVariable &pv = this->get_pv(pv_name); // will throw if pv not in map
        pv.set_monitor(var);
    }

    ProcessVariable &get_pv(const std::string &pv_name);

    void update();

  private:
    std::unordered_map<std::string, ProcessVariable> pv_map;
};
