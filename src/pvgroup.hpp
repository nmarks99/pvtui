#pragma once
#include <memory>
#include <pv/caProvider.h>
#include <pva/client.h>

#include <unordered_map>
#include <variant>

struct PVAny {
    std::string subfield = "value";
    std::string value = "";
    int prec = 4;
    
    PVAny(const std::string &sfield) : subfield(sfield) {}
    PVAny() : subfield("value"), value(""), prec(4) {}

    operator std::string() const {
        return value;
    }

};
std::string operator+(const PVAny& lhs, const std::string& rhs);
std::string operator+(const std::string& lhs, const PVAny& rhs);


// Represents a typical "enum" (mbbo/mbbi) with an integer index and string name
struct PVEnum {
    int index = 0;
    std::vector<std::string> choices;
    std::string choice = "";
};

// Types of variables which can be set to be updated by a monitor
using MonitorPtr = std::variant<std::monostate, std::string *, int *, double *, std::vector<std::string> *,
                                std::vector<int> *, std::vector<double> *, PVEnum *, PVAny*>;

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

    // returns true is new data was received, else false
    bool update();

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

    ProcessVariable& get_pv(const std::string &pv_name);

    // equivalent to PVGroup.get_pv("pv_name")
    ProcessVariable& operator[](const std::string &pv_name);

    // returns true is new data was received, else false
    bool update();

  private:
    std::unordered_map<std::string, ProcessVariable> pv_map;
};
