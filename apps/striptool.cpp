#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>
#include <string>

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

// std::deque<double> linspace(double start, double stop, size_t num_points) {
    // std::deque<double> out(num_points);
    // const double step = (stop - start) / (num_points - 1);
    // double val = start;
    // for (size_t i = 0; i < num_points; i++) {
	// out.at(i) = val;
	// val += step;
    // }
    // return out;
// }

using PlotData = std::vector<PlotSeries<std::deque<double>>>;

std::vector<Color> channel_colors = {
    Color::Red,
    Color::Green,
    Color::Blue,
};

struct Channel {
    std::deque<double> x;
    std::deque<double> y;
    VarWidget<double> var;
    Color color;
};
std::vector<std::unique_ptr<Channel>> channels;

int main(int argc, char *argv[]) {

    App app(argc, argv);

    if (app.args.help(CLI_HELP_MSG)) return EXIT_SUCCESS;
    if (app.args.positional_args().empty()) {
	std::cerr << "No PVs given\n";
	return EXIT_FAILURE;
    }

    auto pv_names = app.args.positional_args();
    if (pv_names.size() > 3) {
	std::cerr << "Only 3 PVs supported\n";
	return 1;
    }

    for (size_t i = 1; i < pv_names.size(); i++) {
	auto pv_name = pv_names.at(i);
	VarWidget<double> pv(app, pv_name);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	app.pvgroup.sync();
	if (pv.connected()) {
	    std::cout << "Adding " << pv_name << std::endl;
	    auto x = arange<std::deque<double>>(0, 5, 0.05);
	    auto chan = std::unique_ptr<Channel>(new Channel{
		x,
		std::deque<double>(x.size(), pv.value()),
		std::move(pv),
		channel_colors.at(i-1)
	    });
	    channels.push_back(std::move(chan));
	} else {
	    throw std::runtime_error("Could not connect to PV: " + pv_name);
	}
    }

    for (auto &c : channels) {
	std::cout << c->var.value() << std::endl;
    }

    // VarWidget<double> m1rbv(app, "nmarks:m1.RBV");
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // app.pvgroup.sync();
//
    // // Create vectors to store the data
    // // this will need to be done dynamically to be able
    // // to add more series to the plot at runtime
    // // const int N = int(20.0/0.1);
    // std::deque<double> x1 = arange<std::deque<double>>(0, 5, 0.05);
    // std::deque<double> y1(x1.size(), m1rbv.value());
    // Color color1 = Color::Red;
//
    // PlotData data = {
	// {&x1, &y1, &color1},
    // };
    PlotData data;

    for (auto &chan : channels) {
	data.push_back({&chan->x, &chan->y, &chan->color});
    }

    // Axis limits
    auto make_input = [&](std::string &str){
	auto op = InputOption{};
	op.multiline = false;
	op.content = &str;
	return Input(op);
    };
    std::string ymin = "-5.0";
    std::string ymax = "5.0";
    std::string xmin = "0.0";
    std::string xmax = "5.0";
    auto ymin_inp = make_input(ymin);
    auto ymax_inp = make_input(ymax);
    auto xmin_inp = make_input(xmin);
    auto xmax_inp = make_input(xmax);

    // New plot channel input
    std::string new_pv_name;
    auto new_chan_inp_op = InputOption{};
    new_chan_inp_op.content = &new_pv_name;
    new_chan_inp_op.multiline = false;
    // new_chan_inp_op.on_enter = [&]{
    // TODO
    // };

    auto new_chan_inp = Input(new_chan_inp_op);

    // Create the plot component
    PlotOption<std::deque<double>> op;
    op.data = &data;
    op.xmin = &xmin;
    op.xmax = &xmax;
    op.ymin = &ymin;
    op.ymax = &ymax;
    auto plot = Plot(op);

    // // autoscale button
    // auto button_op = ButtonOption::Simple();
    // button_op.label = "Auto-Scale";
    // button_op.on_click = [&](){
	// plot->OnEvent(PlotEvent::AutoScale);
    // };
    // auto autoscale_button = Button(button_op);

    // Main container to define interactivity of components
    auto main_container = Container::Vertical({
	plot,
	ymin_inp,
	ymax_inp,
	xmin_inp,
	xmax_inp,
	// autoscale_button
    });


    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {

	auto legend = Elements{};
	for (auto &c : channels) {
	    legend.push_back(
		hbox({
		    text(unicode::rectangle(1)) | color(c->color),
		    text("m1.RBV = " + std::to_string(c->var.value()))
		})
	    );
	};
	return hbox({
	    plot->Render() | (border | (plot->Active() ? color(Color::LightSkyBlue1) : color(Color::White))),
	    vbox({
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
		    // separatorEmpty(),
		    // autoscale_button->Render() | italic | size(WIDTH, EQUAL, 12),
		}),

		separator(),

		// Channel legend
		// vbox({legend})
		// hbox({
		    // text(unicode::rectangle(1)) | color(Color::Red),
		    // text("m1.RBV = " + std::to_string(m1rbv.value()))
		// }),
	    }) | border | size(WIDTH, EQUAL, 35),
	});
    });

    // custom main program loop
    app.main_loop = [&](App& app, const Component& renderer, int poll_ms) {
	Loop loop(&app.screen, renderer);
	while (!loop.HasQuitted()) {
	    app.pvgroup.sync();

	    for (auto &c : channels) {
		std::cout << c->var.pv_name() << " = " << c->var.value() << std::endl;
		// c.y.push_back(c.var.value());
		// c.y.pop_front();
	    }
	    // y1.push_back(m1rbv.value());
	    // y1.pop_front();

	    // app.screen.PostEvent(Event::Custom);
//
	    // loop.RunOnce();
	    std::this_thread::sleep_for(std::chrono::milliseconds(poll_ms));
	}
    };

    app.run(main_renderer);

    return 0;
}
