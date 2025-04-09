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

int main(int argc, char *argv[]) {

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // PVGroup to connect to all the PVs we need and manager monitors
    PVGroup pvgroup(provider, {
	"namSoft:m1.VAL",
	"namSoft:m1.RBV",
	"namSoft:m1.TWV",
	"namSoft:m1.TWF",
	"namSoft:m1.TWR",
    });

    std::string debug_string = "DEBUG";
    const int VAL_MIN = 0;
    const int VAL_MAX = 100;
    const int BUTTON_INC = 5;
    const int SLIDER_INC = 1;

    auto plus_button = PVButton(pvgroup.channels.at("namSoft:m1.TWF"), " > ", 1);
    auto minus_button = PVButton(pvgroup.channels.at("namSoft:m1.TWR"), " < ", 1);

    // FIX: make it so Input value can be double, etc, (template it)
    // we need the value we pass to PVInput to be the same we give to the monitor
    std::string val_string;
    double mon_val_double = 0.0;
    auto val_input = PVInput(pvgroup.channels.at("namSoft:m1.VAL"), val_string);
    pvgroup.create_monitor("namSoft:m1.VAL", mon_val_double);

    std::string twv_string;
    auto twv_input = PVInput(pvgroup.channels.at("namSoft:m1.TWV"), twv_string);

    double rbv = 0.0;
    pvgroup.create_monitor("namSoft:m1.RBV", rbv);

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
        val_input,
        Container::Horizontal({minus_button, twv_input, plus_button}),
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
	    separatorEmpty(),
	    text(std::to_string(rbv)) | color(Color::DodgerBlue1) | hcenter,
	    separatorEmpty(),
	    val_input->Render()  | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 10) | hcenter,
	    separatorEmpty(),
	    hbox({
		minus_button->Render() | size(WIDTH, EQUAL, 5), separatorEmpty(),
		twv_input->Render() | bgcolor(Color::Cyan) | size(WIDTH, EQUAL, 6) | center,
		separatorEmpty(), plus_button->Render() | size(WIDTH, EQUAL, 5)
	    }) | hcenter,
	    separatorEmpty(),
	    paragraph(debug_string) | color(Color::Yellow) | hcenter,
	}) | size(WIDTH, EQUAL, 20) | hcenter;
    });

    // Custom main loop
    double val_double = 0.0;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	pvgroup.update();
	val_string = std::to_string(mon_val_double);
        screen.PostEvent(Event::Custom);
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
