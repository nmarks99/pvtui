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


// Creates a ftxui::Dropdown component with a more minimal style
Component SmallDropdown(const std::vector<std::string> &labels, int &selected);

int main(int argc, char *argv[]) {
  
    // The pvtui library includes a minimal argument parser
    pvtui::ArgParser args(argc, argv);

    // Handle the "--help" or "-h" flags to show a help message
    if (args.flag("help") or args.flag("h")) {
	std::cout << CLI_HELP_MSG << std::endl;
	return EXIT_SUCCESS;
    }

    // For most applications we'd want to parse a "--macros" argument
    // for things like an IOC prefic "P", but we won't for this demo.
    // if (not args.macros_present({"P", "R"})) {
	// printf("Missing required macros\nRequired macros: P, R\n");
	// return EXIT_FAILURE;
    // }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    // If you want to connect to CA PVs as well, you must start the CAClientFactory
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // The PVGroup class makes it easy to manage PVs in an application like this.
    // We create a PVGroup with all the PVs we will use for this program, then later
    // we can link variables in our code to PVs and the values will be updated
    // automatically by calling pvgroup.update() in our main program loop
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
    // after calling pvgroup.update()
    double rbv = 0.0;
    pvgroup.set_monitor("xxx:m1.RBV", rbv);
    // You can alternatively monitor and store the value of most PV's as a string
    // regardless of their actual type. This can be useful for this
    // since everything is ultimately displayed as a string anyway
    // e.g.
    // std::string rbv_str = "";
    // pvgroup.set_monitor("xxx:m1.RBV", rbv_str);
  
    // .DESC field is actually a string type
    std::string desc = "";
    pvgroup.set_monitor("xxx:m1.DESC", desc);

    // Monitor xxx:string
    // Create an input field to change and display the value of xxx:string
    // auto tform = [](ftxui::InputState s) {
	// return s.element | color(Color::Black) | bgcolor(Color::LightSeaGreen);
    // };
    auto desc_input = PVInput(pvgroup["xxx:m1.DESC"], desc, PVPutType::String);

    // Clickable buttons
    auto twf_button = PVButton(pvgroup["xxx:m1.TWF"], " > ", 1);
    auto twr_button = PVButton(pvgroup["xxx:m1.TWR"], " < ", 1);

    // SPMG is an enum type. We can use pvtui::PVEnum
    // and a pvtui::PVChoice or pvtui::Dropdown menu
    PVEnum spmg;
    pvgroup.set_monitor("xxx:m1.SPMG", spmg);
    auto spmg_choiceh = PVChoiceH(pvgroup["xxx:m1.SPMG"], spmg.choices, spmg.index);
    auto spmg_choicev = PVChoiceV(pvgroup["xxx:m1.SPMG"], spmg.choices, spmg.index);

    // Dropdown menu
    // xxx:long is just a longout record for demonstration create
    // a dropdown menu to change its SCAN field
    PVEnum long_scan;
    pvgroup.set_monitor("xxx:long.SCAN", long_scan);
    auto scan_dropdown = PVDropdown(pvgroup["xxx:long.SCAN"], long_scan.choices, long_scan.index);

    // Dropdown menu to swtich between tabs
    int selected_tab = 0;
    std::vector<std::string> reldis_labels = {
	"Screen 1", "Screen 2"
    };
    auto related_display = SmallDropdown(reldis_labels, selected_tab);

    // The container defines the interactivity of components
    // Every component you want to interact with must be in here
    auto main_container = Container::Vertical({
	// Tab 1 ------------
	Container::Horizontal({
	    twr_button, twf_button,
	}),
	desc_input,
	// ------------------

	// Tab 2 ------------
	spmg_choiceh,
	spmg_choicev,
	scan_dropdown,
	// ------------------

	related_display,
    });

    // The renderer defines the visual layout
    auto main_renderer = Renderer(main_container, [&] {
	if (selected_tab == 0) {
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
		    desc_input->Render() | size(WIDTH, EQUAL, 30) |  EPICSColor::EDIT
		}),
		separator(),
		related_display->Render() | EPICSColor::MENU | size(WIDTH, EQUAL, 10)
	    }) | size(WIDTH, EQUAL, 70) | size(HEIGHT, EQUAL, 70) | center;
	} else {
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
		related_display->Render() | EPICSColor::MENU | size(WIDTH, EQUAL, 10)
	    }) | size(WIDTH, EQUAL, 70) | size(HEIGHT, EQUAL, 70) | center;
	}
    });

    // Main program loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	// update monitors for PV's in the PVGroup
	if (pvgroup.update()) {
	    // Force an update by posting an event
	    // only if there is new data (update returns true)
	    screen.PostEvent(Event::Custom);
	}
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return EXIT_SUCCESS;
}

Component SmallDropdown(const std::vector<std::string> &labels, int &selected) {
    auto dropdown_op = DropdownOption({
	.radiobox = {
	    .entries = &labels,
	    .selected = &selected,
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
        },
    });
    return Dropdown(dropdown_op);
}
