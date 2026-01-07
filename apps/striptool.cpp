#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <sstream>

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

using PlotData = std::vector<PlotSeries<std::deque<double>>>;

struct Channel {
    Channel(VarWidget<double> var, Color color, double y0)
	: x(arange<std::deque<double>>(0, 5.0, 0.05)),
	y(std::deque<double>(x.size(), y0)), color(color), var(var)
    {}
    std::deque<double> x;
    std::deque<double> y;
    Color color;
    VarWidget<double> var;
};

int main(int argc, char *argv[]) {

    App app(argc, argv);
    if (app.args.help(CLI_HELP_MSG)) return EXIT_SUCCESS;

    std::vector<Channel> channels;
    std::vector<Color> colors = {Color::Red, Color::Green, Color::Blue};
    for (int i = 0; i < 3; i++) {
	std::stringstream pvname;
	pvname << "nmarks:m" << i+1 << ".RBV";
	VarWidget<double> rbv(app.pvgroup, pvname.str());
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	app.pvgroup.sync();
	if (rbv.connected()) {
	    Channel chan(std::move(rbv), colors.at(i), rbv.value());
	    channels.push_back(chan);
	} else {
	    throw std::runtime_error("not connected");
	}
    }

    PlotData data;
    for (auto& chan : channels) {
	data.push_back({&chan.x, &chan.y, &chan.color});
    }

    // Axis limits
    std::string ymin = "-5.0";
    std::string ymax = "5.0";
    std::string xmin = "0.0";
    std::string xmax = "5.0";
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
    PlotOption<std::deque<double>> op;
    op.data = &data;
    op.xmin = &xmin;
    op.xmax = &xmax;
    op.ymin = &ymin;
    op.ymax = &ymax;
    auto plot = Plot(op);

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	plot,
	ymin_inp,
	ymax_inp,
	xmin_inp,
	xmax_inp,
    });

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
	return hbox({
	    plot->Render() | (border | (plot->Active() ? color(Color::LightSkyBlue1) : color(Color::White))),

	    // sidebar
	    vbox({
		text("Axis limits") | underlined | bold,
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

		// plot legend showing connected PVs and their values
		text("Channels") | underlined | bold,
		[&]{
		    Elements legend_elems;
		    for (const auto& chan : channels) {
			legend_elems.push_back(hbox({
			    text(unicode::rectangle(1)) | color(chan.color),
			    text(chan.var.pv_name() + " = " + std::to_string(chan.var.value()))
			}));
		    }
		    return vbox(legend_elems);
		}()

	    }) | border | size(WIDTH, EQUAL, 30)
	});
    });


    app.main_loop = [&channels](App& app, const Component& renderer, int poll_ms) {
	Loop loop(&app.screen, renderer);
	while (!loop.HasQuitted()) {
	    app.pvgroup.sync();

	    for (auto& chan : channels) {
		chan.y.push_back(chan.var.value());
		chan.y.pop_front();
	    }

	    app.screen.PostEvent(Event::Custom);

	    loop.RunOnce();
	    std::this_thread::sleep_for(std::chrono::milliseconds(poll_ms));
	}
    };

    app.run(main_renderer);

    return 0;
}
