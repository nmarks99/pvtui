#include <cstdlib>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>  // for operator+, to_string
#include <charconv>
 
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for gauge, separator, text, vbox, operator|, Element, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Green, Color::Red
#include "ftxui/component/loop.hpp"

#include <pva/client.h>
#include <pv/caProvider.h>

#include "pvtui.hpp"
 
using namespace ftxui;

int main() {

    std::string debug_string = "DEBUG";

    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // Create the screen. Interactive use the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Create some components
    double rbv = 0.0;
    const int VAL_MIN = 0;
    const int VAL_MAX = 100;
    const int BUTTON_INC = 5;
    const int SLIDER_INC = 1;
    
    pvac::ClientChannel m1_twf_chan(provider.connect("namSoft:m1.TWF"));
    pvac::ClientChannel m1_twr_chan(provider.connect("namSoft:m1.TWR"));
    pvac::ClientChannel m1_rbv_chan(provider.connect("namSoft:m1.RBV"));
    pvac::ClientChannel m1_val_chan(provider.connect("namSoft:m1.VAL"));
    pvac::ClientChannel m1_twv_chan(provider.connect("namSoft:m1.TWV"));

    auto plus_button = PVButton(m1_twf_chan, " > ", 1);
    auto minus_button = PVButton(m1_twr_chan, " < ", 1);

    std::string val_string;
    auto val_input = Input(InputOption({
	.content = &val_string,
	.multiline = false,
	.on_enter = [&](){
	    double val_double;
	    auto res = std::from_chars(val_string.data(), val_string.data()+val_string.size(), val_double);
	    if (res.ec == std::errc()) {
		m1_val_chan.put().set("value", val_double).exec();
		debug_string = "";
	    } else {
		debug_string = "Conversion error";
	    }
	},
    }));

    std::string twv_string;
    auto twv_input = Input(InputOption({
	.content = &twv_string,
	.multiline = false,
	.on_enter = [&](){
	    double twv_double;
	    auto res = std::from_chars(twv_string.data(), twv_string.data()+twv_string.size(), twv_double);
	    if (res.ec == std::errc()) {
		m1_twv_chan.put().set("value", twv_double).exec();
		debug_string = "";
	    } else {
		debug_string = "Conversion error";
	    }
	},
    }));

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	val_input,
	Container::Horizontal({
	    minus_button,
	    twv_input,
	    plus_button
	}),
    });

    // Event handler for main container
    main_container |= CatchEvent([&](Event event){
	if (event == Event::Character('q')){
	    screen.Exit();
	    return true;
	}
	return false;
    });

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&]{
	return vbox({
	    separatorEmpty(),
	    text(std::to_string(rbv)) | color(Color::DarkBlue) | hcenter,
	    separatorEmpty(),
	    val_input->Render() | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10) | hcenter,
	    separatorEmpty(),
	    hbox({
		minus_button->Render() | size(WIDTH, EQUAL, 5),
		separatorEmpty(),
		separatorEmpty(),
		twv_input->Render() | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 5) | center,
		separatorEmpty(),
		separatorEmpty(),
		plus_button->Render() | size(WIDTH, EQUAL, 5)
	    }) | hcenter,
	    separatorEmpty(),
	    paragraph(debug_string) | color(Color::Yellow) | hcenter,
	}) | size(WIDTH, EQUAL, 20) | hcenter;
    });

   
    // Custom main loop
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {

	rbv = m1_rbv_chan.get()->getSubFieldT<epics::pvData::PVDouble>("value")->getAs<double>();
	screen.PostEvent(Event::Custom);

	loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // if we don't need to do anything special in the main loop, we can do
    // screen.Loop(main_renderer);

    return 0;
}
