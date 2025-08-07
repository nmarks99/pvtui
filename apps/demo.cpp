// A template to use as a starting point for
// creating a new PVTUI application

#include <ftxui/dom/elements.hpp>
#include <pv/caProvider.h>
#include <pva/client.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

#include <string>

#include <pvtui/pvtui.hpp>

using namespace ftxui;
using namespace pvtui;

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (not args.macros_present({"P"})) {
	printf("Missing required macros\nRequired macros: P\n");
	return EXIT_FAILURE;
    }
    std::string P = args.macros.at("P");

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS PVA client
    // Start CAClientFactory so we can see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // PVGroup to manage all PVs for displays
    PVGroup pvgroup(provider);

    // Create all the widgets
    InputWidget string1(pvgroup, P+"string", PVPutType::String);
    InputWidget float1(pvgroup, P+"float", PVPutType::Double);
    InputWidget long1(pvgroup, P+"long", PVPutType::Integer);

    VarWidget<std::string> string1_rbv(pvgroup, P+"string");
    VarWidget<double> float1_rbv(pvgroup, P+"float");
    VarWidget<int> long1_rbv(pvgroup, P+"long");

    ChoiceWidget enum1_h(pvgroup, P+"enum", ChoiceStyle::Horizontal);
    ChoiceWidget enum1_v(pvgroup, P+"enum", ChoiceStyle::Vertical);
    ChoiceWidget enum1_d(pvgroup, P+"enum", ChoiceStyle::Dropdown);

    ButtonWidget plus(pvgroup, P+"add1.PROC", " + ");
    ButtonWidget minus(pvgroup, P+"subtract1.PROC", " - ");


    // ftxui container to define interactivity of components
    auto main_container = Container::Vertical({
        ftxui::Container::Vertical({
            string1.component(),
            float1.component(),
            Container::Horizontal({
                long1.component(),
                minus.component(),
                plus.component(),
            }),
            enum1_h.component(),
            enum1_v.component(),
            enum1_d.component(),
        })
    });

    // ftxui renderer defines the visual layout
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            separator() | color(Color::Black),
            hbox({
                vbox({
                    text("PV") | color(Color::Black) | size(WIDTH, EQUAL, 30),
                    separator() | color(Color::Black),
                    text(P+"string") | color(Color::Black),
                    separatorEmpty(),
                    text(P+"float") | color(Color::Black),
                    separatorEmpty(),
                    text(P+"long") | color(Color::Black),
                    separatorEmpty(),
                    text(P+"enum") | color(Color::Black),
                }) | size(WIDTH, EQUAL, 30),

                separator() | color(Color::Black),

                vbox({
                    text("Input") | color(Color::Black) | size(WIDTH, EQUAL, 30),
                    separator() | color(Color::Black),
                    string1.component()->Render(),
                    separatorEmpty(),
                    float1.component()->Render(),
                    separatorEmpty(),
                    hbox({
                        long1.component()->Render(),
                        minus.component()->Render() | color(Color::Black),
                        plus.component()->Render() | color(Color::Black),
                    }),
                    separatorEmpty(),
                    hbox({
                        vbox({ enum1_h.component()->Render() | xflex}),
                        separatorEmpty(),
                        vbox({ enum1_v.component()->Render() | xflex}),
                        separatorEmpty(),
                        vbox({ enum1_d.component()->Render() | xflex}),
                    }),
                }) | size(WIDTH, EQUAL, 30),

                separator() | color(Color::Black),

                vbox({
                    text("Readback") | color(Color::Black) | size(WIDTH, EQUAL, 30),
                    separator() | color(Color::Black),
                    text(string1_rbv.value()) | EPICSColor::READBACK,
                    separatorEmpty(),
                    text(std::to_string(float1_rbv.value())) | EPICSColor::READBACK,
                    separatorEmpty(),
                    text(std::to_string(long1_rbv.value())) | EPICSColor::READBACK,
                    separatorEmpty(),
                    text("Index = " + std::to_string(enum1_h.value().index)),
                    text("Choice = " + enum1_h.value().choice),
                }) | EPICSColor::READBACK | size(WIDTH, EQUAL, 30),
            }),

        }) | center | EPICSColor::BACKGROUND;
    });

    // Main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        // update monitored variables in the PVGroup, if new data post an ftxui event
        if (pvgroup.update()) {
            screen.PostEvent(Event::Custom);
        }
        // run the ftxui main loop once and sleep
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }
}
