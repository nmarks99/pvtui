#include <cassert>
#include <cstdlib>
#include <string>
#include <unordered_map>

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
};


int main(int argc, char *argv[]) {
   
    std::string debug_string = "DEBUG";

    if (argc == 1) {
	printf("Usage: %s <motor>\n", argv[0]);
	return 0;
    }

    std::string motor_pv_str(argv[1]);

    // struct of motor PV names for convenience
    const MotorFields motor {
	.desc = motor_pv_str + ".DESC",
	.rbv  = motor_pv_str + ".RBV",
	.drbv = motor_pv_str + ".DRBV",
	.val  = motor_pv_str + ".VAL",
	.dval = motor_pv_str + ".DVAL",
	.twr  = motor_pv_str + ".TWR",
	.twv  = motor_pv_str + ".TWV",
	.twf  = motor_pv_str + ".TWF",
	.hlm  = motor_pv_str + ".HLM",
	.llm  = motor_pv_str + ".LLM",
	.dhlm = motor_pv_str + ".DHLM",
	.dllm = motor_pv_str + ".DLLM",
	.egu  = motor_pv_str + ".EGU",
	.set  = motor_pv_str + ".SET",
	.hls  = motor_pv_str + ".HLS",
	.lls  = motor_pv_str + ".LLS",
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
    });

    // tweak buttons, don't need readback
    auto twf_button = PVButton(pvgroup.channels.at(motor.twf), " > ", 1);
    auto twr_button = PVButton(pvgroup.channels.at(motor.twr), " < ", 1);

    // user value input and monitor
    std::string val_string;
    auto val_input = PVInput(pvgroup.channels.at(motor.val), val_string);
    pvgroup.create_monitor(motor.val, val_string);

    // user high limit input and monitor
    std::string hlm_string;
    auto hlm_input = PVInput(pvgroup.channels.at(motor.hlm), hlm_string);
    pvgroup.create_monitor(motor.hlm, hlm_string);

    // user low limit input and monitor
    std::string llm_string;
    auto llm_input = PVInput(pvgroup.channels.at(motor.llm), llm_string);
    pvgroup.create_monitor(motor.llm, llm_string);

    // dial high limit input and monitor
    std::string dhlm_string;
    auto dhlm_input = PVInput(pvgroup.channels.at(motor.dhlm), dhlm_string);
    pvgroup.create_monitor(motor.dhlm, dhlm_string);

    // dial low limit input and monitor
    std::string dllm_string;
    auto dllm_input = PVInput(pvgroup.channels.at(motor.dllm), dllm_string);
    pvgroup.create_monitor(motor.dllm, dllm_string);

    // dial value input and monitor
    std::string dval_string;
    auto dval_input = PVInput(pvgroup.channels.at(motor.dval), dval_string);
    pvgroup.create_monitor(motor.dval, dval_string);

    // tweak value and monitor
    std::string twv_string;
    auto twv_input = PVInput(pvgroup.channels.at(motor.twv), twv_string);
    pvgroup.create_monitor(motor.twv, twv_string);

    // user readback value
    double rbv = 0.0;
    pvgroup.create_monitor<double>(motor.rbv, rbv);
    
    // dial readback value
    double drbv = 0.0;
    pvgroup.create_monitor<double>(motor.drbv, drbv);

    // EGU string readback
    std::string egu = "";
    pvgroup.create_monitor(motor.egu, egu);

    // string description readback
    std::string desc = "";
    pvgroup.create_monitor<std::string>(motor.desc, desc);

    // High limit switch
    int hls = 0;
    std::string hls_box = unicode::rectangle(2, 1);
    pvgroup.create_monitor<int>(motor.hls, hls);
    
    // Low limit switch
    int lls = 0;
    std::string lls_box = unicode::rectangle(2, 1);
    pvgroup.create_monitor<int>(motor.lls, lls);

    // Set/Use buttons
    int setuse = 0;
    auto set_button = PVButton(pvgroup.channels.at(motor.set), " Set ", 1);
    auto use_button = PVButton(pvgroup.channels.at(motor.set), " Use ", 0);

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
	}),
	Container::Horizontal({
	    twr_button, twv_input, twf_button
	}),
	Container::Horizontal({
	    use_button, set_button,
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
	    text(desc) | center,

	    separator(),

	    // 4 column hbox
	    // none | user | dial | other
	    hbox({
		vbox({}) | size(WIDTH, EQUAL, egu.length()),
		separatorEmpty(),
		vbox({
		    hlm_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10),
		    separatorEmpty(), 	
		    text(std::to_string(rbv)) | color(Color::Blue) | center,
		    separatorEmpty(), 	
		    val_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		    separatorEmpty(), 	
		    llm_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10),
		}),
		separatorEmpty(), 	
		vbox({
		    dhlm_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10),
		    separatorEmpty(), 	
		    text(std::to_string(drbv)) | color(Color::Blue) | center,
		    separatorEmpty(), 	
		    dval_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		    separatorEmpty(), 	
		    dllm_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10),
		}),
		separatorEmpty(),
		vbox({
		    text(hls ? unicode::rectangle(2,1) : "") | color(Color::Red),
		    separatorEmpty(), 	
		    separatorEmpty(), 	
		    separatorEmpty(), 	
		    separatorEmpty(), 	
		    text(egu),
		    separatorEmpty(),
		    text(lls ? unicode::rectangle(2,1) : "") | color(Color::Red),
		}) | size(WIDTH, EQUAL, egu.length()),
	    }) | center,
	    
	    separatorEmpty(), 	

	    hbox({
		twr_button->Render(),
		separatorEmpty(),
		twv_input->Render() | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 11) | center,
		separatorEmpty(),
		twf_button->Render(),
	    }) | center

	    // separatorEmpty(),
	    // paragraph(debug_string) | color(Color::Yellow) | hcenter,
	}) | center;
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 10;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	pvgroup.update();
        screen.PostEvent(Event::Custom);
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return 0;
}
