#include <numeric>

#include <pv/caProvider.h>
#include <pva/client.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/event.hpp>

#include <pvtui/pvtui.hpp>

using namespace ftxui;
using namespace pvtui;

static constexpr std::string_view CLI_HELP_MSG = R"(
pvtui_sr - APS storage ring status display
Inspired by xfd-display.adl
Note: pvgateway must be set correctly. See /APSshare/bin/xfd-display

Usage:
  pvtui_sr [options]

Options:
  -h, --help        Show this help message and exit.

For more details, visit: https://github.com/nmarks99/pvtui
)";

std::vector<double> downsample_and_clip(const std::vector<double>& input, int target_size, double curr_min, double curr_max, double height) {
    std::vector<double> result;
    double chunk_size = static_cast<double>(input.size()) / target_size;

    for (int i = 0; i < target_size; ++i) {
        int start = static_cast<int>(i * chunk_size);
        int end = static_cast<int>((i + 1) * chunk_size);
        end = std::min(end, static_cast<int>(input.size()));
        if (start >= end) break;

        double sum = std::accumulate(input.begin() + start, input.begin() + end, 0.0);
        double avg = sum / (end - start);

        double scaled = ((avg - curr_min) / (curr_max - curr_min)) * height;
        scaled = std::clamp(scaled, 0.0, height);
        result.push_back(scaled);
    }

    return result;
}

std::string debug_string = "";

static const std::unordered_map<int, Element> inj_status_text = {
    {0, text("Waiting for Injection") | color(Color::Red)},
    {1, text("")},
    {2, text("Swapout in Progress") | color(Color::Green)},
    {3, text("Waiting for Injection") | color(Color::Red)},
    {4, text("Stored Beam") | color(Color::Blue)},
    {5, text("")},
};

static const std::unordered_map<int, Element> shutter_status_text = {
    {0, text("Shutters Disabled") | bgcolor(Color::Red) | size(WIDTH, EQUAL, 17)},
    {1, text("Shutters Enabled") | bgcolor(Color::Green) | color(Color::Black) | size(WIDTH, EQUAL, 15)},
};


int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (args.help(CLI_HELP_MSG)) return EXIT_SUCCESS;

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // PVGroup to manage all PVs for this display
    PVGroup pvgroup(provider);

    // Widgets for each PV we want on the display
    // This particular display has no interactive elements
    VarWidget<std::string> time_and_date(pvgroup, "S:IOC:timeOfDayForm1SI");
    VarWidget<std::string> current(pvgroup, "S-DCCT:CurrentM");
    VarWidget<int> lifetime(pvgroup, "S-DCCT:LifetimeM");
    VarWidget<PVEnum> injection_status(pvgroup, "S-INJ:InjectionOperationM");
    VarWidget<std::string> injection_period(pvgroup, "S-INJ:InjectionPeriodCounterM");
    VarWidget<PVEnum> desired_mode(pvgroup, "S:DesiredMode");
    VarWidget<PVEnum> actual_mode(pvgroup, "S:ActualMode");
    VarWidget<PVEnum> shutter_status(pvgroup, "RF-ACIS:FePermit:Sect1To35IdM");
    VarWidget<int> num_shutters_open(pvgroup, "NoOfShuttersOpenA");
    VarWidget<std::string> operators(pvgroup, "OPS:message1");
    VarWidget<std::string> floor_coord(pvgroup, "OPS:message2");
    VarWidget<std::string> fill_patt(pvgroup, "OPS:message3");
    VarWidget<std::string> dump_reason(pvgroup, "OPS:message5");
    VarWidget<std::string> dump_reason_cont(pvgroup, "OPS:message16");
    VarWidget<std::string> prob_info(pvgroup, "OPS:message4");
    VarWidget<std::string> prob_info_cont(pvgroup, "OPS:message14");
    VarWidget<std::string> next_fill(pvgroup, "OPS:message6");
    VarWidget<std::string> next_fill_cont(pvgroup, "OPS:message17");
    VarWidget<std::string> next_update(pvgroup, "OPS:message18");

    // VarWidget does not work right here since there isn't a way to preallocate 1440
    std::vector<double> user_ops_current(1440, 0.0);
    pvgroup.add("S:UserOpsCurrent");
    pvgroup.set_monitor("S:UserOpsCurrent", user_ops_current);

    std::vector<double> other_current(1440, 0.0);
    pvgroup.add("S:OtherCurrent");
    pvgroup.set_monitor("S:OtherCurrent", other_current);

    auto plot1_renderer = Renderer([&] {
        const double CURR_MAX = 200;
        const double CURR_MIN = 0;
        const double TARGET_HEIGHT = 50;
        const int TARGET_WIDTH = 100;
        auto c = Canvas(TARGET_WIDTH, TARGET_HEIGHT);
        std::vector<double> comp_user = downsample_and_clip(user_ops_current, TARGET_WIDTH, CURR_MIN, CURR_MAX, TARGET_HEIGHT);
        std::vector<double> comp_other = downsample_and_clip(other_current, TARGET_WIDTH, CURR_MIN, CURR_MAX, TARGET_HEIGHT);

        // "user" current
        std::vector<int> y1(comp_user.size());
        for (size_t x = 0; x < comp_user.size(); x++) {
            y1[x] = static_cast<int>(TARGET_HEIGHT-(comp_user.at(x)));
        }
        for (size_t x = 1; x < comp_user.size()-1; x++) {
            c.DrawPointLine(x, y1[x], x + 1, y1[x + 1], Color::Blue);
        }

        // "other" current
        std::vector<int> y2(comp_other.size());
        for (size_t x = 0; x < comp_other.size(); x++) {
            y2[x] = static_cast<int>(TARGET_HEIGHT-(comp_other.at(x)));
        }
        for (size_t x = 1; x < comp_other.size()-1; x++) {
            c.DrawPointLine(x, y2[x], x + 1, y2[x + 1], Color::Red);
        }

        return canvas(std::move(c));
    });

    // Container for "q" to quit
    auto main_container = Container::Vertical({});
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
            text("Storage Ring Status") | borderLight | bold | size(WIDTH, EQUAL, 19),
            text(time_and_date.value()),
            separatorEmpty(),

            hbox({
                text("Current:  "),
                text(current.value()) | size(WIDTH, EQUAL, 7),
                text(" mA")
            }),
            hbox({
                text("Lifetime: "),
                text(std::to_string(lifetime.value())) | size(WIDTH, EQUAL, 7),
                text(" min")
            }),

            separatorEmpty(),
            inj_status_text.at(injection_status.value().index),
            text("Swapout In: " + injection_period.value() + " sec."),
            separatorEmpty(),
            shutter_status_text.at(shutter_status.value().index),
            text("Machine Status: " + desired_mode.value().choice),
            text("Operating Mode: " + actual_mode.value().choice),
            text("Shutters Open: " + std::to_string(num_shutters_open.value())),

            separatorEmpty(),
            text("Beam History: ") | bold | italic | underlined | size(WIDTH, EQUAL, 11),
            vbox({
                separatorEmpty(),
                hbox({
                    vbox({
                        text("200 -"),
                        filler(),
                        text("150 -"),
                        filler(),
                        text("100 -"),
                        filler(),
                        text("50  -"),
                        filler(),
                        text("0   -"),
                    }),
                    separator(),
                    separatorEmpty(),
                    plot1_renderer->Render(),
                    separatorEmpty(),
                }),
                separator(),
                hbox({
                    text("    24hr"), filler(), text("12hr"), filler(), text("0hr")
                }),
            }) | size(WIDTH, EQUAL, 57),
            separatorEmpty(),

            text("Messages from Operations: ") | bold | italic | underlined | size(WIDTH, EQUAL, 22),
            text("        Operators: " + operators.value()),
            text("Floor Coordinator: " + floor_coord.value()),
            text("     Fill Pattern: " + fill_patt.value()),
            text(" Dump/Trip Reason: " + dump_reason.value()),
            text("Trip Reason(cont): " + dump_reason_cont.value()),
            text("     Problem Info: " + prob_info.value()),
            text(" Prob Info (cont): " + prob_info_cont.value()),
            text("        Next Fill: " + next_fill.value()),
            text("  Next Fill(cont): " + next_fill_cont.value()),
            text("      Next Update: " + next_update.value()),
        });
    });

    // Custom main loop
    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        if (pvgroup.sync()) {
            screen.PostEvent(Event::Custom);
        }
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return 0;
}
