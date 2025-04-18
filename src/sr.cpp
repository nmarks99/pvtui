#include <cassert>
#include <cstdlib>
#include <string>
#include <functional>

#include <pv/caProvider.h>
#include <pva/client.h>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

#include "pvtui.hpp"
#include "pvgroup.hpp"

using namespace ftxui;


class CurrentGraph {
  public:
    std::vector<int> operator()(int width, int height) const {
        return curr_vec;
        // std::vector<int> output(width);
        // for (int i = 0; i < width; ++i) {
            // output[i] = static_cast<int>(curr);
        // }
        // return output;
    }

    int curr = 0;
    std::vector<int> curr_vec;

};

std::vector<int> parse_string_to_vector(const std::string& input) {
    std::vector<int> result;
    std::stringstream ss(input);
    char ch;
    double value;
    ss >> ch;
    while (ss >> value) {
        result.push_back(value);
        ss >> ch;
        if (ch == ']') {
	    break;
	} 
    }
    return result;
}


// HACK: should get these from the PV
constexpr std::array<const char *, 7> stat_labels = {
    "State Unknown",
    "USER OPERATIONS",
    "SUPLEMENTAL TIME",
    "Not Used",
    "ASD Studies"
    "NO BEAM",
    "MAINTENANCE",
};

// TODO:
// constexpr std::array<const char *, 7> mode_labels = {
// };

int main(int argc, char *argv[]) {

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    PVGroup pvgroup(provider, {
        "S-DCCT:CurrentM",
        "S-DCCT:LifetimeM",
        "S-INJ:InjectionOperationM",
        "S-INJ:InjectionPeriodCounterM",
        "S:DesiredMode",
        "S:ActualMode",
        "NoOfShuttersOpenA"
        // "S:UserOpsCurrent", // array
    });

    std::string current = "";
    pvgroup.set_monitor("S-DCCT:CurrentM", current);

    std::string lifetime = "";
    pvgroup.set_monitor("S-DCCT:LifetimeM", lifetime);

    std::string injection_status = "";
    pvgroup.set_monitor("S-INJ:InjectionOperationM", injection_status);

    std::string injection_period;
    pvgroup.set_monitor("S-INJ:InjectionPeriodCounterM", injection_period);

    int desired_mode = 0;
    pvgroup.set_monitor("S:DesiredMode", desired_mode);
    
    std::string actual_mode = "";
    pvgroup.set_monitor("S:ActualMode", actual_mode);

    std::string num_shutters_open = "";
    pvgroup.set_monitor("NoOfShuttersOpenA", num_shutters_open);

    // Just using container for "q" to quit. Maybe there's a better way?
    auto main_container = Container::Vertical({});
    main_container |= CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }
        return false;
    });

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("Current: " + current), 
            text("Lifetime: " + lifetime),
            separatorEmpty(),
            text("Injection Status: " + injection_status),
            text("Swapout In: " + injection_period + " sec."),
            separatorEmpty(),
            text("Machine Status: " + std::string(stat_labels.at(desired_mode))),
            text("Operating Mode: " + actual_mode),
            text("Shutters Open: " + num_shutters_open),
        });
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 10;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        pvgroup.update();
        screen.PostEvent(Event::Custom);
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return 0;
}
