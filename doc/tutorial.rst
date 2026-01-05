.. _tutorial:

Tutorial: Creating an App
=========================

This page will provide a brief tutorial on creating a new PVTUI application.

Note that PVTUI is primarily a *wrapper* library around FTXUI to make your TUI application EPICS aware.
This tutorial will not give an in-depth explanation on how to use FTXUI. For that I recommend
consulting the example applications provided in the FTXUI repository and the FTXUI API
documentation.

A typical structure for a simple PVTUI app is shown below.

.. code-block:: cpp

    #include <pvtui/pvtui.hpp>
    #include <ftxui/component/component.hpp>

    using namespace ftxui;
    using namespace pvtui;

    int main(int argc, char *argv[]) {

        // Instantiate an App to manage everything
        App app(argc, argv);

        // Check for required macros if desired
        if (not app.args.macros_present({"P"})) {
            std::cerr << "Missing required macros\nRequired macros: P\n";
            return EXIT_FAILURE;
        }

        // Create all the widgets you need
        InputWidget desc(app, "$(P)pvname.DESC", PVPutType::String);

        // ftxui container to define interactivity of components
        auto main_container = Container::Vertical({
            // include all the interactive widgets here
            desc.component(),
        });

        // ftxui renderer defines the visual layout
        auto main_renderer = Renderer(main_container, [&] {
            // define the visual layout of the application
            return hbox({ // e.g.
                desc.component()->Render()
            })
        });

        // Run the main loop
        app.run(main_renderer);
    }

After some minimal boilerplate and creating all the TUI widgets you'll need, most of the development process is spent defining the
ftxui::Renderer, which determines the visual layout of the widgets.
