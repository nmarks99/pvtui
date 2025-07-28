#pragma once
#include "ftxui/component/component_base.hpp"
#include "pvgroup.hpp"
#include <pv/caProvider.h>
#include <pva/client.h>

/**
 * @brief Placeholder struct for cases where no specific value is needed or available.
 */
struct NoValue {};

/**
 * @brief Convenience struct to hold a PV name, a C++ variable for its value, and an associated
 * FTXUI Component.
 * @tparam T The type of the C++ variable that will hold the PV's value.
 */
template <typename T> struct PVWidget {
  public:
    std::string pv_name = ""; ///< The name of the associated PV.
    T value;                  ///< The C++ variable that holds the PV's current value.

    /**
     * @brief Sets the FTXUI Component associated with this widget.
     * @param component The FTXUI Component to link.
     */
    void set_component(ftxui::Component component) { component_ = component; };

    /**
     * @brief Retrieves the FTXUI Component associated with this widget.
     * @return The FTXUI Component.
     * @throw std::runtime_error If no component has been set for this widget.
     */
    ftxui::Component component() {
        if (component_) {
            return component_;
        } else {
            throw std::runtime_error("No component defined for " + pv_name);
        }
    };

  private:
    ftxui::Component component_ = nullptr; ///< The FTXUI Component instance.
};

/**
 * @brief Base class for PVTUI displays
 *
 * Provides common functionality for managing a group of PVs and rendering a UI.
 */
class DisplayBase {
  public:
    /**
     * @brief Constructs a DisplayBase object.
     * @param pvgroup A shared pointer to the PVGroup managing the PVs for this display.
     */
    // DisplayBase(const std::shared_ptr<PVGroup> &pvgroup);
    DisplayBase(PVGroup &pvgroup);

    /**
     * @brief Destroys the DisplayBase object.
     */
    virtual ~DisplayBase() = default;

    /**
     * @brief Updates all Process Variables managed by the associated PVGroup.
     * @return True if any PV received new data, false otherwise.
     */
    virtual bool pv_update();

    /**
     * @brief Pure virtual function to get the FTXUI Element for rendering the display.
     * @return An FTXUI Element representing the display's visual content.
     */
    virtual ftxui::Element get_renderer() = 0;

    /**
     * @brief Pure virtual function to get the FTXUI Component container for the display.
     * @return An FTXUI Component that acts as the main container for the display's interactive
     * elements.
     */
    virtual ftxui::Component get_container() = 0;

  protected:
    PVGroup &pvgroup;         ///< Shared pointer to the PVGroup instance.
    static constexpr bool MonitorOn = true;   ///< for clarity when calling connect_pv
    static constexpr bool MonitorOff = false; ///< for clarity when calling connect_pv

    template <typename T>
    void connect_pv(PVWidget<T> &widget, const std::string &pv_name, bool monitor) {
        widget.pv_name = pv_name;
        pvgroup.add(pv_name);
        if constexpr (std::is_constructible_v<MonitorPtr, T *>) {
            // This block is only compiled if T* can be held by MonitorPtr
            if (monitor) {
                pvgroup.set_monitor(pv_name, widget.value);
            }
        } else {
            if (monitor) {
                throw std::logic_error(pv_name + " widget value" + " of type " +
                                       typeid(widget.value).name() + " cannot be monitored");
            }
        }
    }
};
