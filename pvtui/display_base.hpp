#pragma once
#include <ftxui/component/component_base.hpp>
#include <pvtui/pvgroup.hpp>
#include <pv/caProvider.h>
#include <pva/client.h>

/**
 * @brief Base class for PVTUI displays
 *
 * Provides common functionality for managing a group of PVs and rendering a UI.
 */
class DisplayBase {
  public:
    /**
     * @brief Constructs a DisplayBase object.
     * @param pvgroup A reference to the PVGroup managing the PVs for this display.
     */
    DisplayBase(PVGroup &pvgroup) : pvgroup(pvgroup) {}

    /**
     * @brief Destroys the DisplayBase object.
     */
    virtual ~DisplayBase() = default;

    /**
     * @brief Checks if new data is available from and of the monitors in the group
     * @return True if any PV received new data, false otherwise.
     */
    virtual bool data_available() { return pvgroup.data_available(); };

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
    PVGroup &pvgroup; ///< Reference to the PVGroup instance.
};
