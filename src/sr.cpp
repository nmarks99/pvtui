#include <cassert>
#include <cstdlib>
#include <string>
#include <cmath>
#include <functional>
#include <numeric>

#include <pv/caProvider.h>
#include <pva/client.h>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

#include "pvgroup.hpp"

using namespace ftxui;

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

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    PVGroup pvgroup(provider, {
        "S-DCCT:CurrentM",
        "S-DCCT:LifetimeM",
        "S-INJ:InjectionOperationM",
        "S-INJ:InjectionPeriodCounterM",
        "S:DesiredMode",
        "S:ActualMode",
        "NoOfShuttersOpenA",
        "S:UserOpsCurrent",
        "S:OtherCurrent",
        "RF-ACIS:FePermit:Sect1To35IdM",
        "OPS:message1",
        "OPS:message2",
        "OPS:message3",
        "OPS:message4",
        "OPS:message5",
        "OPS:message6",
        "OPS:message14",
        "OPS:message16",
        "OPS:message17",
        "OPS:message18",
        "S:IOC:timeOfDayForm1SI"
    });

    std::string time_and_date = "";
    pvgroup.set_monitor("S:IOC:timeOfDayForm1SI", time_and_date);

    PVAny current;
    pvgroup.set_monitor("S-DCCT:CurrentM", current);
    auto &current_PV = pvgroup.get_pv("S-DCCT:CurrentM");

    int lifetime = 0;
    pvgroup.set_monitor("S-DCCT:LifetimeM", lifetime);

    PVEnum injection_status;
    pvgroup.set_monitor("S-INJ:InjectionOperationM", injection_status);

    PVAny injection_period; injection_period.prec=1;
    pvgroup.set_monitor("S-INJ:InjectionPeriodCounterM", injection_period);

    PVEnum desired_mode;
    pvgroup.set_monitor("S:DesiredMode", desired_mode);
    
    PVEnum actual_mode;
    pvgroup.set_monitor("S:ActualMode", actual_mode);

    PVEnum shutter_status;
    pvgroup.set_monitor("RF-ACIS:FePermit:Sect1To35IdM", shutter_status);

    int num_shutters_open = 0;
    pvgroup.set_monitor("NoOfShuttersOpenA", num_shutters_open);

    std::vector<double> user_ops_current(1440, 0.0);
    pvgroup.set_monitor("S:UserOpsCurrent", user_ops_current);
    
    std::vector<double> other_current(1440, 0.0);
    pvgroup.set_monitor("S:OtherCurrent", other_current);

    std::string operators = "";
    pvgroup.set_monitor("OPS:message1", operators);
    
    std::string floor_coord = "";
    pvgroup.set_monitor("OPS:message2", floor_coord);
    
    std::string fill_patt = "";
    pvgroup.set_monitor("OPS:message3", fill_patt);

    std::string dump_reason = "";
    pvgroup.set_monitor("OPS:message5", dump_reason);

    std::string dump_reason_cont = "";
    pvgroup.set_monitor("OPS:message16", dump_reason_cont);
    
    std::string prob_info = "";
    pvgroup.set_monitor("OPS:message4", prob_info);

    std::string prob_info_cont = "";
    pvgroup.set_monitor("OPS:message14", prob_info_cont);

    std::string next_fill = "";
    pvgroup.set_monitor("OPS:message6", next_fill);

    std::string next_fill_cont = "";
    pvgroup.set_monitor("OPS:message17", next_fill_cont);

    std::string next_update = "";
    pvgroup.set_monitor("OPS:message18", next_update);

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
        for (int x = 0; x < comp_user.size(); x++) {
            float dx = float(x);
            y1[x] = static_cast<int>(TARGET_HEIGHT-(comp_user.at(x)));
        }
        for (int x = 1; x < comp_user.size()-1; x++) {
            c.DrawPointLine(x, y1[x], x + 1, y1[x + 1], Color::Blue);
        }

        // "other" current
        std::vector<int> y2(comp_other.size());
        for (int x = 0; x < comp_other.size(); x++) {
            float dx = float(x);
            y2[x] = static_cast<int>(TARGET_HEIGHT-(comp_other.at(x)));
        }
        for (int x = 1; x < comp_other.size()-1; x++) {
            c.DrawPointLine(x, y2[x], x + 1, y2[x + 1], Color::Red);
        }

        return canvas(std::move(c));
    });

    // Container for "q" to quit. Maybe there's a better way?
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
            text(time_and_date),
            separatorEmpty(),

            hbox({
                text("Current:  "),
                text(current.value) | size(WIDTH, EQUAL, 7),
                text(" mA")
            }),
            hbox({
                text("Lifetime: "),
                text(std::to_string(lifetime)) | size(WIDTH, EQUAL, 7),
                text(" min")
            }),

            separatorEmpty(),
            inj_status_text.at(injection_status.index),
            text("Swapout In: " + injection_period + " sec."),
            separatorEmpty(),
            shutter_status_text.at(shutter_status.index),
            text("Machine Status: " + desired_mode.choice),
            text("Operating Mode: " + actual_mode.choice),
            text("Shutters Open: " + std::to_string(num_shutters_open)),

            separatorEmpty(),
            text("Beam History: ") | bold | italic | underlined | size(WIDTH, EQUAL, 11),
            vbox({
                // separator(),
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
                    // separator(),
                }),
                separator(),
                hbox({
                    text("    24hr"), filler(), text("12hr"), filler(), text("0hr")
                }),
            }) | size(WIDTH, EQUAL, 57),
            separatorEmpty(),

            text("Messages from Operators: ") | bold | italic | underlined | size(WIDTH, EQUAL, 22),
            text("        Operators: " + operators),
            text("Floor Coordinator: " + floor_coord),
            text("     Fill Pattern: " + fill_patt),
            text(" Dump/Trip Reason: " + dump_reason),
            text("Trip Reason(cont): " + dump_reason_cont),
            text("     Problem Info: " + prob_info),
            text(" Prob Info (cont): " + prob_info_cont),
            text("        Next Fill: " + next_fill),
            text("  Next Fill(cont): " + next_fill_cont),
            text("      Next Update: " + next_update),
        });
    });
        
    // Custom main loop
    constexpr int POLL_PERIOD_MS = 200;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        pvgroup.update();
        screen.PostEvent(Event::Custom);
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }

    return 0;
}
