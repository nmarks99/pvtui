#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

#include <pvtui/argh.h>
#include <pvtui/pvgroup.hpp>

namespace pvtui {

/**
 * @brief Provides unicode characters for TUI elements.
 */
namespace unicode {
constexpr std::string_view full_block = "█";    ///< Unicode character for a full block.
constexpr std::string_view green_circle = "🟢"; ///< Unicode character for a green circle.
constexpr std::string_view red_circle = "🔴";   ///< Unicode character for a red circle.

/**
 * @brief Generates a vertical string of full block characters.
 * @param len The number of full blocks (lines) to generate.
 * @return A string containing `len` full block characters, each on a new line.
 */
std::string rectangle(int len);
} // namespace unicode

/**
 * @brief Defines the data types for PV put operations for InputWidget
 */
enum class PVPutType {
    Integer,
    Double,
    String,
};

/**
 * @brief Style options for ChoiceWidget
 */
enum class ChoiceStyle {
    Vertical,
    Horizontal,
    Dropdown,
};

///< Type alias for input transformation function.
using InputTransform = std::function<ftxui::Element(ftxui::InputState)>;

/**
 * @brief Parses command-line arguments for PVTUI applications.
 *
 * Handles macro definitions similar to MEDM, and any additional flags
 */
class ArgParser {
  public:
    /**
     * @brief Constructs an ArgParser from command-line arguments.
     * @param argc Argument count.
     * @param argv Argument values.
     */
    ArgParser(int argc, char *argv[]);

    /**
     * @brief Constructs an ArgParser with a custom help message.
     * @param argc Argument count.
     * @param argv Argument values.
     * @param help_msg Custom help message (currently unused in constructor logic).
     */
    ArgParser(int argc, char *argv[], const std::string &help_msg);

    /**
     * @brief Checks if all specified macros are present in the parsed arguments.
     * @param macro_list A list of macro names to check.
     * @return True if all macros are present, false otherwise.
     */
    bool macros_present(const std::vector<std::string> &macro_list) const;

    /**
     * @brief Checks if a specific command-line flag is set.
     * @param f The flag name (e.g., "-h", "--version").
     * @return True if the flag is present, false otherwise.
     */
    bool flag(const std::string &f) const;

    /**
     * @brief Replaces macros in the given string and returns the result.
     * @param str A string with macros like $(P), $(R), etc.
     * @return A string with the macros replaced by the values in the macros dict
     */
    std::string replace(const std::string &str) const;

    std::unordered_map<std::string, std::string> macros; ///< Parsed macros (e.g., "P=VAL").
    std::string provider = "ca"; ///< The EPICS provider type (e.g., "ca", "pva").

  private:
    argh::parser cmdl_; ///< Internal argh parser instance.

    /**
     * @brief Splits a string by a given delimiter.
     * @param input The string to split.
     * @param delimiter The character to split by.
     * @return A vector of substrings.
     */
    std::vector<std::string> split_string(const std::string &input, char delimiter);

    /**
     * @brief Creates a map of macro names to values from an string similar to MEDM or caQtDM.
     * @param all_macros A string like "P=xxx:,M=m1".
     * @return An unordered map of macro names to values. Returns an empty map on parse error.
     */
    std::unordered_map<std::string, std::string> get_macro_dict(std::string all_macros);
};

/**
 * @brief Base class for all widget types used in the PVTUI system.
 *
 * Provides a standard interface for accessing PV names and FTXUI components,
 * while ensuring widgets register themselves with a PVGroup.
 */
class WidgetBase {
  public:
    virtual ~WidgetBase() = default;
    /**
     * @brief Get the PV name associated with the widget.
     * @return The fully expanded PV name.
     */
    std::string pv_name() const;

    /**
     * @brief Get the FTXUI component associated with the widget.
     * @return A valid FTXUI component.
     * @throws std::runtime_error if no component is set.
     */
    ftxui::Component component() const;

    bool connected() const;

  protected:
    /**
     * @brief Constructs a WidgetBase and registers the PV with a group.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param args ArgParser for macro expansion.
     * @param pv_name The macro-style PV name (e.g., "$(P)$(R)VAL").
     */
    WidgetBase(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name);

    /**
     * @brief Constructs a WidgetBase with a raw PV name.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param pv_name A fully-expanded PV name.
     */
    WidgetBase(PVGroup &pvgroup, const std::string &pv_name);

    std::string pv_name_;                                   ///< The PV name.
    ftxui::Component component_;                            ///< Underlying FTXUI component.
    bool connected_;                                        ///< Boolean for PV connection status
    std::unique_ptr<ConnectionMonitor> connection_monitor_; ///< Monitors PV connection status.
};

/**
 * @brief An editable input field linked to a PV.
 *
 * Supports typed PV put operations (int, double, string).
 */
class InputWidget : public WidgetBase {
  public:
    /**
     * @brief Constructs an InputWidget with macro expansion.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param args ArgParser for macro replacement.
     * @param pv_name The PV name with macros, e.g. "$(P)$(M).VAL".
     * @param put_type Specifies how the input value is written to the PV.
     */
    InputWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name,
                PVPutType put_type, InputTransform tf = nullptr);

    /**
     * @brief Constructs an InputWidget with an already expanded PV name.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param pv_name The PV name.
     * @param put_type Specifies how the input value is written to the PV.
     */
    InputWidget(PVGroup &pvgroup, const std::string &pv_name, PVPutType put_type);

    /**
     * @brief Get the current value of the input field.
     * @return The current string value.
     */
    std::string value() const;

  private:
    std::string value_; ///< Internal value monitored from the PV.
};

/**
 * @brief A simple button widget that writes a fixed value when pressed.
 */
class ButtonWidget : public WidgetBase {
  public:
    /**
     * @brief Constructs a ButtonWidget with macro expansion.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param args ArgParser for macro replacement.
     * @param pv_name The PV name with macros, e.g. "$(P)$(M).VAL".
     * @param label The text displayed on the button.
     * @param press_val The value written to the PV on press.
     */
    ButtonWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name,
                 const std::string &label, int press_val = 1);

    /**
     * @brief Constructs a ButtonWidget with an expanded PV name.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param pv_name The PV name.
     * @param label The text displayed on the button.
     * @param press_val The value written to the PV on press.
     */
    ButtonWidget(PVGroup &pvgroup, const std::string &pv_name, const std::string &label,
                 int press_val = 1);
};

/**
 * @brief A read-only variable display widget useful for readback values
 *
 * @tparam T The C++ type used to store the PV value.
 */
template <typename T> class VarWidget : public WidgetBase {
  public:
    /**
     * @brief Constructs a VarWidget with macro expansion.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param args ArgParser for macro replacement.
     * @param pv_name The PV name with macros, e.g. "$(P)$(M).VAL".
     */
    VarWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name)
        : WidgetBase(pvgroup, args, pv_name) {
        pvgroup.set_monitor(pv_name_, value_);
    }

    /**
     * @brief Constructs a VarWidget with a fully expanded PV name.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param pv_name The PV name.
     */
    VarWidget(PVGroup &pvgroup, const std::string &pv_name) : WidgetBase(pvgroup, pv_name) {
        pvgroup.set_monitor(pv_name_, value_);
    }

    /**
     * @brief Get the current value of the variable.
     * @return The current value stored in the widget.
     */
    T value() const { return value_; };

    /**
     * @brief Deleted component method. This widget does not have a UI element.
     */
    ftxui::Component component() const = delete;

  private:
    T value_; ///< Current value from the PV.
};

/**
 * @brief A widget for selecting an enum-style PV value from a list.
 *
 * Supports vertical, horizontal, or dropdown layout styles.
 */
class ChoiceWidget : public WidgetBase {
  public:
    /**
     * @brief Constructs a ChoiceWidget with macro expansion.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param args ArgParser for macro replacement.
     * @param pv_name The PV name with macros, e.g. "$(P)$(M).VAL".
     * @param style Layout style (vertical, horizontal, dropdown).
     */
    ChoiceWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name,
                 ChoiceStyle style);

    /**
     * @brief Constructs a ChoiceWidget with a PV name without macros.
     * @param pvgroup The PVGroup managing the PVs used in this widget.
     * @param pv_name The PV name.
     * @param style Layout style (vertical, horizontal, dropdown).
     */
    ChoiceWidget(PVGroup &pvgroup, const std::string &pv_name, ChoiceStyle style);

    /**
     * @brief Get the current enum value.
     * @return The current PVEnum value.
     */
    PVEnum value() const;

  private:
    PVEnum value_; ///< Current enum value from the PV.
};

/**
 * @brief Functions to generate FTXUI decorators for EPICS-style UI elements.
 * To align stylistically with MEDM, caQtDM etc, when PVs are disconnected, the widget
 * is drawn as a white rectangle
 */
namespace EPICSColor {
using namespace ftxui;

static const Decorator WHITE_ON_WHITE = bgcolor(Color::White) | color(Color::White);

inline Decorator edit(const WidgetBase &w) {
    return w.connected() ? bgcolor(Color::RGB(87, 202, 228)) | color(Color::Black) : WHITE_ON_WHITE;
}
inline Decorator menu(const WidgetBase &w) {
    return w.connected() ? bgcolor(Color::RGB(16, 105, 25)) | color(Color::White) : WHITE_ON_WHITE;
}
inline Decorator readback(const WidgetBase &w) {
    return w.connected() ? bgcolor(Color::RGB(196, 196, 196)) | color(Color::DarkBlue)
                         : WHITE_ON_WHITE;
}
inline Decorator link(const WidgetBase &w) {
    return w.connected() ? bgcolor(Color::RGB(148, 148, 228)) | color(Color::Black)
                         : WHITE_ON_WHITE;
}
inline Decorator custom(const WidgetBase &w, Decorator style) {
    return w.connected() ? style : WHITE_ON_WHITE;
}
inline Decorator background() { return bgcolor(Color::RGB(196, 196, 196)); }
} // namespace EPICSColor

} // namespace pvtui
