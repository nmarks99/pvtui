#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <limits>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <ftxui-plot/plot.hpp>
#include <pvtui/pvtui.hpp>

static constexpr std::string_view CLI_HELP_MSG = R"(
pvtui_striptool - TUI StripTool plotting tool.

Usage:
    pvtui_striptool [options]

Options:
    -h, --help        Show this help message and exit.
    -m, --macro       Macros to pass to the UI

Examples:
    # TODO

For more details, visit: https://github.com/nmarks99/pvtui
)";

using namespace ftxui;
using namespace pvtui;

using PlotData = std::vector<PlotSeries>;

int main(int argc, char *argv[]) {

    auto screen = ScreenInteractive::Fullscreen();


    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (args.flag("help") or args.flag("h")) {
	std::cout << CLI_HELP_MSG << std::endl;
	return EXIT_SUCCESS;
    }

    // Instantiate EPICS PVA client
    // Start CAClientFactory so we can see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // PVGroup to manage all PVs for displays
    PVGroup pvgroup(provider);

    // Create vectors to store the data
    // this will need to be done dynamically to be able
    // to add more series to the plot at runtime
    const int N = int(20.0/0.1);
    std::vector<double> x1 = linspace(0, 20.0, N);
    std::vector<double> y1(N, std::numeric_limits<double>::quiet_NaN());
    Color color1 = Color::Red;

    PlotData data = {
	{&x1, &y1, &color1},
    };

    VarWidget<double> m1rbv(pvgroup, "nmrt:m1.RBV");

    // Axis limits
    std::string ymin = "-10";
    std::string ymax = "10";
    std::string xmin;
    std::string xmax;
    auto make_input = [&](std::string &str){
	auto op = InputOption{};
	op.multiline = false;
	op.content = &str;
	return Input(op);
    };
    auto ymin_inp = make_input(ymin);
    auto ymax_inp = make_input(ymax);
    auto xmin_inp = make_input(xmin);
    auto xmax_inp = make_input(xmax);

    // Create the plot component
    PlotOption op;
    op.data = &data;
    op.xmin = &xmin;
    op.xmax = &xmax;
    op.ymin = &ymin;
    op.ymax = &ymax;
    auto plot = Plot(op);

    // autoscale button
    auto button_op = ButtonOption::Simple();
    button_op.label = "Auto-Scale";
    button_op.on_click = [&](){
	plot->OnEvent(PlotEvent::AutoScale);
    };
    auto autoscale_button = Button(button_op);

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	plot,
	ymin_inp,
	ymax_inp,
	xmin_inp,
	xmax_inp,
	autoscale_button
    });

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
	return vbox({
	    plot->Render() | (border | (plot->Active() ? color(Color::LightSkyBlue1) : color(Color::White))),
	    hbox({
		vbox({
		    text("Axis limits") | underlined,
		    hbox({
			text("X Range: "),
			xmin_inp->Render() | size(WIDTH, EQUAL, 10) | bgcolor(Color::RGB(50,50,50)),
			separatorEmpty(),
			xmax_inp->Render() | size(WIDTH, EQUAL, 10) | bgcolor(Color::RGB(50,50,50)),
		    }),
		    hbox({
			text("Y Range: "),
			ymin_inp->Render() | size(WIDTH, EQUAL, 10) | bgcolor(Color::RGB(50,50,50)),
			separatorEmpty(),
			ymax_inp->Render() | size(WIDTH, EQUAL, 10) | bgcolor(Color::RGB(50,50,50)),
		    }),
		    separatorEmpty(),
		    autoscale_button->Render() | size(WIDTH, EQUAL, 12),
		}) | borderEmpty,
		separator(),
		vbox({
		    text("m1.RBV = " + std::to_string(m1rbv.value()))
		}),
	    }) | border | size(HEIGHT, EQUAL, 12),
	});
    });

    // main program loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	pvgroup.sync();

	y1.push_back(m1rbv.value());
	y1.erase(y1.begin());

	screen.PostEvent(Event::Custom);

	loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return 0;
}
