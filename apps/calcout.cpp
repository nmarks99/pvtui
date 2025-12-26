#include <pv/caProvider.h>
#include <pva/client.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

#include <pvtui/pvtui.hpp>

using namespace ftxui;
using namespace pvtui;

static constexpr std::string_view CLI_HELP_MSG = R"(
pvtui_calcout - Terminal UI for EPICS calcout record
Inspired by MEDM calcout record screens.

Usage:
  pvtui_calcout [options]

Options:
  -h, --help        Show this help message and exit.
  -m, --macro       Macros to pass to the UI (required: P, C)

Examples:
    pvtui_calcout --macro "P=xxx:,C=calcout1"

For more details, visit: https://github.com/nmarks99/pvtui
)";

pvac::ClientProvider init_epics_provider(const std::string &p) {
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(p);
    return provider;
}

struct Tui {
    Tui(int argc, char *argv[]) :
	args(ArgParser(argc, argv)),
	provider(init_epics_provider(args.provider)),
	pvgroup(PVGroup(provider)),
	screen(ScreenInteractive::Fullscreen()) {}

    void default_main_loop(const Component &renderer, int poll_period_ms) {
	Loop loop(&screen, renderer);
	while (!loop.HasQuitted()) {
	    if (pvgroup.sync()) {
		screen.PostEvent(Event::Custom);
	    }
	    loop.RunOnce();
	    std::this_thread::sleep_for(std::chrono::milliseconds(poll_period_ms));
	}
    }

    void run(const Component &renderer, int poll_period_ms = 100) {
	if (custom_loop) {
	    custom_loop(renderer, poll_period_ms);
	}
	default_main_loop(renderer, poll_period_ms);
    }

    std::function<void(const Component &renderer, int poll_period_ms)> custom_loop;
    ArgParser args;
    pvac::ClientProvider provider;
    PVGroup pvgroup;
    ScreenInteractive screen;
};

int main(int argc, char *argv[]) {

    Tui app(argc, argv);

    if (app.args.help(CLI_HELP_MSG)) return EXIT_SUCCESS;

    if (not app.args.macros_present({"P", "C"})) {
	printf("Missing required macros\nRequired macros: P, C\n");
	return EXIT_FAILURE;
    }

    ChoiceWidget scan(app.pvgroup, app.args, "$(P)$(C).SCAN", ChoiceStyle::Dropdown);
    InputWidget desc(app.pvgroup, app.args, "$(P)$(C).DESC", PVPutType::String);
    InputWidget prec(app.pvgroup, app.args, "$(P)$(C).PREC", PVPutType::Integer);
    InputWidget inpa(app.pvgroup, app.args, "$(P)$(C).INPA", PVPutType::String);
    InputWidget a_val(app.pvgroup, app.args, "$(P)$(C).A", PVPutType::Double);
    InputWidget inpb(app.pvgroup, app.args, "$(P)$(C).INPB", PVPutType::String);
    InputWidget b_val(app.pvgroup, app.args, "$(P)$(C).B", PVPutType::Double);
    InputWidget inpc(app.pvgroup, app.args, "$(P)$(C).INPC", PVPutType::String);
    InputWidget c_val(app.pvgroup, app.args, "$(P)$(C).C", PVPutType::Double);
    InputWidget inpd(app.pvgroup, app.args, "$(P)$(C).INPD", PVPutType::String);
    InputWidget d_val(app.pvgroup, app.args, "$(P)$(C).D", PVPutType::Double);
    InputWidget calc(app.pvgroup, app.args, "$(P)$(C).CALC", PVPutType::String);
    InputWidget ocal(app.pvgroup, app.args, "$(P)$(C).OCAL", PVPutType::String);
    InputWidget out(app.pvgroup, app.args, "$(P)$(C).OUT", PVPutType::String);
    InputWidget flnk(app.pvgroup, app.args, "$(P)$(C).FLNK", PVPutType::String);
    VarWidget<std::string> val(app.pvgroup, app.args, "$(P)$(C).VAL");
    VarWidget<std::string> oval(app.pvgroup, app.args, "$(P)$(C).OVAL");
    ChoiceWidget dopt(app.pvgroup, app.args, "$(P)$(C).DOPT", ChoiceStyle::Dropdown);
    ChoiceWidget ivoa(app.pvgroup, app.args, "$(P)$(C).IVOA", ChoiceStyle::Dropdown);
    ChoiceWidget oopt(app.pvgroup, app.args, "$(P)$(C).OOPT", ChoiceStyle::Dropdown);
    InputWidget odly(app.pvgroup, app.args, "$(P)$(C).ODLY", PVPutType::Double);
    InputWidget ivov(app.pvgroup, app.args, "$(P)$(C).IVOV", PVPutType::Double);
    ButtonWidget proc(app.pvgroup, app.args, "$(P)$(C).PROC", " PROC ");

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	desc.component(),
	scan.component(),
	proc.component(),
	prec.component(),
	inpa.component(), a_val.component(),
	inpb.component(), b_val.component(),
	inpc.component(), c_val.component(),
	inpd.component(), d_val.component(),
	calc.component(),
	ocal.component(),
	odly.component(), oopt.component(), dopt.component(),
	ivoa.component(), ivov.component(), out.component(), flnk.component(),
    });


    const Decorator COLORDISCON = color(Color::White) | bgcolor(Color::White);

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
	    hbox({
		desc.component()->Render() | color(Color::Black) |  bgcolor(Color::RGB(210,210,210)) | size(WIDTH, LESS_THAN, 32) | xflex,
		separatorEmpty(),
		text("(" + app.args.macros.at("P")+app.args.macros.at("C") + ")") | color(Color::Black)
	    }),
	    separatorEmpty(),
	    hbox({
		scan.component()->Render()
		    | EPICSColor::edit(scan)
		    | size(WIDTH, EQUAL, 10),
		separatorEmpty(),
		proc.component()->Render()
		    | EPICSColor::edit(proc)
		    | size(WIDTH, EQUAL, 8),
		filler(),
		text("PREC: ") | color(Color::Black),
		prec.component()->Render()
		    | EPICSColor::edit(prec)
		    | size(WIDTH, EQUAL, 3),
		separatorEmpty()
	    }),
	    separatorEmpty(),

	    hbox({
		text("A") | color(Color::Black),
		separatorEmpty(),
		inpa.component()->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::link(inpa),
		separatorEmpty(),
		a_val.component()->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::edit(a_val)
	    }),
	    separatorEmpty(),
	    hbox({
		text("B") | color(Color::Black),
		separatorEmpty(),
		inpb.component()->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::link(inpb),
		separatorEmpty(),
		b_val.component()->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::edit(b_val)
	    }),
	    separatorEmpty(),
	    hbox({
		text("C") | color(Color::Black),
		separatorEmpty(),
		inpc.component()->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::link(inpc),
		separatorEmpty(),
		c_val.component()->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::edit(c_val)
	    }),
	    separatorEmpty(),
	    hbox({
		text("D") | color(Color::Black),
		separatorEmpty(),
		inpd.component()->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::link(inpd),
		separatorEmpty(),
		d_val.component()->Render() | size(WIDTH, EQUAL, 13) | EPICSColor::edit(d_val)
	    }),

	    separator() | color(Color::Black),

	    hbox({
		text("CALC") | color(Color::Black),
		filler() | size(WIDTH, EQUAL, 2),
		calc.component()->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::edit(calc),
		separatorEmpty(),
		text("   " + val.value()) | EPICSColor::readback(val),
	    }) | (dopt.value().index == 0 ? border : borderEmpty) | color(Color::Black),

	    hbox({
		text("OCAL") | color(Color::Black),
		filler() | size(WIDTH, EQUAL, 2),
		ocal.component()->Render() | size(WIDTH, EQUAL, 32) | EPICSColor::edit(ocal),
		separatorEmpty(),
		text("   " + oval.value()) | EPICSColor::readback(oval),
	    }) | (dopt.value().index == 1 ? border : borderEmpty) | color(Color::Black),

	    separator() | color(Color::Black),

	    hbox({
		text("ODLY ") | color(Color::Black),
		odly.component()->Render()
		    | size(WIDTH, EQUAL, 4)
		    | EPICSColor::custom(odly, color(Color::Black) | bgcolor(Color::RGB(245, 179, 27))),
		filler() | size(WIDTH, EQUAL, 4),
		oopt.component()->Render()
		    | size(WIDTH, EQUAL, 25)
		    | EPICSColor::custom(oopt, color(Color::Black) | bgcolor(Color::RGB(245, 179, 27))),
		separatorEmpty(),
		dopt.component()->Render()
		    | EPICSColor::custom(dopt, color(Color::Yellow) | bgcolor(Color::RGB(130, 95, 16)))
		    | xflex
		    | size(WIDTH, EQUAL, 10)
	    }),

	    separatorEmpty(),

	    hbox({
		ivoa.component()->Render()
		    | EPICSColor::custom(ivoa, color(Color::Yellow) | bgcolor(Color::RGB(130, 95, 16)))
		    | size(WIDTH, EQUAL, 15),
		text("  IVOV ") | color(Color::Black),
		ivov.component()->Render()
		    | EPICSColor::custom(ivov, color(Color::Black) | bgcolor(Color::RGB(245, 179, 27)))
		    | size(WIDTH, EQUAL, 4),
		text("  OUT ") | color(Color::Black),
		out.component()->Render() | EPICSColor::link(out) | size(WIDTH, LESS_THAN, 17) | xflex
	    }),
	    separatorEmpty(),
	    hbox({
		text("FLNK ") | color(Color::Black),
		flnk.component()->Render() | size(WIDTH, EQUAL, 18) | EPICSColor::link(flnk)
	    }),

	}) | center | bgcolor(Color::RGB(196,196,196));
    });

    // Run with default main loop
    app.run(main_renderer);

    return EXIT_SUCCESS;
}
