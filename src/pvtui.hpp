#pragma once

#include <cstdlib>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
 
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/loop.hpp"

#include "pvgroup.hpp"

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
}

ftxui::Component PVButton(ProcessVariable &pv, const std::string &label, int value);

ftxui::Component PVInput(ProcessVariable &pv, std::string &disp_str);

ftxui::Component PVChoiceH(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected);
ftxui::Component PVChoiceV(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected);

