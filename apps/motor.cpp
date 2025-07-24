#include <charconv>
#include <stdexcept>
#include <vector>

#include "motor_display.hpp"
#include "pvtui.hpp"

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
    All,
    Multi,
};

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (not args.macros_present({"P"})) {
        printf("Missing required macro P\n");
        return EXIT_FAILURE;
    }

    // Flags to set display type
    MotorDisplayType display_type = MotorDisplayType::Small;
    if (args.flag("small")) {
        display_type = MotorDisplayType::Small;
    } else if (args.flag("medium")) {
        display_type = MotorDisplayType::Medium;
    } else if (args.flag("all")) {
        display_type = MotorDisplayType::All;
    } else if (args.flag("multi")) {
        display_type = MotorDisplayType::Multi;
    }

    // If M1 macro present, assume Mutli display type
    // otherwise assume single and M macro must be present
    if (args.macros_present({"M1"})) {
        display_type = MotorDisplayType::Multi;
    } else if (not args.macros_present({"M"})) {
        printf("Missing required macro M or (M1,M2,...)\n");
        return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS PVA client and CAClientFactory to see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // unique_ptr's to DisplayBase for each screen
    std::vector<std::unique_ptr<DisplayBase>> displays;

    // shared_ptr to PVGroup to manage all PVs for displays
    std::shared_ptr<PVGroup> pvgroup = std::make_shared<PVGroup>(provider);

    // multi display creates a SmallMotorDisplay for each Mn macro where n is an integer.
    // The resulting screen is similar to motorNx.adl
    std::vector<int> motor_num_vec;
    std::vector<ArgParser> args_vec;
    switch (display_type) {
    case MotorDisplayType::Multi:
        for (const auto &[k, v] : args.macros) {
            const size_t ind = k.find("M");
            if (ind != std::string::npos) {
                const std::string num_str = std::string(k.begin() + ind + 1, k.end());
                int num;
                if (std::from_chars(num_str.data(), num_str.data() + num_str.size(), num).ec !=
                    std::errc()) {
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
            displays.emplace_back(std::make_unique<SmallMotorDisplay>(pvgroup, args_n));
        }
        break;

    case MotorDisplayType::Small:
        displays.emplace_back(std::make_unique<SmallMotorDisplay>(pvgroup, args));
        break;

    case MotorDisplayType::Medium:
        displays.emplace_back(std::make_unique<MediumMotorDisplay>(pvgroup, args));
        break;

    case MotorDisplayType::All:
        displays.emplace_back(std::make_unique<AllMotorDisplay>(pvgroup, args));
        break;
    }

    auto main_container = ftxui::Container::Horizontal({});
    for (auto &d : displays) {
        main_container->Add(d->get_container());
    }

    auto main_renderer = ftxui::Renderer(main_container, [&] {
        Elements elements_vec;
        for (auto &d : displays) {
            elements_vec.push_back(d->get_renderer());
        }
        return hbox({elements_vec}) | center | pvtui::EPICSColor::BACKGROUND;
    });

    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        for (auto &d : displays) {
            if (d->pv_update()) {
                screen.PostEvent(Event::Custom);
            }
        }
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }
}
