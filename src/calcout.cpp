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

struct CalcoutFields {
    std::string desc;
    std::string scan;

    std::string inpa;
    std::string inpb;
    std::string inpc;
    std::string inpd;

    std::string a;
    std::string b;
    std::string c;
    std::string d;

    std::string calc;
    std::string ocal;
    std::string val;
    std::string oval;

    std::string out;
    std::string flnk;
};

static constexpr std::string_view CLI_HELP_MSG = R"(
PVTUI calcout - Terminal UI for EPICS calcout record

Usage:
  pvtui_calcout [options] 

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

    if (not args.macros_present({"P", "C"})) {
	printf("Missing required macros\nRequired macros: P, C\n");
	return EXIT_FAILURE;
    }

    // struct of calcout PV names for convenience
    const CalcoutFields calcout {
	.desc = args.macros.at("P") + args.macros.at("C") + ".DESC",
	.scan = args.macros.at("P") + args.macros.at("C") + ".SCAN",
	.inpa = args.macros.at("P") + args.macros.at("C") + ".INPA",
	.inpb = args.macros.at("P") + args.macros.at("C") + ".INPB",
	.inpc = args.macros.at("P") + args.macros.at("C") + ".INPC",
	.inpd = args.macros.at("P") + args.macros.at("C") + ".INPD",
	.a = args.macros.at("P") + args.macros.at("C") + ".A",
	.b = args.macros.at("P") + args.macros.at("C") + ".B",
	.c = args.macros.at("P") + args.macros.at("C") + ".C",
	.d = args.macros.at("P") + args.macros.at("C") + ".D",
	.calc = args.macros.at("P") + args.macros.at("C") + ".CALC",
	.ocal = args.macros.at("P") + args.macros.at("C") + ".OCAL",
	.val = args.macros.at("P") + args.macros.at("C") + ".VAL",
	.oval = args.macros.at("P") + args.macros.at("C") + ".OVAL",
	.out = args.macros.at("P") + args.macros.at("C") + ".OUT",
	.flnk = args.macros.at("P") + args.macros.at("C") + ".FLNK",
    };

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // PVGroup to connect to all the PVs we need and manager monitors
    PVGroup pvgroup(provider, {
	calcout.desc,
	calcout.scan,
	calcout.inpa,
	calcout.inpb,
	calcout.inpc,
	calcout.inpd,
	calcout.a,
	calcout.b,
	calcout.c,
	calcout.d,
	calcout.calc,
	calcout.ocal,
	calcout.val,
	calcout.oval,
	calcout.out,
	calcout.flnk,
    });

    // Create monitors and ftxui components for all the PVs we will use.
    // Note any PVs used below must be defined in the pvgroup in order
    // for monitor to work.
    
    std::string desc = "";
    auto desc_input = PVInput(pvgroup.get_pv(calcout.desc), desc);
    pvgroup.set_monitor(calcout.desc, desc);

    std::string inpa = "";
    auto inpa_input = PVInput(pvgroup.get_pv(calcout.inpa), inpa);
    pvgroup.set_monitor(calcout.inpa, inpa);

    PVAny a_val;
    auto a_input = PVInput(pvgroup.get_pv(calcout.a), a_val.value);
    pvgroup.set_monitor(calcout.a, a_val);

    std::string inpb = "";
    auto inpb_input = PVInput(pvgroup.get_pv(calcout.inpb), inpb);
    pvgroup.set_monitor<std::string>(calcout.inpb, inpb);
    
    PVAny b_val;
    auto b_input = PVInput(pvgroup.get_pv(calcout.b), b_val.value);
    pvgroup.set_monitor(calcout.b, b_val);
    
    std::string inpc = "";
    auto inpc_input = PVInput(pvgroup.get_pv(calcout.inpc), inpc);
    pvgroup.set_monitor<std::string>(calcout.inpc, inpc);
    
    PVAny c_val;
    auto c_input = PVInput(pvgroup.get_pv(calcout.c), c_val.value);
    pvgroup.set_monitor(calcout.c, c_val);
   
    std::string inpd = "";
    auto inpd_input = PVInput(pvgroup.get_pv(calcout.inpd), inpd);
    pvgroup.set_monitor<std::string>(calcout.inpd, inpd);
    
    PVAny d_val;
    auto d_input = PVInput(pvgroup.get_pv(calcout.d), d_val.value);
    pvgroup.set_monitor(calcout.d, d_val);


    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	desc_input,
	Container::Horizontal({
	    inpa_input, a_input
	}),
	Container::Horizontal({
	    inpb_input, b_input
	}),
	Container::Horizontal({
	    inpc_input, c_input
	}),
	Container::Horizontal({
	    inpd_input, d_input
	}),
    });

    // Event handler for main container
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
	    desc_input->Render() | color(Color::Black) | bgcolor(Color::RGB(210,210,210)) | size(WIDTH, EQUAL, 45),
	    separatorEmpty(),

	    hbox({
		text("A") | bold | color(Color::Black),
		separatorEmpty(),
		inpa_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		a_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	    separatorEmpty(),
	    hbox({
		text("B") | bold | color(Color::Black),
		separatorEmpty(),
		inpb_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		b_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	    separatorEmpty(),
	    hbox({
		text("C") | bold | color(Color::Black),
		separatorEmpty(),
		inpc_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		c_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	    separatorEmpty(),
	    hbox({
		text("D") | bold | color(Color::Black),
		separatorEmpty(),
		inpd_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		d_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	}) | center | bgcolor(Color::RGB(196,196,196));
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	pvgroup.update();
        screen.PostEvent(Event::Custom);
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return EXIT_SUCCESS;
}
