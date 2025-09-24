#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <variant>
#include <mutex>

#include <pv/caProvider.h>
#include <pva/client.h>

/**
 * @brief Represents an EPICS enum (mbbo/mbbi) with an integer index and string choices.
 */
struct PVEnum {
    int index = 0;                    ///< Current index of the enum choice.
    std::vector<std::string> choices; ///< All available string choices.
    std::string choice = "";          ///< Currently selected string choice.
};

/**
 * @brief Variant type for pointers to variables monitored by a PV.
 */
using MonitorPtr = std::variant<std::monostate,
                                std::string *,
                                int *,
                                double *,
                                std::vector<std::string> *,
                                std::vector<int> *,
                                std::vector<double> *,
                                PVEnum *>;

/**
 * @brief Monitors a pvac::ClientChannel's connection status.
 */
class ConnectionMonitor : public pvac::ClientChannel::ConnectCallback {
  public:
    /**
     * @brief Constructs a ConnectionMonitor.
     */
    ConnectionMonitor() = default;

    /**
     * @brief Destroys the ConnectionMonitor.
     */
    virtual ~ConnectionMonitor() override = default;

    /**
     * @brief Callback when connection status changes.
     * @param event Connection event details.
     */
    virtual void connectEvent(const pvac::ConnectEvent &event) override final;

    /**
     * @brief Checks if connected.
     * @return True if connected, false otherwise.
     */
    bool connected() const;

  private:
    bool connected_ = false; ///< Connection status flag.
};

/**
 * @brief Manages a single EPICS Process Variable (PV).
 *
 * Handles connection, monitoring, and value updates for a PV.
 */
struct PVHandler : public pvac::ClientChannel::MonitorCallback {
  public:
    pvac::ClientChannel channel; ///< PVA client channel.
    std::string name;            ///< Name of the process variable.

    /**
     * @brief Constructs a PVHandler.
     * @param provider PVA client provider.
     * @param pv_name Name of the process variable.
     */
    PVHandler(pvac::ClientProvider &provider, const std::string &pv_name);

    /**
     * @brief Checks if the PV channel is connected.
     * @return True if connected, false otherwise.
     */
    bool connected() const;

    /**
     * @brief Checks if new data is available from the monitor
     * @return True if new data was received, false otherwise.
     */
    bool data_available();

    /**
     * @brief Adds the variable to list of variables which are updated by monitor
     * @tparam T Type of the variable.
     * @param var Reference to the variable to be updated.
     */
    template <typename T> void set_monitor(T &var) { monitor_var_ptrs_.push_back(&var); }

    pvac::Monitor& getMonitor() { return monitor_; }
    std::mutex mutex;



  private:
    pvac::Monitor monitor_;                                 ///< PVA data monitor.
    std::vector<MonitorPtr> monitor_var_ptrs_;              ///< Pointers to the user's variable.
    std::unique_ptr<ConnectionMonitor> connection_monitor_; ///< Monitors connection status.
    bool new_data = false;

    /**
     * @brief Called when the monitor fires (new data, etc.)
     * @param evt Monitor event struct
     */
    void monitorEvent(const pvac::MonitorEvent& evt) override final;
                                                            ///
    /**
     * @brief Extracts and assigns the PV value to the monitored variable.
     * @param pfield Pointer to the PVStructure containing new data.
     */
    void get_monitored_variable(const epics::pvData::PVStructure *pfield);
};

/**
 * @brief Manages a group of EPICS Process Variables.
 */
struct PVGroup {
  public:
    /**
     * @brief Constructs a PVGroup with initial PVs.
     * @param provider PVA client provider.
     * @param pv_list Names of PVs to add.
     */
    PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_list);

    /**
     * @brief Constructs an empty PVGroup.
     * @param provider PVA client provider.
     */
    PVGroup(pvac::ClientProvider &provider);

    /**
     * @brief Adds a new PV to the group without macro substitutions.
     * @param pv_name Name of the PV.
     * @throw std::runtime_error If PV name already exists.
     */
    void add(const std::string &pv_name);

    /**
     * @brief Sets the variable to monitor for a specific PV in the group.
     * @tparam T Type of the variable.
     * @param pv_name Name of the PV.
     * @param var Reference to the variable.
     * @throw std::runtime_error If PV not found.
     */
    template <typename T> void set_monitor(const std::string &pv_name, T &var) {
        PVHandler &pv = this->get_pv(pv_name);
        pv.set_monitor(var);
    }

    /**
     * @brief Retrieves a PVHandler by name.
     * @param pv_name Name of the PV.
     * @return Reference to the PVHandler object.
     * @throw std::runtime_error If PV not found.
     */
    PVHandler &get_pv(const std::string &pv_name);

    /**
     * @brief Accesses a PVHandler by name using array subscript operator.
     * @param pv_name Name of the PV.
     * @return Reference to the PVHandler object.
     * @throw std::runtime_error If PV not found.
     */
    PVHandler &operator[](const std::string &pv_name);

    /**
     * @brief Checks if new data is available from and of the monitors in the group
     * @return True if any PV received new data, false otherwise.
     */
    bool data_available();

    void add_sync_callback(std::function<void()> cb) {
	sync_callbacks_.push_back(std::move(cb));
    }

    void run_sync_callbacks() {
	for (auto cb : sync_callbacks_) {
	    cb();
	}
    }

  private:
    // bool new_data = false;
    pvac::ClientProvider &provider_;                   ///< PVA client provider.
    std::unordered_map<std::string, std::unique_ptr<PVHandler>> pv_map; ///< Map of PVs by name.
    std::vector<std::function<void()>> sync_callbacks_;
};
