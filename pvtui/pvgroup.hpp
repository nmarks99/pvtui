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
 * @brief Represents the state of an EPICS enumeration (e.g., mbbo/mbbi).
 */
struct PVEnum {
    int index = 0;                    ///< The current integer index of the selected choice.
    std::vector<std::string> choices; ///< The list of all available string choices for the enum.
    std::string choice = "";          ///< The string value of the currently selected choice.
};

/**
 * @brief A variant type that holds a pointer to a variable monitored by a PV.
 *
 * This allows a single mechanism to update variables of different types.
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
     * @brief Callback invoked when the channel's connection status changes.
     * @param event The connection event details provided by the client channel.
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
     * @brief Checks if new data has been received from the PV monitor.
     * @return True if new data is available, false otherwise.
     */
    bool data_available();

    /**
     * @brief Registers a variable to be updated when the PV monitor receives new data.
     * @tparam T The type of the variable to monitor.
     * @param var A reference to the variable that will be updated.
     */
    template <typename T> void set_monitor(T &var) { monitor_var_ptrs_.push_back(&var); }

    /**
     * @brief Gets the underlying PVA monitor instance.
     * @return A reference to the pvac::Monitor object.
     */
    pvac::Monitor& getMonitor() { return monitor_; }

    /**
     * @brief Gets the mutex for thread-safe access to PV data.
     * @return A reference to the std::mutex object.
     */
    std::mutex &get_mutex() { return mutex; }

  private:
    std::mutex mutex;



  private:
    pvac::Monitor monitor_;                                 ///< PVA data monitor.
    std::vector<MonitorPtr> monitor_var_ptrs_;              ///< Pointers to the user's variable.
    std::unique_ptr<ConnectionMonitor> connection_monitor_; ///< Monitors connection status.
    bool new_data = false;

    /**
     * @brief Callback invoked when a monitor event occurs (e.g., new data).
     * @param evt The monitor event containing the new data and status.
     */
    void monitorEvent(const pvac::MonitorEvent& evt) override final;
                                                            ///
    /**
     * @brief Extracts the PV value from the event and updates the monitored variable.
     * @param pfield A pointer to the PVStructure containing the new data.
     */
    void get_monitored_variable(const epics::pvData::PVStructure *pfield);
};

/**
 * @brief Manages a collection of EPICS Process Variables (PVs).
 *
 * This class provides a centralized way to add, access, and monitor a group of
 * PVs, handling the underlying connections and data updates.
 */
struct PVGroup {
  public:
    /**
     * @brief Constructs a PVGroup and initializes it with a list of PVs.
     * @param provider The PVA client provider.
     * @param pv_list A list of PV names to add to the group.
     */
    PVGroup(pvac::ClientProvider &provider, const std::vector<std::string> &pv_list);

    /**
     * @brief Constructs an empty PVGroup.
     * @param provider The PVA client provider.
     */
    PVGroup(pvac::ClientProvider &provider);

    /**
     * @brief Adds a new PV to the group.
     * @param pv_name The name of the PV to add.
     * @throws std::runtime_error if a PV with the same name already exists.
     */
    void add(const std::string &pv_name);

    /**
     * @brief Registers a variable to be updated by a specific PV in the group.
     * @tparam T The type of the variable to monitor.
     * @param pv_name The name of the PV to monitor.
     * @param var A reference to the variable that will be updated.
     * @throws std::runtime_error if the PV is not found in the group.
     */
    template <typename T> void set_monitor(const std::string &pv_name, T &var) {
        PVHandler &pv = this->get_pv(pv_name);
        pv.set_monitor(var);
    }

    /**
     * @brief Retrieves a PVHandler from the group by its name.
     * @param pv_name The name of the PV to retrieve.
     * @return A reference to the corresponding PVHandler object.
     * @throws std::runtime_error if the PV is not found.
     */
    PVHandler &get_pv(const std::string &pv_name);

    /**
     * @brief Provides array-like access to a PVHandler in the group.
     * @param pv_name The name of the PV to access.
     * @return A reference to the corresponding PVHandler object.
     * @throws std::runtime_error if the PV is not found.
     */
    PVHandler &operator[](const std::string &pv_name);

    /**
     * @brief Checks if any PV in the group has received new data.
     * @return True if new data is available in any monitor, false otherwise.
     */
    bool data_available();

    /**
     * @brief Adds a callback function to be executed after a PV is synchronized.
     * @param name The name of the PV to associate the callback with.
     * @param cb The callback function to execute.
     * @throws std::runtime_error if the PV is not found in the group.
     */
    void add_sync_callback(const std::string &name, std::function<void()> cb) {
	if (pv_map.count(name)) {
	    sync_callbacks_[name] = std::move(cb);
	} else {
	    throw std::runtime_error(name + "not in map");
	}
    }

  private:
    pvac::ClientProvider &provider_;                   ///< PVA client provider.
    std::unordered_map<std::string, std::unique_ptr<PVHandler>> pv_map; ///< Map of PVs by name.
    std::unordered_map<std::string, std::function<void()>> sync_callbacks_;
};
