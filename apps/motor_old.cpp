#include <cassert>
#include <cstdlib>
#include <string>

#include <pv/caProvider.h>
#include <pva/client.h>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
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

struct MotorFields {
    std::string desc;
    std::string rbv;
    std::string drbv;
    std::string val;
    std::string dval;
    std::string twr;
    std::string twv;
    std::string twf;
    std::string hlm;
    std::string llm;
    std::string dhlm;
    std::string dllm;
    std::string egu;
    std::string set;
    std::string hls;
    std::string lls;
    std::string spmg;
    std::string stop;
    std::string dmov;
    std::string able;
};

static constexpr std::string_view CLI_HELP_MSG = R"(
PVTUI Motor - Terminal UI for EPICS motors

Usage:
  pvtui_motor [options] 

Options:
  -h, --help                   Show this help message and exit.
  -m, --macro "k1=v1,k2=v2..." Define a macro variable. Same format as MEDM or caQtDM

Examples:
  pvtui_motor --macro "P=xxx:,M=m1"
    Start TUI display for motor xxx:m1 with a style similar to motorx.adl

For more details, visit: https://github.com/nmarks99/pvtui
)";


int main(int argc, char *argv[]) {
  
    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (args.flag("help") or args.flag("h")) {
	std::cout << CLI_HELP_MSG << std::endl;
	return EXIT_SUCCESS;
    }

    if (not args.macros_present({"P", "M"})) {
	printf("Missing required macros\nRequired macros: P, M\n");
	return EXIT_FAILURE;
    }

    // struct of motor PV names for convenience
    const MotorFields motor {
	.desc = args.macros.at("P") + args.macros.at("M") + ".DESC",
	.rbv  = args.macros.at("P") + args.macros.at("M") + ".RBV",
	.drbv = args.macros.at("P") + args.macros.at("M") + ".DRBV",
	.val  = args.macros.at("P") + args.macros.at("M") + ".VAL",
	.dval = args.macros.at("P") + args.macros.at("M") + ".DVAL",
	.twr  = args.macros.at("P") + args.macros.at("M") + ".TWR",
	.twv  = args.macros.at("P") + args.macros.at("M") + ".TWV",
	.twf  = args.macros.at("P") + args.macros.at("M") + ".TWF",
	.hlm  = args.macros.at("P") + args.macros.at("M") + ".HLM",
	.llm  = args.macros.at("P") + args.macros.at("M") + ".LLM",
	.dhlm = args.macros.at("P") + args.macros.at("M") + ".DHLM",
	.dllm = args.macros.at("P") + args.macros.at("M") + ".DLLM",
	.egu  = args.macros.at("P") + args.macros.at("M") + ".EGU",
	.set  = args.macros.at("P") + args.macros.at("M") + ".SET",
	.hls  = args.macros.at("P") + args.macros.at("M") + ".HLS",
	.lls  = args.macros.at("P") + args.macros.at("M") + ".LLS",
	.spmg = args.macros.at("P") + args.macros.at("M") + ".SPMG",
	.stop = args.macros.at("P") + args.macros.at("M") + ".STOP",
	.dmov = args.macros.at("P") + args.macros.at("M") + ".DMOV",
	.able = args.macros.at("P") + args.macros.at("M") + "_able",
    };

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // PVGroup to connect to all the PVs we need and manager monitors
    PVGroup pvgroup(provider, {
	motor.val,
	motor.rbv,
	motor.dval,
	motor.drbv,
	motor.twv,
	motor.twf,
	motor.twr,
	motor.desc,
	motor.hlm,
	motor.llm,
	motor.dhlm,
	motor.dllm,
	motor.egu,
	motor.set,
	motor.hls,
	motor.lls,
	motor.spmg,
	motor.stop,
	motor.dmov,
	motor.able
    });

    // Create monitors and ftxui components for all the PVs we will use.
    // Note any PVs used below must be defined in the pvgroup in order
    // for monitor to work.

    // tweak buttons, don't need readback
    auto twf_button = PVButton(pvgroup[motor.twf], " > ", 1);
    auto twr_button = PVButton(pvgroup[motor.twr], " < ", 1);

    // // adjustment to the default way PVInput renders
    // auto tform = [](ftxui::InputState s) {
	// // return s.element | center | color(Color::Black);
	// s.element |= ftxui::color(ftxui::Color::Black);
	// if (s.is_placeholder) {
	    // s.element |= ftxui::dim;
	// }
	// if (s.focused) {
	    // s.element |= ftxui::inverted;
	// } else if (s.hovered) {
	    // s.element |= ftxui::bgcolor(ftxui::Color::GrayLight);
	// }
	// return s.element;
    // };

    // user value input and monitor
    std::string val_string;
    auto val_input = PVInput(pvgroup[motor.val], val_string, PVPutType::Double);
    pvgroup.set_monitor(motor.val, val_string);

    // user high limit input and monitor
    std::string hlm_string;
    auto hlm_input = PVInput(pvgroup[motor.hlm], hlm_string, PVPutType::Double);
    pvgroup.set_monitor(motor.hlm, hlm_string);

    // user low limit input and monitor
    std::string llm_string;
    auto llm_input = PVInput(pvgroup[motor.llm], llm_string, PVPutType::Double);
    pvgroup.set_monitor(motor.llm, llm_string);

    // dial high limit input and monitor
    std::string dhlm_string;
    auto dhlm_input = PVInput(pvgroup[motor.dhlm], dhlm_string, PVPutType::Double);
    pvgroup.set_monitor(motor.dhlm, dhlm_string);

    // dial low limit input and monitor
    std::string dllm_string;
    auto dllm_input = PVInput(pvgroup[motor.dllm], dllm_string, PVPutType::Double);
    pvgroup.set_monitor(motor.dllm, dllm_string);

    // dial value input and monitor
    std::string dval_string;
    auto dval_input = PVInput(pvgroup[motor.dval], dval_string, PVPutType::Double);
    pvgroup.set_monitor(motor.dval, dval_string);

    // tweak value and monitor
    std::string twv_string;
    auto twv_input = PVInput(pvgroup[motor.twv], twv_string, PVPutType::Double);
    pvgroup.set_monitor(motor.twv, twv_string);

    // user readback value
    double rbv = 0.0;
    pvgroup.set_monitor<double>(motor.rbv, rbv);
    
    // dial readback value
    double drbv = 0.0;
    pvgroup.set_monitor<double>(motor.drbv, drbv);
    
    // done moving flag
    int dmov = 0;
    pvgroup.set_monitor<int>(motor.dmov, dmov);

    // EGU string readback
    std::string egu = "";
    pvgroup.set_monitor(motor.egu, egu);

    // string description
    std::string desc = "";
    auto desc_input = PVInput(pvgroup[motor.desc], desc, PVPutType::String);
    pvgroup.set_monitor<std::string>(motor.desc, desc);

    // High limit switch
    int hls = 0;
    std::string hls_box = unicode::rectangle(2, 1);
    pvgroup.set_monitor<int>(motor.hls, hls);
    
    // Low limit switch
    int lls = 0;
    std::string lls_box = unicode::rectangle(2, 1);
    pvgroup.set_monitor<int>(motor.lls, lls);

    // Enable/disable toggle
    PVEnum en_dis_enum;
    auto en_dis_menu = PVChoiceH(pvgroup[motor.able], en_dis_enum.choices, en_dis_enum.index);
    pvgroup.set_monitor(motor.able, en_dis_enum);

    // Use/Set toggle
    PVEnum use_set_enum;
    auto use_set_menu = PVChoiceH(pvgroup[motor.set], use_set_enum.choices, use_set_enum.index);
    pvgroup.set_monitor(motor.set, use_set_enum);

    // Stop, pause, move, go toggle
    PVEnum spmg_enum;
    auto spmg_menu = PVChoiceV(pvgroup[motor.spmg], spmg_enum.choices, spmg_enum.index);
    pvgroup.set_monitor(motor.spmg, spmg_enum);

    // use this PV for connection status. We assume if we can connect
    // to this, we can connect to all the PVs for this display
    auto &desc_pv = pvgroup[motor.desc];

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	desc_input,
	Container::Horizontal({
	    Container::Vertical({
		hlm_input,
		val_input,
		llm_input,
	    }),
	    Container::Vertical({
		dhlm_input,
		dval_input,
		dllm_input,
	    }),
	    Container::Vertical({
		spmg_menu,
	    }),
	}),
	Container::Horizontal({
	    twr_button, twv_input, twf_button
	}),
	Container::Horizontal({
	    en_dis_menu, use_set_menu,
	})
    });


    Decorator ColorDisabled = bgcolor(Color::RGBA(80,10,4,230)) | color(Color::Black);

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({

	    [&](){
		if (desc_pv.connected()) {
		    return text("");
		} else {
		    return text("Disconnected") | color(Color::Red);
		}
	    }() | center,

	    desc_input->Render() | color(Color::Black) | bgcolor(Color::RGB(210,210,210)) | size(WIDTH, EQUAL, 21) | center,
	    separatorEmpty(),

	    // 6 column hbox of vbox's
	    // none | none | user | dial | lims/egu | spmg
	    hbox({
		filler() | size(WIDTH, EQUAL, egu.length()+8),
		vbox({
		    text("User") | center,
		    hlm_input->Render() | center | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		    separatorEmpty(), 	
		    text(std::to_string(rbv)) | (use_set_enum.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)) | center,
		    separatorEmpty(), 	
		    val_input->Render() | center | (en_dis_enum.index==0 ? EPICSColor::EDIT : ColorDisabled) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		    separatorEmpty(), 	
		    llm_input->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		}),
		separatorEmpty(), 	
		vbox({
		    text("Dial") | center,
		    dhlm_input->Render() | center | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		    separatorEmpty(), 	
		    text(std::to_string(drbv)) | (use_set_enum.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)) | center,
		    separatorEmpty(), 	
		    dval_input->Render() | center | (en_dis_enum.index==0 ? EPICSColor::EDIT : ColorDisabled) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		    separatorEmpty(), 	
		    dllm_input->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		}),
		separatorEmpty(),
		vbox({
		    separatorEmpty(),
		    text(hls ? unicode::rectangle(2,1) : "") | color(Color::Red),
		    filler(),
		    text(egu) | EPICSColor::READBACK,
		    separatorEmpty(),
		    text(lls ? unicode::rectangle(2,1) : "") | color(Color::Red),
		}) | size(WIDTH, EQUAL, egu.length()),
		separatorEmpty(),
		vbox({
		    text(dmov ? "" : "Moving") | color(Color::DarkGreen) | italic | bold,
		    spmg_menu->Render() | EPICSColor::EDIT | center,
		    separatorEmpty(),
		}),
		separatorEmpty(),
	    }) | center,
	    
	    separatorEmpty(), 	

	    hbox({
		twr_button->Render() | color(Color::Black),
		separatorEmpty(),
		twv_input->Render() | center | EPICSColor::EDIT | size(WIDTH, EQUAL, 11) | center,
		separatorEmpty(),
		twf_button->Render() | color(Color::Black),
	    }) | center,

	    separatorEmpty(),

	    hbox({
		separatorEmpty(),
		use_set_menu->Render() | EPICSColor::EDIT,
		separatorEmpty(),
		en_dis_menu->Render() | EPICSColor::EDIT,
	    }) | center,

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
