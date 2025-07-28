#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/node.hpp"

using namespace ftxui;
using namespace pvtui;

class TestDisplay : public DisplayBase {
  public:
    TestDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args);
    ~TestDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    InputWidget desc;

    const pvtui::ArgParser &args;
};


TestDisplay::TestDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args),
    desc(pvgroup, args.replace("$(P)$(R).DESC"))
{}

ftxui::Component TestDisplay::get_container() {
    return Container::Vertical({
	desc.component()
    });
}

ftxui::Element TestDisplay::get_renderer() {
    return vbox({
	desc.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 30)
    });
}



int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (not args.macros_present({"P", "R"})) {
	printf("Missing required macro P, R\n");
	return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS PVA client
    // Start CAClientFactory so we can see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // shared_ptr to PVGroup to manage all PVs for displays
    std::shared_ptr<PVGroup> pvgroup = std::make_shared<PVGroup>(provider);

    TestDisplay display(pvgroup, args);

    auto main_container = display.get_container();
    auto main_renderer = ftxui::Renderer(main_container, [&] {
        return display.get_renderer();
    });

    constexpr int POLL_PERIOD_MS = 100;
    Loop loop(&screen, main_renderer);
    while (!loop.HasQuitted()) {
        if (display.pv_update()) {
            screen.PostEvent(Event::Custom);
        }
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_PERIOD_MS));
    }
}

