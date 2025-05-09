#pragma once

#include <cstdlib>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <unordered_map>
 
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/loop.hpp"

#include "pvgroup.hpp"
#include "argh.h"

namespace pvtui {

namespace unicode {
constexpr std::string_view full_block = "█";
constexpr std::string_view green_circle = "🟢";
constexpr std::string_view red_circle = "🔴";
std::string rectangle(int width, int height);
}

namespace EPICSColor {
using namespace ftxui;
static const Decorator EDIT = bgcolor(Color::RGB(87,202,228)) | color(Color::Black);
static const Decorator READBACK = color(Color::DarkBlue);
static const Decorator BACKGROUND = color(Color::RGB(196,196,196));
static const Decorator LINK = bgcolor(Color::RGB(148,148,228)) | color(Color::Black);
}

ftxui::Component PVButton(ProcessVariable &pv, const std::string &label, int value);


enum class PVPutType {
    Int,
    Double,
    String,
};

ftxui::Component PVInput(ProcessVariable &pv, std::string &disp_str, PVPutType put_type);

ftxui::Component PVChoiceH(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected);
ftxui::Component PVChoiceV(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected);

ftxui::Component PVDropdown(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected);

struct ArgParser {
  public:
    ArgParser(int argc, char *argv[]);

    ArgParser(int argc, char *argv[], const std::string &help_msg);

    bool macros_present(const std::vector<std::string> &macro_list) const;

    bool flag(const std::string &f) const;

    std::unordered_map<std::string, std::string> macros;
    std::string provider = "ca";

  private:
    argh::parser cmdl_;

    // splits a string to a vector of strings by the given delimiter
    std::vector<std::string> split_string(const std::string& input, char delimiter);

    // creates a map of macro names to macro values given EPICS style "P=xxx:,M=m1"
    std::unordered_map<std::string, std::string> get_macro_dict(std::string all_macros);
};
}
