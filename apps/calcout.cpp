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
    std::string odly;
    std::string dopt;
    std::string oopt;
    std::string ivoa;
    std::string ivov;
};

static constexpr std::string_view CLI_HELP_MSG = R"(
PVTUI calcout - Terminal UI for EPICS calcout record

Usage:
  pvtui_calcout [options] 

Options:
  -h, --help                   Show this help message and exit.

Examples:
    pvtui_calcout --macro "P=xxx:,C=userCalcOut1"

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
	.odly = args.macros.at("P") + args.macros.at("C") + ".ODLY",
	.dopt = args.macros.at("P") + args.macros.at("C") + ".DOPT",
	.oopt = args.macros.at("P") + args.macros.at("C") + ".OOPT",
	.ivoa = args.macros.at("P") + args.macros.at("C") + ".IVOA",
	.ivov = args.macros.at("P") + args.macros.at("C") + ".IVOV",
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
	calcout.odly,
	calcout.dopt,
	calcout.oopt,
	calcout.ivoa,
	calcout.ivov,
    });

    // Create monitors and ftxui components for all the PVs we will use.
    // Note any PVs used below must be defined in the pvgroup in order
    // for monitor to work.
    // std::string scan;
    PVEnum scan_enum;
    auto scan_menu = PVDropdown(pvgroup.get_pv(calcout.scan), scan_enum.choices, scan_enum.index);
    pvgroup.set_monitor(calcout.scan, scan_enum);
    
    std::string desc = "";
    auto desc_input = PVInput(pvgroup[calcout.desc], desc, PVPutType::String);
    pvgroup.set_monitor(calcout.desc, desc);

    std::string inpa = "";
    auto inpa_input = PVInput(pvgroup[calcout.inpa], inpa, PVPutType::String);
    pvgroup.set_monitor(calcout.inpa, inpa);

    std::string a_val;
    auto a_input = PVInput(pvgroup[calcout.a], a_val, PVPutType::Double);
    pvgroup.set_monitor(calcout.a, a_val);

    std::string inpb = "";
    auto inpb_input = PVInput(pvgroup[calcout.inpb], inpb, PVPutType::String);
    pvgroup.set_monitor<std::string>(calcout.inpb, inpb);
    
    std::string b_val;
    auto b_input = PVInput(pvgroup[calcout.b], b_val, PVPutType::Double);
    pvgroup.set_monitor(calcout.b, b_val);
    
    std::string inpc = "";
    auto inpc_input = PVInput(pvgroup[calcout.inpc], inpc, PVPutType::String);
    pvgroup.set_monitor<std::string>(calcout.inpc, inpc);
    
    std::string c_val;
    auto c_input = PVInput(pvgroup[calcout.c], c_val, PVPutType::Double);
    pvgroup.set_monitor(calcout.c, c_val);
   
    std::string inpd = "";
    auto inpd_input = PVInput(pvgroup[calcout.inpd], inpd, PVPutType::String);
    pvgroup.set_monitor<std::string>(calcout.inpd, inpd);
    
    std::string d_val;
    auto d_input = PVInput(pvgroup[calcout.d], d_val, PVPutType::Double);
    pvgroup.set_monitor(calcout.d, d_val);

    std::string calc;
    auto calc_input = PVInput(pvgroup[calcout.calc], calc, PVPutType::String);
    pvgroup.set_monitor(calcout.calc, calc);

    std::string ocal;
    auto ocal_input = PVInput(pvgroup[calcout.ocal], ocal, PVPutType::String);
    pvgroup.set_monitor(calcout.ocal, ocal);

    std::string out;
    auto out_input = PVInput(pvgroup[calcout.out], out, PVPutType::String);
    pvgroup.set_monitor(calcout.out, out);

    std::string flnk;
    auto flnk_input = PVInput(pvgroup[calcout.flnk], flnk, PVPutType::String);
    pvgroup.set_monitor(calcout.flnk, flnk);

    std::string val_rbv;
    pvgroup.set_monitor(calcout.val, val_rbv);
    
    std::string oval_rbv;
    pvgroup.set_monitor(calcout.oval, oval_rbv);

    PVEnum dopt_enum;
    auto dopt_menu = PVDropdown(pvgroup.get_pv(calcout.dopt), dopt_enum.choices, dopt_enum.index);
    pvgroup.set_monitor(calcout.dopt, dopt_enum);

    PVEnum ivoa_enum;
    auto ivoa_menu = PVDropdown(pvgroup.get_pv(calcout.ivoa), ivoa_enum.choices, ivoa_enum.index);
    pvgroup.set_monitor(calcout.ivoa, ivoa_enum);
    
    PVEnum oopt_enum;
    auto oopt_menu = PVDropdown(pvgroup.get_pv(calcout.oopt), oopt_enum.choices, oopt_enum.index);
    pvgroup.set_monitor(calcout.oopt, oopt_enum);

    std::string odly;
    auto odly_input = PVInput(pvgroup.get_pv(calcout.odly), odly, PVPutType::Double);
    pvgroup.set_monitor(calcout.odly, odly);

    std::string ivov;
    auto ivov_input = PVInput(pvgroup.get_pv(calcout.ivov), ivov, PVPutType::Double);
    pvgroup.set_monitor(calcout.ivov, ivov);

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	desc_input,
	scan_menu,
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
	calc_input,
	ocal_input,
	Container::Horizontal({
	    odly_input, oopt_menu, dopt_menu
	}),
	Container::Horizontal({
	    ivoa_menu, ivov_input, out_input, flnk_input,
	})
    });


    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
	    hbox({
		desc_input->Render() | color(Color::Black) |  bgcolor(Color::RGB(210,210,210)) | size(WIDTH, LESS_THAN, 32) | xflex,
		separatorEmpty(),
		text("(" + args.macros.at("P")+args.macros.at("C") + ")") | color(Color::Black)
	    }),
	    separatorEmpty(),
	    scan_menu->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 10) | color(Color::Black),
	    separatorEmpty(),

	    hbox({
		text("A") | color(Color::Black),
		separatorEmpty(),
		inpa_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		a_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	    separatorEmpty(),
	    hbox({
		text("B") | color(Color::Black),
		separatorEmpty(),
		inpb_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		b_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	    separatorEmpty(),
	    hbox({
		text("C") | color(Color::Black),
		separatorEmpty(),
		inpc_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		c_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),
	    separatorEmpty(),
	    hbox({
		text("D") | color(Color::Black),
		separatorEmpty(),
		inpd_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::LINK,
		separatorEmpty(),
		d_input->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::EDIT
	    }),

	    separator() | color(Color::Black),
	    
	    hbox({
		text("CALC") | color(Color::Black),
		filler() | size(WIDTH, EQUAL, 2),
		calc_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::EDIT,
		separatorEmpty(),
		text("   " + val_rbv) | EPICSColor::READBACK,
	    }) | (dopt_enum.index == 0 ? border : borderEmpty) | color(Color::Black),

	    hbox({
		text("OCAL") | color(Color::Black),
		filler() | size(WIDTH, EQUAL, 2),
		ocal_input->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::EDIT,
		separatorEmpty(),
		text("   " + oval_rbv) | EPICSColor::READBACK,
	    }) | (dopt_enum.index == 1 ? border : borderEmpty) | color(Color::Black),

	    separator() | color(Color::Black),

	    hbox({
		text("ODLY ") | color(Color::Black),
		odly_input->Render() | size(WIDTH, EQUAL, 4) | color(Color::Black) | bgcolor(Color::RGB(245, 179, 27)),
		filler() | size(WIDTH, EQUAL, 4),
		oopt_menu->Render() | size(WIDTH, EQUAL, 25) | color(Color::Black) | bgcolor(Color::RGB(245, 179, 27)),
		separatorEmpty(),
		dopt_menu->Render() | size(WIDTH, EQUAL, 10) | color(Color::Yellow) | bgcolor(Color::RGB(130, 95, 16)) | xflex,
	    }),

	    separatorEmpty(),

	    hbox({
		ivoa_menu->Render() | color(Color::Yellow) | bgcolor(Color::RGB(130, 95, 16)) | size(WIDTH, EQUAL, 15),
		text("  IVOV ") | color(Color::Black),
		ivov_input->Render() | color(Color::Black) | bgcolor(Color::RGB(245, 179, 27)) | size(WIDTH, EQUAL, 4),
		text("  OUT ") | color(Color::Black),
		out_input->Render() | EPICSColor::LINK | xflex
	    }),
	    separatorEmpty(),
	    hbox({
		text("FLNK ") | color(Color::Black),
		flnk_input->Render() | size(WIDTH, EQUAL, 18) | EPICSColor::LINK
	    }),

	}) | center | bgcolor(Color::RGB(196,196,196));
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	if (pvgroup.update()) {
	    screen.PostEvent(Event::Custom);
	}
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return EXIT_SUCCESS;
}
