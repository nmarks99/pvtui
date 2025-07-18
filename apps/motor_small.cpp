#include "pvtui.hpp"
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

    if (not args.macros_present({"P"})) {
	printf("Missing required macros\nRequired macros: P, M\n");
	return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    auto args1 = args;
    args1.macros["M"] = args1.macros.at("M1");
    
    auto args2 = args;
    args2.macros["M"] = args2.macros.at("M2");
    
    auto args3 = args;
    args3.macros["M"] = args2.macros.at("M3");

    SmallMotorDisplay display1(provider, args1);
    SmallMotorDisplay display2(provider, args2);
    SmallMotorDisplay display3(provider, args3);

    auto main_container = ftxui::Container::Horizontal({
	display1.get_container(),
	display2.get_container(),
	display3.get_container(),
    });
    auto main_renderer = ftxui::Renderer(main_container, [&] {
	return hbox({
	    display1.get_renderer(),
	    display2.get_renderer(),
	    display3.get_renderer(),
	}) | center;
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	if (display1.pv_update()) {
	    screen.PostEvent(Event::Custom);
	}
	if (display2.pv_update()) {
	    screen.PostEvent(Event::Custom);
	}
	if (display3.pv_update()) {
	    screen.PostEvent(Event::Custom);
	}
        loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

}
