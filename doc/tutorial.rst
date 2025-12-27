.. _tutorial:

Creating an App
===============

This page will provide a brief tutorial on creating a new PVTUI application.

PVTUI is primarily a *wrapper* library around FTXUI to make your TUI application EPICS aware.
This tutorial will not give an in-depth explanation on how to use FTXUI. For that I recommend
consulting the example applications provided in the FTXUI repository and the FTXUI API
documentation.


.. code-block:: cpp

    #include <pvtui/pvtui.hpp>
    #include <ftxui/component/component.hpp>

    using namespace ftxui;
    using namespace pvtui;

    int main(int argc, char *argv[]) {

        App app(argc, argv);

        if (not app.args.macros_present({"P"})) {
            std::cerr << "Missing required macros\nRequired macros: P\n";
            return EXIT_FAILURE;
        }

        // Create all the widgets
        InputWidget desc(app, "$(P)float1.DESC", PVPutType::String);

        // ftxui container to define interactivity of components
        auto main_container = Container::Vertical({
            desc.component()
        });

        // ftxui renderer defines the visual layout
        auto main_renderer = Renderer(main_container, [&] {
            return hbox({
                text(app.args.macros.at("P") + "float1.DESC: ")
                    | color(Color::Black),
                    separatorEmpty(),
                desc.component()->Render()
                    | EPICSColor::edit(desc)
                    | size(WIDTH, EQUAL, 15)
            }) | center | EPICSColor::background();
        });

        // Main loop
        app.run(main_renderer);
    }
