#include <pv/caProvider.h>
#include <pva/client.h>

#include <pvtui/display_base.hpp>
#include <pvtui/pvtui.hpp>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

using namespace ftxui;
using namespace pvtui;

int main(int argc, char *argv[]) {

    App app(argc, argv);

    if (not app.args.macros_present({"P", "M"})) {
	printf("Missing required macro P, M\n");
	return EXIT_FAILURE;
    }

    // Create all the widgets
    InputWidget desc(app, "$(P)$(M).DESC", PVPutType::String);
    InputWidget twv(app, "$(P)$(M).TWV", PVPutType::Double);
    ButtonWidget twr(app, "$(P)$(M).TWR", " < ", 1);
    ButtonWidget twf(app, "$(P)$(M).TWF", " > ", 1);
    VarWidget<std::string> rbv(app, "$(P)$(M).RBV");

    auto main_container = Container::Vertical({
        desc.component(),
        Container::Vertical({
            twr.component(), twv.component(), twf.component()
        })
    });

    auto main_renderer = ftxui::Renderer(main_container, [&] {
        return vbox({
            desc.component()->Render() | EPICSColor::edit(desc) | size(WIDTH, EQUAL, 30),
            text(rbv.value()) | center | EPICSColor::readback(rbv),
            hbox({
                twr.component()->Render() | color(Color::Black),
                twv.component()->Render() | EPICSColor::edit(twv) | size(WIDTH, EQUAL, 10),
                twf.component()->Render() | color(Color::Black),
            })
        }) | center | EPICSColor::background();
    });

    app.run(main_renderer);
}

