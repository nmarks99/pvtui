#include <cassert>
#include <cstdlib>
#include <string>
#include <cmath>
#include <functional>

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

std::vector<double> squash_average(const std::vector<double>& input, std::size_t target_size) {
    std::vector<double> output;

    if (input.empty() || target_size == 0)
        return output;

    std::size_t input_size = input.size();
    output.resize(target_size, 0.0);

    for (std::size_t i = 0; i < target_size; ++i) {
        // Compute the range of input indices that map to this output bin
        std::size_t start_idx = std::floor(i * input_size / static_cast<double>(target_size));
        std::size_t end_idx   = std::floor((i + 1) * input_size / static_cast<double>(target_size));

        double sum = 0.0;
        std::size_t count = 0;
        for (std::size_t j = start_idx; j < end_idx && j < input_size; ++j) {
            sum += input[j];
            ++count;
        }

        output[i] = (count > 0) ? (sum / count) : 0.0;
    }

    return output;
}

std::string debug_string = "";

struct CurrentGraph {
    std::vector<int> operator()(int width, int height) const {
        std::vector<int> output(width);
        for (int i = 0; i < width; ++i) {
            float v = 0;
            v += 0.1f * sin((i + shift) * 0.1f);  
            v += 0.2f * sin((i + shift + 10) * 0.15f);
            v += 0.1f * sin((i + shift) * 0.03f);
            v *= height;
            v += 0.5f * height;
            output[i] = static_cast<int>(v);
        }
        // for (const auto &v : output) {
            // debug_string = debug_string + "," + std::to_string(v);
        // }
        return output;
    }
    int shift = 0;
};

struct RealDataGraph {
    const std::vector<double>& data;

    std::vector<int> operator()(int width, int height) const {
        std::vector<double> squashed = squash_average(data, width);

        constexpr double min_val = 0.0;
        constexpr double max_val = 200.0;
        constexpr double range = max_val - min_val;

        std::vector<int> output(width);
        for (int i = 0; i < width; ++i) {
            // Clamp values outside the expected range
            double clamped = std::clamp(squashed[i], min_val, max_val);

            // Normalize and flip Y-axis
            double normalized = (clamped - min_val) / range;
            output.at(i) = static_cast<int>((1.0 - normalized) * (height - 1));
        }

        return output;
    }
};

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
    {1, text("Shutters Enabled") | bgcolor(Color::Green) | color(Color::Black) | size(WIDTH, EQUAL, 16)},
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
    });

    std::string current = "";
    pvgroup.set_monitor("S-DCCT:CurrentM", current);

    int lifetime = 0;
    pvgroup.set_monitor("S-DCCT:LifetimeM", lifetime);

    PVEnum injection_status;
    pvgroup.set_monitor("S-INJ:InjectionOperationM", injection_status);

    std::string injection_period;
    pvgroup.set_monitor("S-INJ:InjectionPeriodCounterM", injection_period);

    PVEnum desired_mode;
    pvgroup.set_monitor("S:DesiredMode", desired_mode);
    
    PVEnum actual_mode;
    pvgroup.set_monitor("S:ActualMode", actual_mode);

    PVEnum shutter_status;
    pvgroup.set_monitor("RF-ACIS:FePermit:Sect1To35IdM", shutter_status);

    int num_shutters_open = 0;
    pvgroup.set_monitor("NoOfShuttersOpenA", num_shutters_open);

    std::vector<double> user_ops_current{1440, 0.0};
    pvgroup.set_monitor("S:UserOpsCurrent", user_ops_current);
    
    std::vector<double> other_current{1440, 0.0};
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

    // Just using container for "q" to quit. Maybe there's a better way?
    auto main_container = Container::Vertical({});
    main_container |= CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }
        return false;
    });


    // RealDataGraph gr{user_ops_current};
    CurrentGraph gr;

    // Main renderer to define visual layout of components and elements
    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            hbox({
                text("Current:  "),
                text(current) | size(WIDTH, EQUAL, 7),
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
            text("Machine Status: " + desired_mode.choice),
            text("Operating Mode: " + actual_mode.choice),
            shutter_status_text.at(shutter_status.index),
            text("Shutters Open: " + std::to_string(num_shutters_open)),
            separatorEmpty(),
            hbox({
                vbox({
                    text("200 _"),
                    separatorEmpty(),
                    separatorEmpty(),
                    separatorEmpty(),
                    separatorEmpty(),
                    text("100 _"),
                    separatorEmpty(),
                    separatorEmpty(),
                    separatorEmpty(),
                    separatorEmpty(),
                    text("  0 _"),
                }) | size(WIDTH, EQUAL, 5),
                graph(std::ref(gr)) | size(HEIGHT, EQUAL, 10) | color(Color::Blue) | border,
            }),
            separatorEmpty(),
            text("        Operators: " + operators),
            text("Floor Coordinator: " + floor_coord),
            text("     Fill Pattern: " + fill_patt),
            text(" Dump/Trip Reason: " + dump_reason),
            text("Trip Reason(cont): " + dump_reason_cont),
            text("     Problem Info:" + prob_info),
            text(" Prob Info (cont):" + prob_info_cont),
            text("        Next Fill:" + next_fill),
            text("  Next Fill(cont):" + next_fill_cont),
            text("      Next Update:" + next_update),
        }) | size(WIDTH, EQUAL, 70);
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
