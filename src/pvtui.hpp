#pragma once

#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include "argh.h"
#include "pvgroup.hpp"

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
 * @brief Custom FTXUI decorators for EPICS-style UI elements.
 */
namespace EPICSColor {
using namespace ftxui;
static const Decorator EDIT = bgcolor(Color::RGB(87, 202, 228)) | color(Color::Black); ///< Decorator for editable fields.
static const Decorator MENU = bgcolor(Color::RGB(16, 105, 25)) | color(Color::White); ///< Decorator for menu items.
static const Decorator READBACK = color(Color::DarkBlue);   ///< Decorator for read-only display values.
static const Decorator BACKGROUND = bgcolor(Color::RGB(196, 196, 196)); ///< Decorator for background elements.
static const Decorator LINK = bgcolor(Color::RGB(148, 148, 228)) | color(Color::Black); ///< Decorator for link-like elements.
} // namespace EPICSColor

/**
 * @brief Creates an FTXUI button component that puts an integer value to a PV.
 * @param pv The PVHandler to interact with.
 * @param label The text label for the button.
 * @param value The integer value to put to the PV when the button is clicked.
 * @return An FTXUI Component representing the button.
 */
ftxui::Component PVButton(PVHandler &pv, const std::string &label, int value);

/**
 * @brief Defines the data types for PV put operations.
 */
enum class PVPutType {
    Int,    ///< Integer type.
    Double, ///< Double-precision floating-point type.
    String, ///< String type.
};

using InputTransform =
    std::function<ftxui::Element(ftxui::InputState)>; ///< Type alias for input transformation function.

/**
 * @brief Creates an FTXUI input component for putting values to a PV.
 * @param pv The PVHandler to interact with.
 * @param disp_str The string holding the current display value (and input value).
 * @param put_type The type of value to put (Int, Double, or String).
 * @param transform Optional custom transformation for the input element's appearance.
 * @return An FTXUI Component representing the input field.
 */
ftxui::Component PVInput(PVHandler &pv, std::string &disp_str, PVPutType put_type,
                         InputTransform transform = nullptr);

/**
 * @brief Creates an FTXUI horizontal choice component (toggle menu) for PV enum values.
 * @param pv The PVHandler (expected to be an enum type).
 * @param labels A vector of string labels for each choice.
 * @param selected The index of the currently selected choice.
 * @return An FTXUI Component for horizontal choices.
 */
ftxui::Component PVChoiceH(PVHandler &pv, const std::vector<std::string> &labels, int &selected);

/**
 * @brief Creates an FTXUI vertical choice component (menu) for PV enum values.
 * @param pv The PVHandler (expected to be an enum type).
 * @param labels A vector of string labels for each choice.
 * @param selected The index of the currently selected choice.
 * @return An FTXUI Component for vertical choices.
 */
ftxui::Component PVChoiceV(PVHandler &pv, const std::vector<std::string> &labels, int &selected);

/**
 * @brief Creates an FTXUI dropdown menu component for PV enum values.
 * @param pv The PVHandler (expected to be an enum type).
 * @param labels A vector of string labels for each dropdown option.
 * @param selected The index of the currently selected option.
 * @return An FTXUI Component for a dropdown menu.
 */
ftxui::Component PVDropdown(PVHandler &pv, const std::vector<std::string> &labels, int &selected);

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
    std::string provider = "ca";                         ///< The EPICS provider type (e.g., "ca", "pva").

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

struct InputWidget {
  public:
    InputWidget(const std::shared_ptr<PVGroup> &pvgroup,
                const std::string &pv_name) :
    pv_name(pv_name),
    pvgroup_(pvgroup) {
        pvgroup->add(pv_name);
        pvgroup->set_monitor(pv_name, value);
        component_ = PVInput(pvgroup_->get_pv(pv_name), value, PVPutType::String);
    };

    std::string value = "";
    std::string pv_name = "";

    ftxui::Component component() {
        if (component_) {
            return component_;
        } else {
            throw std::runtime_error("No component defined for " + pv_name);
        }
    };

  private:
    std::shared_ptr<PVGroup> pvgroup_;
    ftxui::Component component_ = nullptr;
};



} // namespace pvtui
