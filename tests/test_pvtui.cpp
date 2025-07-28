#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/node.hpp"

using namespace ftxui;
using namespace pvtui;

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (not args.macros_present({"P", "M"})) {
	printf("Missing required macro P, M\n");
	return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS PVA client
    // Start CAClientFactory so we can see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // shared_ptr to PVGroup to manage all PVs for displays
    // std::shared_ptr<PVGroup> pvgroup = std::make_shared<PVGroup>(provider);
    PVGroup pvgroup(provider);

    // Create all the widgets
    InputWidget desc(pvgroup, args, "$(P)$(M).DESC", PVPutType::String);
    InputWidget twv(pvgroup, args, "$(P)$(M).TWV", PVPutType::Double);
    ButtonWidget twr(pvgroup, args, "$(P)$(M).TWR", " < ", 1);
    ButtonWidget twf(pvgroup, args, "$(P)$(M).TWF", " > ", 1);
    VarWidget<std::string> rbv(pvgroup, args, "$(P)$(M).RBV");

    auto main_container = Container::Vertical({
        desc.component(),
        Container::Vertical({
            twr.component(), twv.component(), twf.component()
        })
    });

    auto main_renderer = ftxui::Renderer(main_container, [&] {
        return vbox({
            desc.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 30),
            text(rbv.value()) | center | EPICSColor::READBACK,
            hbox({
                twr.component()->Render() | color(Color::Black),
                twv.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
                twf.component()->Render() | color(Color::Black),
            })
        }) | center | EPICSColor::BACKGROUND;
    });

    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        if (pvgroup.update()) {
            screen.PostEvent(Event::Custom);
        }
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }
}

