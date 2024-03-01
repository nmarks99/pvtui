#include <array>
#include <functional>
#include <sstream>
#include <string>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

// Display a component nicely with a title on the left.
Component Wrap(std::string name, Component component) {
    return Renderer(component, [name, component] {
        return hbox({
                   text(name) | size(WIDTH, EQUAL, 8),
                   separator(),
                   component->Render() | xflex,
               }) |
               xflex;
    });
}

int main() {

    std::array<int, 4> values = {0};
    auto screen = ScreenInteractive::FitComponent();

    // create buttons
    std::string button_label1 = "Increment1";
    std::string button_label2 = "Increment2";
    std::string button_label3 = "Increment3";
    std::string button_label4 = "Increment4";
    std::function<void()> on_click1 = [&] { values.at(0) += 1; };
    std::function<void()> on_click2 = [&] { values.at(1) += 1; };
    std::function<void()> on_click3 = [&] { values.at(2) += 1; };
    std::function<void()> on_click4 = [&] { values.at(3) += 1; };
    Component button1 = Button(&button_label1, on_click1, ButtonOption::Animated(Color::Red));
    Component button2 = Button(&button_label2, on_click2, ButtonOption::Animated(Color::Purple));
    Component button3 = Button(&button_label3, on_click3, ButtonOption::Animated(Color::Green));
    Component button4 = Button(&button_label4, on_click4, ButtonOption::Animated(Color::Yellow));

    // create input box
    // std::string input_label;
    // Component input = Input(&input_label, "");
    // input = Wrap("Input", input);

    // create the layout
    auto buttons = Container::Vertical({
        button1,
        button2,
        button3,
        button4,
    });

    auto component = Renderer(buttons, [&] {
        std::stringstream ss;
        for (const auto &v : values) {
            ss << std::to_string(v) << ", ";
        }
        return hbox({
                   vbox({
                       button1->Render(),
                       separator(),
                       button2->Render(),
                   }),
                   separator(),
                   vbox({
                       button3->Render(),
                       separator(),
                       button4->Render(),
                   }),
                   separator(),
                   text(ss.str()),
               }) |
               xflex | border;
    });

    // auto component = Renderer(layout, [&] {
    // return vbox({
    // input->Render(),
    // separator(),
    // button->Render(),
    // separator(),
    // text("value = " + std::to_string(value)),
    // }) |
    // xflex | size(WIDTH, GREATER_THAN, 40) | border;
    // });

    screen.Loop(component);

    return 0;
}
