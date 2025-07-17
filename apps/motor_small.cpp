#include "pvtui.hpp"
#include "pvgroup.hpp"
#include "motor_display.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

using namespace ftxui;
using namespace pvtui;

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (not args.macros_present({"P", "M"})) {
	printf("Missing required macros\nRequired macros: P, M\n");
	return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    SmallMotorDisplay display(provider, args);

    auto main_container = display.get_container();
    auto main_renderer = ftxui::Renderer(main_container, [&] {
	return display.get_renderer();
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	if (display.pv_update()) {
	    screen.PostEvent(Event::Custom);
	}
        loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

}
