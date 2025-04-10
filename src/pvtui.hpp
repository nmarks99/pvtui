#pragma once

#include <cstdlib>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <charconv>
 
// #include "ftxui/component/captured_mouse.hpp"
// #include "ftxui/dom/elements.hpp"
// #include "ftxui/screen/color.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
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

ftxui::Component PVInput(pvac::ClientChannel &pv_channel, std::string &disp_str);

