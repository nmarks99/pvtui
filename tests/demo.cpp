#include <cassert>
#include <cstdlib>
#include <string>

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
using namespace pvtui;

static constexpr std::string_view CLI_HELP_MSG = R"(
PVTUI demo - Terminal UI for EPICS

Usage:
  pvtui_demo [options] 

Options:
  -h, --help                   Show this help message and exit.

Examples:
    ???

For more details, visit: https://github.com/nmarks99/pvtui
)";


int main(int argc, char *argv[]) {
  
    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (args.flag("help") or args.flag("h")) {
	std::cout << CLI_HELP_MSG << std::endl;
	return EXIT_SUCCESS;
    }

    // if (not args.macros_present({"P", "C"})) {
	// printf("Missing required macros\nRequired macros: P, C\n");
	// return EXIT_FAILURE;
    // }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    PVGroup pvgroup(provider, {
	"xxx:m1.VAL",
	"xxx:m1.RBV",
	"xxx:m1.TWV",
	"xxx:m1.TWF",
	"xxx:m1.TWR",
	"xxx:m1.PREC",
	"xxx:m1.DESC",
	"xxx:m1.SPMG",
	"xxx:m1.VELO",
	"xxx:long",
	"xxx:string",
	"xxx:float",
	"xxx:enum",
	"xxx:double_array",
	"xxx:long_array",
	"xxx:string_array"
    });

    // Set monitors like this
    // rbv double value will be updated when xxx:m1.RBV changes
    double rbv = 0.0;
    pvgroup.set_monitor("xxx:m1.RBV", rbv);

    // You can alternatively monitor and store the value of most PV's as a string
    // regardless of their actual type. This can be useful for this
    // library since everything is displayed as a string anyway
    // e.g.
    // std::string rbv_str = "";
    // pvgroup.set_monitor("xxx:m1.RBV", rbv_str);
  
    // .DESC field is actually a string type
    std::string desc = "";
    pvgroup.set_monitor("xxx:m1.DESC", desc);


    // Monitor xxx:string
    std::string xxxstr = "";
    pvgroup.set_monitor("xxx:string", xxxstr);
    // Create an input field to change and display the value of xxx:string
    auto tform = [](ftxui::InputState s) {
	return s.element | color(Color::White);
    };
    auto string_input = PVInput(pvgroup["xxx:string"], xxxstr, PVPutType::String, tform);

    // Clickable buttons
    auto twf_button = PVButton(pvgroup["xxx:m1.TWF"], " > ", 1);
    auto twr_button = PVButton(pvgroup["xxx:m1.TWR"], " < ", 1);


    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	Container::Horizontal({
	    twr_button, twf_button,
	}),
	Container::Horizontal({
	    string_input,
	}),
    });

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
	    separator(),
	    hbox({
		text("xxx:m1.DESC") | size(WIDTH, EQUAL, 20), separator(),
		text(desc),
	    }),
	    separator(),
	    hbox({
		text("xxx:m1.RBV") | size(WIDTH, EQUAL, 20), separator(),
		text(std::to_string(rbv)),
	    }),
	    separator(),
	    hbox({
		text("xxx:m1.TWR/TWF") | size(WIDTH, EQUAL, 20),
		separator(),
		text("Click the buttons to tweak the motor: "),
		separatorEmpty(),
		twr_button->Render() | color(Color::Orange1),
		separatorEmpty(),
		twf_button->Render() | color(Color::Orange1),
	    }),
	    separator(),
	    hbox({
		text("xxx:string") | size(WIDTH, EQUAL, 20),
		separator(),
		string_input->Render()
	    }),
	    separator(),
        }) | size(WIDTH, EQUAL, 80);
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {

	// update monitors for PV's in the PVGroup
	pvgroup.update();

	// Force an update by posting an event
	// and run the FTXUI main loop once
        screen.PostEvent(Event::Custom);
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return EXIT_SUCCESS;
}
