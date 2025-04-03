#include <cstdlib>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>  // for operator+, to_string
 
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
 
using namespace ftxui;

namespace unicode {

constexpr std::string_view full_block = "█";
constexpr std::string_view green_circle = "🟢";
constexpr std::string_view red_circle = "🔴";

std::string rectangle(int width, int height) {
    std::string out;
    out.reserve(width*height*full_block.length()+height);
    for (int i = 0; i < height; i++){
	for (int j = 0; j < width; j++){
	   out.append(full_block);
	}
	out.append("\n");
    }
    return out;
}

}

Component PVButton(const std::string &label, pvac::ClientChannel &pv_channel, int value) {
    return Button(ButtonOption({
	.label = label,
	.on_click = [&pv_channel, value](){
	    pv_channel.put().set("value", value).exec();
	}
    }));
};

int main() {

    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");
    // const double current_val = channel.get()->getSubFieldT<epics::pvData::PVDouble>("value")->getAs<double>();
    // std::cout << "PV value = " << channel.get() << std::endl;

    // Create the screen. Interactive use the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Create some components
    int val = 50;
    const int VAL_MIN = 0;
    const int VAL_MAX = 100;
    const int BUTTON_INC = 5;
    const int SLIDER_INC = 1;
    
    pvac::ClientChannel m1_twf_chan(provider.connect("namSoft:m1.TWF"));
    pvac::ClientChannel m1_twr_chan(provider.connect("namSoft:m1.TWR"));
    auto plus_button = PVButton(" > ", m1_twf_chan, 1);
    auto minus_button = PVButton(" < ", m1_twr_chan, 1);

    pvac::ClientChannel m1_rbv_chan(provider.connect("namSoft:m1.RBV"));
    auto slider  = Slider(SliderOption<int>({
	.value = &val,
	.min = -10,
	.max = 10,
	.increment = SLIDER_INC,
	.color_active = Color::Blue,
	.color_inactive = Color::Blue,
    }));

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	slider,
	Container::Horizontal({
	    minus_button,
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
	    hbox({
		text([&val]()->std::string {
		    auto strval = std::to_string(val);
		    int nspaces = 4 - strval.length();
		    for (int i = 0; i < nspaces; i++) {
			strval.append(" ");
		    }
		    return strval;
		}()),
		slider->Render(),
	    }),
	    separatorEmpty(),
	    hbox({
		minus_button->Render() | size(WIDTH, EQUAL, 5),
		separatorEmpty(),
		plus_button->Render() | size(WIDTH, EQUAL, 5)
	    }) | hcenter,
	    separatorEmpty(),
	    // paragraph(unicode::rectangle(4,2)) | color(Color::DarkRed) | hcenter,
	}) | size(WIDTH, EQUAL, 50) | hcenter;
    });

   
    // Custom main loop
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	val = m1_rbv_chan.get()->getSubFieldT<epics::pvData::PVDouble>("value")->getAs<double>();
	screen.PostEvent(Event::Custom);
	loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // if we don't need to do anything special in the main loop, we can do
    // screen.Loop(main_renderer);

    return 0;
}
