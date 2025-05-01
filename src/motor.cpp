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
    std::string able;
};


int main(int argc, char *argv[]) {
   
    if (argc <= 2) {
	printf("Usage: %s <prefix> <motor>\n", argv[0]);
	return 0;
    }
    std::string ioc_prefix(argv[1]);
    std::string motor_pv(argv[2]);
    assert(ioc_prefix.length() > 0);
    assert(motor_pv.length() > 0);

    // struct of motor PV names for convenience
    const MotorFields motor {
	.desc = ioc_prefix + motor_pv + ".DESC",
	.rbv  = ioc_prefix + motor_pv + ".RBV",
	.drbv = ioc_prefix + motor_pv + ".DRBV",
	.val  = ioc_prefix + motor_pv + ".VAL",
	.dval = ioc_prefix + motor_pv + ".DVAL",
	.twr  = ioc_prefix + motor_pv + ".TWR",
	.twv  = ioc_prefix + motor_pv + ".TWV",
	.twf  = ioc_prefix + motor_pv + ".TWF",
	.hlm  = ioc_prefix + motor_pv + ".HLM",
	.llm  = ioc_prefix + motor_pv + ".LLM",
	.dhlm = ioc_prefix + motor_pv + ".DHLM",
	.dllm = ioc_prefix + motor_pv + ".DLLM",
	.egu  = ioc_prefix + motor_pv + ".EGU",
	.set  = ioc_prefix + motor_pv + ".SET",
	.hls  = ioc_prefix + motor_pv + ".HLS",
	.lls  = ioc_prefix + motor_pv + ".LLS",
	.spmg = ioc_prefix + motor_pv + ".SPMG",
	.stop = ioc_prefix + motor_pv + ".STOP",
	.able = ioc_prefix + motor_pv + "_able",
    };

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

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
	motor.able
    });

    // tweak buttons, don't need readback
    auto twf_button = PVButton(pvgroup.get_pv(motor.twf), " > ", 1);
    auto twr_button = PVButton(pvgroup.get_pv(motor.twr), " < ", 1);

    // user value input and monitor
    PVAny val_string;
    auto val_input = PVInput(pvgroup.get_pv(motor.val), val_string.value);
    pvgroup.set_monitor(motor.val, val_string);

    // user high limit input and monitor
    PVAny hlm_string;
    auto hlm_input = PVInput(pvgroup.get_pv(motor.hlm), hlm_string.value);
    pvgroup.set_monitor(motor.hlm, hlm_string);

    // user low limit input and monitor
    PVAny llm_string;
    auto llm_input = PVInput(pvgroup.get_pv(motor.llm), llm_string.value);
    pvgroup.set_monitor(motor.llm, llm_string);

    // dial high limit input and monitor
    PVAny dhlm_string;
    auto dhlm_input = PVInput(pvgroup.get_pv(motor.dhlm), dhlm_string.value);
    pvgroup.set_monitor(motor.dhlm, dhlm_string);

    // dial low limit input and monitor
    PVAny dllm_string;
    auto dllm_input = PVInput(pvgroup.get_pv(motor.dllm), dllm_string.value);
    pvgroup.set_monitor(motor.dllm, dllm_string);

    // dial value input and monitor
    PVAny dval_string;
    auto dval_input = PVInput(pvgroup.get_pv(motor.dval), dval_string.value);
    pvgroup.set_monitor(motor.dval, dval_string);

    // tweak value and monitor
    PVAny twv_string;
    auto twv_input = PVInput(pvgroup.get_pv(motor.twv), twv_string.value);
    pvgroup.set_monitor(motor.twv, twv_string);

    // user readback value
    double rbv = 0.0;
    pvgroup.set_monitor<double>(motor.rbv, rbv);
    
    // dial readback value
    double drbv = 0.0;
    pvgroup.set_monitor<double>(motor.drbv, drbv);

    // EGU string readback
    std::string egu = "";
    pvgroup.set_monitor(motor.egu, egu);

    // string description readback
    std::string desc = "";
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
    std::vector<std::string> en_dis_labels {"Enable", "Disable"};
    PVEnum en_dis_enum;
    auto en_dis_menu = PVChoiceH(pvgroup.get_pv(motor.able), en_dis_labels, en_dis_enum.index);
    pvgroup.set_monitor(motor.able, en_dis_enum);

    // Use/Set toggle
    std::vector<std::string> use_set_labels {"Use", "Set"};
    PVEnum use_set_enum;
    auto use_set_menu = PVChoiceH(pvgroup.get_pv(motor.set), use_set_labels, use_set_enum.index);
    pvgroup.set_monitor(motor.set, use_set_enum);

    // Stop, pause, move, go toggle
    std::vector<std::string> spmg_labels {"Stop", "Pause ", "Move", "Go"};
    PVEnum spmg_enum;
    auto spmg_menu = PVChoiceV(pvgroup.get_pv(motor.spmg), spmg_labels, spmg_enum.index);
    pvgroup.set_monitor(motor.spmg, spmg_enum);

    // use this PV for connection status. We assume if we can connect
    // to this, we can connect to all the PVs for this display
    auto &desc_pv = pvgroup.get_pv(motor.desc);

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
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

    // Event handler for main container
    main_container |= CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }
        return false;
    });

    Decorator ColorDisabled = bgcolor(Color::RGBA(80,10,4,230)) | color(Color::Black);

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
	    text(desc) | color(Color::Black) | center,

	    separatorEmpty() | color(Color::Black),

	    // 6 column hbox of vbox's
	    // none | none | user | dial | lims/egu | spmg
	    hbox({
		vbox({}) | size(WIDTH, EQUAL, 7),
		vbox({}) | size(WIDTH, EQUAL, egu.length()),
		separatorEmpty(),
		vbox({
		    text("User") | center,
		    hlm_input->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		    separatorEmpty(), 	
		    text(std::to_string(rbv)) | (use_set_enum.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)) | center,
		    separatorEmpty(), 	
		    val_input->Render()  | (en_dis_enum.index==0 ? EPICSColor::EDIT : ColorDisabled) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		    separatorEmpty(), 	
		    llm_input->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		}),
		separatorEmpty(), 	
		vbox({
		    text("Dial") | center,
		    dhlm_input->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		    separatorEmpty(), 	
		    text(std::to_string(drbv)) | (use_set_enum.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)) | center,
		    separatorEmpty(), 	
		    dval_input->Render()  | (en_dis_enum.index==0 ? EPICSColor::EDIT : ColorDisabled) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		    separatorEmpty(), 	
		    dllm_input->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		}),
		separatorEmpty(),
		vbox({
		    separatorEmpty(),
		    text(hls ? unicode::rectangle(2,1) : "") | color(Color::Red),
		    separatorEmpty(),
		    separatorEmpty(), 	
		    separatorEmpty(), 	
		    separatorEmpty(), 	
		    text(egu) | EPICSColor::READBACK,
		    separatorEmpty(),
		    text(lls ? unicode::rectangle(2,1) : "") | color(Color::Red),
		}) | size(WIDTH, EQUAL, egu.length()),
		separatorEmpty(),
		vbox({
		    separatorEmpty(),
		    spmg_menu->Render() | EPICSColor::EDIT | center,
		})
	    }) | center,
	    
	    separatorEmpty(), 	

	    hbox({
		twr_button->Render() | color(Color::Black),
		separatorEmpty(),
		twv_input->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 11) | center,
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

	    separatorEmpty(),

	    [&](){
		if (desc_pv.connected()) {
		    return text("Connected") | color(Color::Green);
		} else {
		    return text("Disconnected") | color(Color::Red);
		}
	    }() | center,


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

    return 0;
}
