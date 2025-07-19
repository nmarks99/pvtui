#include <stdexcept>
#include <vector>
#include <charconv>

#include "pvtui.hpp"
#include "motor_display.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

using namespace ftxui;
using namespace pvtui;

enum class MotorDisplayType {
    Small,
    Medium,
    Setup,
    Multi,
};

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    MotorDisplayType display_type = MotorDisplayType::Small;

    if (args.flag("medium")) {
	display_type = MotorDisplayType::Medium;
    } else if (args.flag("setup")) {
	display_type = MotorDisplayType::Setup;
    }

    if (not args.macros_present({"P"})) {
	printf("Missing required macro P\n");
	return EXIT_FAILURE;
    }

    if (args.macros_present({"M1"})) {
	display_type = MotorDisplayType::Multi;
    } else if (not args.macros_present({"M"})) {
	printf("Missing required macro M or (M1,M2,...)\n");
	return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    std::vector<std::unique_ptr<SmallMotorDisplay>> displays;
    std::vector<int> motor_num_vec;
    std::vector<ArgParser> args_vec;
    switch (display_type) {
	case MotorDisplayType::Multi:
	    for (const auto &[k, v] : args.macros) {
		size_t ind = k.find("M");
		if (ind != std::string::npos) {
		    const std::string num_str = std::string(k.begin()+ind+1, k.end());
		    int num;
		    if (std::from_chars(num_str.data(), num_str.data()+num_str.size(), num).ec != std::errc()) {
			throw std::runtime_error("Invalid macro " + k);
		    }
		    motor_num_vec.push_back(num);
		}
	    }
	    std::sort(motor_num_vec.begin(), motor_num_vec.end());
	    for (const auto &v : motor_num_vec) {
		auto args_n = args;
		args_n.macros["M"] = args_n.macros.at("M" + std::to_string(v));
		args_vec.push_back(args_n);
		displays.emplace_back(std::make_unique<SmallMotorDisplay>(provider, args_n));
	    }
	    break;
	case MotorDisplayType::Small:
	    // std::cout << "Small not implemented" << std::endl;
	    // return EXIT_FAILURE;
	    break;
	case MotorDisplayType::Medium:
	    // std::cout << "Medium not implemented" << std::endl;
	    // return EXIT_FAILURE;
	    break;
	case MotorDisplayType::Setup:
	    std::cout << "Setup not implemented" << std::endl;
	    return EXIT_FAILURE;
	    break;
    }

    MediumMotorDisplay display(provider, args);
    auto main_container = display.get_container();
    auto main_renderer = Renderer(main_container, [&]{
	return display.get_renderer();
    });


    // auto main_container = ftxui::Container::Horizontal({
	// [&](){
	    // auto c = ftxui::Container::Horizontal({});
	    // for (auto &d : displays) {
		// c->Add(d->get_container());
	    // }
	    // return c;
	// }()
    // });
//
//
    // auto main_renderer = ftxui::Renderer(main_container, [&] {
	// Elements elements_vec;
	// for (auto &d : displays) {
	    // elements_vec.push_back(d->get_renderer());
	// }
	// return hbox({
	    // elements_vec
	// }) | center;
    // });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
	if (display.pv_update()) {
	    screen.PostEvent(Event::Custom);
	}
	// for (auto &d : displays) {
	    // if (d->pv_update()) {
		// screen.PostEvent(Event::Custom);
	    // }
	// }
        loop.RunOnce();
	std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }
}
