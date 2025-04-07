#include <cstdlib>
#include <string>
#include <charconv>
#include <cassert>
#include <unordered_map>

#include <pva/client.h>
#include <pv/caProvider.h>
 
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/component/loop.hpp"

#include "pvtui.hpp"
 
using namespace ftxui;

struct Motor {
    pvac::ClientChannel rbv;
    pvac::ClientChannel val;
    pvac::ClientChannel twv;
    pvac::ClientChannel twf;
    pvac::ClientChannel twr;

    Motor(pvac::ClientProvider &provider, const std::string &prefix, const std::string &m) {
	rbv = provider.connect(prefix + m + ".RBV");
	val = provider.connect(prefix + m + ".VAL");
	twv = provider.connect(prefix + m + ".TWV");
	twf = provider.connect(prefix + m + ".TWF");
	twr = provider.connect(prefix + m + ".TWR");
    }
};


int main(int argc, char *argv[]) {


    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // Create Motor which contains pvac::ClientChannel's for most motor PVs
    Motor motor(provider, "namSoft:", "m1");


    std::string debug_string = "DEBUG";
    double rbv = 0.0;
    const int VAL_MIN = 0;
    const int VAL_MAX = 100;
    const int BUTTON_INC = 5;
    const int SLIDER_INC = 1;

    auto plus_button = PVButton(motor.twf, " > ", 1);
    auto minus_button = PVButton(motor.twr, " < ", 1);

    std::string val_string;
    auto val_input = PVInput(motor.val, val_string);

    std::string twv_string;
    auto twv_input = PVInput(motor.twv, twv_string);

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
	    text(std::to_string(rbv)) | color(Color::DodgerBlue1) | hcenter,
	    separatorEmpty(),
	    val_input->Render() | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10) | hcenter,
	    separatorEmpty(),
	    hbox({
		minus_button->Render() | size(WIDTH, EQUAL, 5),
		separatorEmpty(),
		twv_input->Render() | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 6) | center,
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

	rbv = motor.rbv.get()->getSubFieldT<epics::pvData::PVDouble>("value")->getAs<double>();
	screen.PostEvent(Event::Custom);

	loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
