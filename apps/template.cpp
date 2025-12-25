// A template to use as a starting point for
// creating a new PVTUI application

#include <pv/caProvider.h>
#include <pva/client.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

#include <pvtui/pvtui.hpp>

using namespace ftxui;
using namespace pvtui;

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    // print the help message if given "--help" or "-h" flags and return
    if (args.help("Help Message Here!")) return EXIT_SUCCESS;

    // return if not all the required macros are given
    if (not args.macros_present({"P", "R"})) {
        std::cerr << "Missing required macros: P, R" << std::endl;
        return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS PVA client
    // Start CAClientFactory so we can see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // PVGroup to manage all PVs for displays
    PVGroup pvgroup(provider);

    // Create all the widgets
    InputWidget desc(pvgroup, args, "$(P)$(R).DESC", PVPutType::String);

    // ftxui container to define interactivity of components
    auto main_container = ftxui::Container::Vertical({
        ftxui::Container::Vertical({})
    });

    // ftxui renderer defines the visual layout
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({}) | EPICSColor::BACKGROUND;
    });

    // Main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        // update monitored variables in the PVGroup, if new data post an ftxui event
        if (pvgroup.sync()) {
            screen.PostEvent(Event::Custom);
        }
        // run the ftxui main loop once and sleep
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }
}
