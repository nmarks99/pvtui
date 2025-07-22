#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/node.hpp>

using namespace ftxui;
using namespace pvtui;

class AsynDisplay : public DisplayBase {
  public:
    AsynDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args);
    ~AsynDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    // Create a PVWidget for each PV aware FTXUI component
    PVWidget<std::string> aout;
    PVWidget<std::string> oeos;
    PVWidget<std::string> ieos;
    PVWidget<std::string> tinp;
    PVWidget<PVEnum> stat;
    PVWidget<PVEnum> sevr;
    PVWidget<std::string> nowt;
    PVWidget<std::string> nawt;
    PVWidget<std::string> nord;
    PVWidget<std::string> eomr;
    PVWidget<std::string> tmot;
    PVWidget<std::string> errs;
    PVWidget<std::string> ofmt;

    PVWidget<PVEnum> tb0;
    PVWidget<PVEnum> tb1;
    PVWidget<PVEnum> tb2;
    PVWidget<PVEnum> tb3;
    PVWidget<PVEnum> tb4;
    PVWidget<PVEnum> tb5;
    PVWidget<PVEnum> tib0;
    PVWidget<PVEnum> tib1;
    PVWidget<PVEnum> tib2;
    PVWidget<std::string> tmsk;
    PVWidget<std::string> tiom;

    // Store macro arguments
    const pvtui::ArgParser &args;
};

int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (not args.macros_present({"P", "R"})) {
        printf("Missing required macro P, R\n");
        return EXIT_FAILURE;
    }

    // Create the FTXUI screen. Interactive and uses the full terminal screen
    auto screen = ScreenInteractive::Fullscreen();

    // Instantiate EPICS PVA client and CAClientFactory to see CA only PVs
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider(args.provider);

    // shared_ptr to PVGroup to manage all PVs for displays
    std::shared_ptr<PVGroup> pvgroup = std::make_shared<PVGroup>(provider);

    AsynDisplay display(pvgroup, args);

    auto main_container = display.get_container();
    auto main_renderer = ftxui::Renderer(main_container, [&] {
        return display.get_renderer();
        // return hbox(display.get_renderer()) | center | pvtui::EPICSColor::BACKGROUND;
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

AsynDisplay::AsynDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {

    aout.pv_name = pvgroup->add("$(P)$(R).AOUT", args.macros);
    aout.set_component(PVInput(pvgroup->get_pv(aout.pv_name), aout.value, PVPutType::String));
    pvgroup->set_monitor(aout.pv_name, aout.value);

    oeos.pv_name = pvgroup->add("$(P)$(R).OEOS", args.macros);
    oeos.set_component(PVInput(pvgroup->get_pv(oeos.pv_name), oeos.value, PVPutType::String));
    pvgroup->set_monitor(oeos.pv_name, oeos.value);
    
    ieos.pv_name = pvgroup->add("$(P)$(R).IEOS", args.macros);
    ieos.set_component(PVInput(pvgroup->get_pv(ieos.pv_name), ieos.value, PVPutType::String));
    pvgroup->set_monitor(ieos.pv_name, ieos.value);

    tinp.pv_name = pvgroup->add("$(P)$(R).TINP", args.macros);
    pvgroup->set_monitor(tinp.pv_name, tinp.value);

    nawt.pv_name = pvgroup->add("$(P)$(R).NAWT", args.macros);
    pvgroup->set_monitor(nawt.pv_name, nawt.value);
    
    nord.pv_name = pvgroup->add("$(P)$(R).NORD", args.macros);
    pvgroup->set_monitor(nord.pv_name, nord.value);
    
    tmot.pv_name = pvgroup->add("$(P)$(R).TMOT", args.macros);
    tmot.set_component(PVInput(pvgroup->get_pv(tmot.pv_name), tmot.value, PVPutType::String));
    pvgroup->set_monitor(tmot.pv_name, tmot.value);

    tmsk.pv_name = pvgroup->add("$(P)$(R).TMSK", args.macros);
    tmsk.set_component(PVInput(pvgroup->get_pv(tmsk.pv_name), tmsk.value, PVPutType::String));
    pvgroup->set_monitor(tmsk.pv_name, tmsk.value);

    tb0.pv_name = pvgroup->add("$(P)$(R).TB0", args.macros);
    tb0.set_component(PVChoiceH(pvgroup->get_pv(tb0.pv_name), tb0.value.choices, tb0.value.index));
    pvgroup->set_monitor(tb0.pv_name, tb0.value);

    tb1.pv_name = pvgroup->add("$(P)$(R).TB1", args.macros);
    tb1.set_component(PVChoiceH(pvgroup->get_pv(tb1.pv_name), tb1.value.choices, tb1.value.index));
    pvgroup->set_monitor(tb1.pv_name, tb1.value);
    
    tb2.pv_name = pvgroup->add("$(P)$(R).TB2", args.macros);
    tb2.set_component(PVChoiceH(pvgroup->get_pv(tb2.pv_name), tb2.value.choices, tb2.value.index));
    pvgroup->set_monitor(tb2.pv_name, tb2.value);
    
    tb3.pv_name = pvgroup->add("$(P)$(R).TB3", args.macros);
    tb3.set_component(PVChoiceH(pvgroup->get_pv(tb3.pv_name), tb3.value.choices, tb3.value.index));
    pvgroup->set_monitor(tb3.pv_name, tb3.value);
    
    tb4.pv_name = pvgroup->add("$(P)$(R).TB4", args.macros);
    tb4.set_component(PVChoiceH(pvgroup->get_pv(tb4.pv_name), tb4.value.choices, tb4.value.index));
    pvgroup->set_monitor(tb4.pv_name, tb4.value);
    
    tb5.pv_name = pvgroup->add("$(P)$(R).TB5", args.macros);
    tb5.set_component(PVChoiceH(pvgroup->get_pv(tb5.pv_name), tb5.value.choices, tb5.value.index));
    pvgroup->set_monitor(tb5.pv_name, tb5.value);

    stat.pv_name = pvgroup->add("$(P)$(R).STAT", args.macros);
    pvgroup->set_monitor(stat.pv_name, stat.value);

    sevr.pv_name = pvgroup->add("$(P)$(R).SEVR", args.macros);
    pvgroup->set_monitor(sevr.pv_name, sevr.value);

    errs.pv_name = pvgroup->add("$(P)$(R).ERRS", args.macros);
    pvgroup->set_monitor(errs.pv_name, errs.value);
}

ftxui::Component AsynDisplay::get_container() {

    return ftxui::Container::Vertical({
        tmot.component(),
        
        ftxui::Container::Horizontal({
            aout.component(),
            oeos.component(),
        }),
        ieos.component(),

        tmsk.component(),
        tb0.component(),
        tb1.component(),
        tb2.component(),
        tb3.component(),
        tb4.component(),
        tb5.component(),
    });

}

ftxui::Element AsynDisplay::get_renderer() {

    return vbox({
        text(args.macros.at("P") + args.macros.at("R"))
            | bold | italic
            | bgcolor(Color::NavyBlue) | color(Color::White),
        separatorEmpty(),

        hbox({
            text("Timeout(sec): ") | color(Color::Black),
            tmot.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 6),
        }),
        separator() | color(Color::Black),
        hbox({
            text("Out: ") | color(Color::Black),
            aout.component()->Render() | EPICSColor::EDIT | xflex,
            separatorEmpty(),
            oeos.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 5),
            separatorEmpty(),
            text(nawt.value) | color(Color::Black) | size(WIDTH, EQUAL, 3),
        }),
        separatorEmpty(),
        hbox({
            text(" In: ") | color(Color::Black),
            text(tinp.value) | bgcolor(Color::RGB(220,220,220)) | xflex,
            separatorEmpty(),
            ieos.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 5),
            separatorEmpty(),
            text(nord.value) | color(Color::Black) | size(WIDTH, EQUAL, 3),
        }),
        separatorEmpty(),
        hbox({
            text("Err: ") | color(Color::Black),
            text(errs.value) | bgcolor(Color::RGB(220,220,220)) | xflex,
        }),
        hbox({
            text("I/O Status: ") | color(Color::Black),
            text(stat.value.choice) | EPICSColor::READBACK,
            filler(),
            text("I/O Severity: ") | color(Color::Black),
            text(sevr.value.choice) | EPICSColor::READBACK
        }),

        separator() | color(Color::Black),
        text("Trace Mask")
            | bold
            | underlined
            | size(WIDTH, EQUAL, 8)
            | color(Color::Black),

        hbox({
            tb0.component()->Render()
                | EPICSColor::EDIT
                | size(WIDTH, EQUAL, 7),
            separatorEmpty(),
            text("traceError") | color(Color::Black)
        }),
        hbox({
            tb1.component()->Render()
                | EPICSColor::EDIT
                | size(WIDTH, EQUAL, 7),
            separatorEmpty(),
            text("traceIODevice") | color(Color::Black)
        }),
        hbox({
            tb2.component()->Render()
                | EPICSColor::EDIT
                | size(WIDTH, EQUAL, 7),
            separatorEmpty(),
            text("traceIOFilter") | color(Color::Black)
        }),
        hbox({
            tb3.component()->Render()
                | EPICSColor::EDIT
                | size(WIDTH, EQUAL, 7),
            separatorEmpty(),
            text("traceIODriver") | color(Color::Black)
        }),
        hbox({
            tb4.component()->Render()
                | EPICSColor::EDIT
                | size(WIDTH, EQUAL, 7),
            separatorEmpty(),
            text("traceFlow") | color(Color::Black)
        }),
        hbox({
            tb5.component()->Render()
                | EPICSColor::EDIT
                | size(WIDTH, EQUAL, 7),
            separatorEmpty(),
            text("traceWarning") | color(Color::Black)
        }),
        separatorEmpty(),
        
    }) | border | color(Color::Black) | size(WIDTH, EQUAL, 48) | center | EPICSColor::BACKGROUND;
}
