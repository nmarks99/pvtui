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

For more details, visit: https://github.com/nmarks99/pvtui
)";

const std::string PVTUI_ASCII_ART = R"(
██████╗ ██╗   ██╗████████╗██╗   ██╗██╗
██╔══██╗██║   ██║╚══██╔══╝██║   ██║██║
██████╔╝██║   ██║   ██║   ██║   ██║██║
██╔═══╝ ╚██╗ ██╔╝   ██║   ██║   ██║██║
██║      ╚████╔╝    ██║   ╚██████╔╝██║
╚═╝       ╚═══╝     ╚═╝    ╚═════╝ ╚═╝)";


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
	"xxx:long.VAL",
	"xxx:long.SCAN",
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
    // Create an input field to change and display the value of xxx:string
    auto tform = [](ftxui::InputState s) {
	return s.element | color(Color::Black) | bgcolor(Color::LightSeaGreen);
    };
    auto desc_input = PVInput(pvgroup["xxx:m1.DESC"], desc, PVPutType::String, tform);

    // Clickable buttons
    auto twf_button = PVButton(pvgroup["xxx:m1.TWF"], " > ", 1);
    auto twr_button = PVButton(pvgroup["xxx:m1.TWR"], " < ", 1);

    // SPMG is an enum type. We can use pvtui::PVEnum
    // and a pvtui::PVChoice or pvtui::Dropdown menu
    PVEnum spmg;
    std::vector<std::string> spmg_ops = {"Stop", "Pause", "Move", "Go"};
    int spmg_selected = 0;
    auto spmg_choiceh = PVChoiceH(pvgroup["xxx:m1.SPMG"], spmg_ops, spmg_selected);
    auto spmg_choicev = PVChoiceV(pvgroup["xxx:m1.SPMG"], spmg_ops, spmg_selected);

    // Dropdown menu
    // xxx:long is just a longout record
    int scan_choice = 0;
    std::vector<std::string> scan_menu_labels{"Passive", "Event", "I/O Intr",
	"10 second", "5 second", "2 second",
	"1 second", ".5 second", ".2 second", ".1 second"};
    auto scan_dropdown = PVDropdown(pvgroup["xxx:long.SCAN"], scan_menu_labels, scan_choice);

    // Main container to define interactivity of components
    // auto main_container = Container::Vertical({
	// Container::Horizontal({
	    // twr_button, twf_button,
	// }),
	// desc_input,
	// spmg_choiceh,
	// spmg_choicev,
	// scan_dropdown,
    // });

    auto tab1_container = Container::Vertical({
	Container::Horizontal({
	    twr_button, twf_button,
	}),
	desc_input,
    });

    auto tab1_renderer = Renderer(tab1_container, [&]{
	return vbox({
	    paragraphAlignCenter(PVTUI_ASCII_ART) | color(Color::DarkViolet),
	    separator(),
	    hbox({
		text("Display a string PV") | size(WIDTH, EQUAL, 20),
		separator(),
		text("xxx:m1.DESC = "),
		text(desc) | color(Color::Blue),
	    }),
	    separator(),
	    hbox({
		text("Display a float PV") | size(WIDTH, EQUAL, 20),
		separator(),
		text("xxx:m1.RBV = "),
		text(std::to_string(rbv)) | color(Color::Blue),
	    }),
	    separator(),
	    hbox({
		text("PVButton") | size(WIDTH, EQUAL, 20),
		separator(),
		text("xxx:m1.TWF/TWR"),
		separatorEmpty(),
		twr_button->Render() | color(Color::LightSeaGreen),
		separatorEmpty(),
		twf_button->Render() | color(Color::LightSeaGreen),
	    }),
	    separator(),
	    hbox({
		text("PVInput") | size(WIDTH, EQUAL, 20),
		separator(),
		text("xxx:m1.DESC  "),
		desc_input->Render() | size(WIDTH, EQUAL, 30)
	    }),
	    separator(),
	});
    });

    auto tab2_container = Container::Vertical({
	spmg_choiceh,
	spmg_choicev,
	scan_dropdown,
    });

    auto tab2_renderer = Renderer(tab2_container, [&]{
	return vbox({
	    paragraphAlignCenter(PVTUI_ASCII_ART) | color(Color::DarkViolet),
	    separator(),
	    hbox({
		text("PVChoiceH") | size(WIDTH, EQUAL, 20),
		separator(),
		spmg_choiceh->Render(),
	    }),
	    separator(),
	    hbox({
		text("PVChoiceV") | size(WIDTH, EQUAL, 20) | vcenter,
		separator(),
		spmg_choicev->Render(),
	    }),
	    separator(),
	    hbox({
		text("PVDropdown") | size(WIDTH, EQUAL, 20),
		separator(),
		text("xxx:long.SCAN   "),
		scan_dropdown->Render() | size(WIDTH, EQUAL, 15) | EPICSColor::EDIT,
	    }),
	    separator(),
	});
    });


    int tab_selected = 0;
    std::vector<std::string> reldis_labels = {
	"Screen 1", "Screen 2"
    };
    auto dropdown_op = DropdownOption({
	.radiobox = {
	    .entries = &reldis_labels,
	    .selected = &tab_selected,
	},
	.transform =
            [](bool open, ftxui::Element checkbox, ftxui::Element radiobox) {
	    if (open) {
		return ftxui::vbox({
		    checkbox | inverted,
		    radiobox | vscroll_indicator | frame |
		    size(HEIGHT, LESS_THAN, 10),
		    filler(),
		});
            }
            return vbox({
                checkbox,
                filler(),
            });
        }
    });
    // auto related_display = Dropdown(&dropdown_ops, &tab_selected);
    auto related_display = Dropdown(dropdown_op);
    auto reldis_renderer = Renderer(related_display, [&]{
	return vbox({
	    separatorEmpty(),
	    related_display->Render() | EPICSColor::MENU | size(WIDTH, EQUAL, 10) 
	});
    });

    auto main_container = Container::Vertical({
	Container::Tab({
	    tab1_renderer,
	    tab2_renderer,
	}, &tab_selected),
	Container::Vertical({
	    reldis_renderer,
	}),
    });

    auto main_renderer = Renderer(main_container, [&] {
	return vbox({
	    main_container->Render(),
	}) | size(WIDTH, EQUAL, 70) | size(HEIGHT, EQUAL, 70) | center;
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
