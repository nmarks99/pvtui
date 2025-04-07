#pragma once

#include <cstdlib>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>  // for operator+, to_string
 
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for gauge, separator, text, vbox, operator|, Element, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Green, Color::Red
#include "ftxui/component/loop.hpp"

#include <pva/client.h>
#include <pv/caProvider.h>

namespace unicode {
constexpr std::string_view full_block = "█";
constexpr std::string_view green_circle = "🟢";
constexpr std::string_view red_circle = "🔴";
std::string rectangle(int width, int height);
}

ftxui::Component PVButton(pvac::ClientChannel &pv_channel, const std::string &label, int value);
ftxui::Component PVInput(pvac::ClientChannel &pv_channel, std::string &strval);
