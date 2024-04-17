#include <chrono>
#include <ftxui/screen/terminal.hpp>
#include <iostream>
#include <string>
#include <thread>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/node.hpp"


int main() {
    using namespace ftxui;
    using namespace std::chrono_literals;

    std::string reset_position;
    for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.005f) {
        std::string count = std::to_string(int(percentage * 5001)) + "/5000";
        auto g1 = hbox({
            text("one: "),
            gauge(percentage) | flex | color(Color::Blue),
            text(" " + count),
        });

        auto g2 = hbox({
            text("two: "),
            gauge(percentage) | flex | color(Color::Red),
            text(" " + count),
        });

        auto document = vbox({g1, separator(), g2});
        document = border(document);
        auto screen = Screen::Create(
            Dimension::Fixed(Terminal::Size().dimx / 2),
            Dimension::Fit(document)
        );
        Render(screen, document);
        std::cout << reset_position;
        screen.Print();
        reset_position = screen.ResetPosition();
        std::this_thread::sleep_for(0.01s);
    }
    std::cout << std::endl;
    return 0;
}
