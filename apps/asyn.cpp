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

static constexpr std::string_view CLI_HELP_MSG = R"(
PVTUI asyn - Terminal UI for EPICS asyn record.
Inspired by the asynOctet.adl and asynRecord.adl screens.

Usage:
  pvtui_asyn [options] 

Options:
  -h, --help        Show this help message and exit.
  -m, --macro       Macros to pass to the UI (required: P, R)        

Examples:
    pvtui_asyn --macro "P=xxx:,R=asyn1"

For more details, visit: https://github.com/nmarks99/pvtui
)";

class AsynDisplay : public DisplayBase {
  public:
    AsynDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args);
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
    PVWidget<PVEnum> tmod;
    PVWidget<std::string> errs;
    PVWidget<std::string> ofmt;
    PVWidget<PVEnum> cnct;
    PVWidget<PVEnum> enbl;
    PVWidget<PVEnum> auct;
    PVWidget<std::string> tfil;
    PVWidget<PVEnum> tb0;
    PVWidget<PVEnum> tb1;
    PVWidget<PVEnum> tb2;
    PVWidget<PVEnum> tb3;
    PVWidget<PVEnum> tb4;
    PVWidget<PVEnum> tb5;
    PVWidget<PVEnum> tib0;
    PVWidget<PVEnum> tib1;
    PVWidget<PVEnum> tib2;
    PVWidget<PVEnum> tinb0;
    PVWidget<PVEnum> tinb1;
    PVWidget<PVEnum> tinb2;
    PVWidget<PVEnum> tinb3;
    PVWidget<std::string> tmsk;
    PVWidget<std::string> tiom;

    // Store macro arguments
    const pvtui::ArgParser &args;
};


AsynDisplay::AsynDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {

    connect_pv(aout, args.replace("$(P)$(R).AOUT"), MonitorOn);
    aout.set_component(PVInput(pvgroup.get_pv(aout.pv_name), aout.value, PVPutType::String));

    connect_pv(oeos, args.replace("$(P)$(R).OEOS"), MonitorOn);
    oeos.set_component(PVInput(pvgroup.get_pv(oeos.pv_name), oeos.value, PVPutType::String));
    
    connect_pv(ieos, args.replace("$(P)$(R).IEOS"), MonitorOn);
    ieos.set_component(PVInput(pvgroup.get_pv(ieos.pv_name), ieos.value, PVPutType::String));

    connect_pv(tinp, args.replace("$(P)$(R).TINP"), MonitorOn);

    connect_pv(nawt, args.replace("$(P)$(R).NAWT"), MonitorOn);

    connect_pv(nord, args.replace("$(P)$(R).NORD"), MonitorOn);
   
    connect_pv(tmot, args.replace("$(P)$(R).TMOT"), MonitorOn);
    tmot.set_component(PVInput(pvgroup.get_pv(tmot.pv_name), tmot.value, PVPutType::String));

    connect_pv(tmod, args.replace("$(P)$(R).TMOD"), MonitorOn);
    tmod.set_component(PVDropdown(pvgroup.get_pv(tmod.pv_name), tmod.value.choices, tmod.value.index));

    connect_pv(tmsk, args.replace("$(P)$(R).TMSK"), MonitorOn);
    tmsk.set_component(PVInput(pvgroup.get_pv(tmsk.pv_name), tmsk.value, PVPutType::String));

    connect_pv(tb0, args.replace("$(P)$(R).TB0"), MonitorOn);
    tb0.set_component(PVChoiceH(pvgroup.get_pv(tb0.pv_name), tb0.value.choices, tb0.value.index));

    connect_pv(tb1, args.replace("$(P)$(R).TB1"), MonitorOn);
    tb1.set_component(PVChoiceH(pvgroup.get_pv(tb1.pv_name), tb1.value.choices, tb1.value.index));

    connect_pv(tb2, args.replace("$(P)$(R).TB2"), MonitorOn);
    tb2.set_component(PVChoiceH(pvgroup.get_pv(tb2.pv_name), tb2.value.choices, tb2.value.index));

    connect_pv(tb3, args.replace("$(P)$(R).TB3"), MonitorOn);
    tb3.set_component(PVChoiceH(pvgroup.get_pv(tb3.pv_name), tb3.value.choices, tb3.value.index));

    connect_pv(tb4, args.replace("$(P)$(R).TB4"), MonitorOn);
    tb4.set_component(PVChoiceH(pvgroup.get_pv(tb4.pv_name), tb4.value.choices, tb4.value.index));

    connect_pv(tb5, args.replace("$(P)$(R).TB5"), MonitorOn);
    tb5.set_component(PVChoiceH(pvgroup.get_pv(tb5.pv_name), tb5.value.choices, tb5.value.index));

    connect_pv(stat, args.replace("$(P)$(R).STAT"), MonitorOn);

    connect_pv(sevr, args.replace("$(P)$(R).SEVR"), MonitorOn);

    connect_pv(errs, args.replace("$(P)$(R).ERRS"), MonitorOn);

    connect_pv(tib0, args.replace("$(P)$(R).TIB0"), MonitorOn);
    tib0.set_component(PVChoiceH(pvgroup.get_pv(tib0.pv_name), tib0.value.choices, tib0.value.index));

    connect_pv(tib1, args.replace("$(P)$(R).TIB1"), MonitorOn);
    tib1.set_component(PVChoiceH(pvgroup.get_pv(tib1.pv_name), tib1.value.choices, tib1.value.index));

    connect_pv(tib2, args.replace("$(P)$(R).TIB2"), MonitorOn);
    tib2.set_component(PVChoiceH(pvgroup.get_pv(tib2.pv_name), tib2.value.choices, tib2.value.index));

    connect_pv(tinb0, args.replace("$(P)$(R).TINB0"), MonitorOn);
    tinb0.set_component(PVChoiceH(pvgroup.get_pv(tinb0.pv_name), tinb0.value.choices, tinb0.value.index));

    connect_pv(tinb1, args.replace("$(P)$(R).TINB1"), MonitorOn);
    tinb1.set_component(PVChoiceH(pvgroup.get_pv(tinb1.pv_name), tinb1.value.choices, tinb1.value.index));

    connect_pv(tinb2, args.replace("$(P)$(R).TINB2"), MonitorOn);
    tinb2.set_component(PVChoiceH(pvgroup.get_pv(tinb2.pv_name), tinb2.value.choices, tinb2.value.index));

    connect_pv(tinb3, args.replace("$(P)$(R).TINB3"), MonitorOn);
    tinb3.set_component(PVChoiceH(pvgroup.get_pv(tinb3.pv_name), tinb3.value.choices, tinb3.value.index));

    connect_pv(cnct, args.replace("$(P)$(R).CNCT"), MonitorOn);
    cnct.set_component(PVDropdown(pvgroup.get_pv(cnct.pv_name), cnct.value.choices, cnct.value.index));

    connect_pv(enbl, args.replace("$(P)$(R).ENBL"), MonitorOn);
    enbl.set_component(PVDropdown(pvgroup.get_pv(enbl.pv_name), enbl.value.choices, enbl.value.index));

    connect_pv(auct, args.replace("$(P)$(R).AUCT"), MonitorOn);
    auct.set_component(PVDropdown(pvgroup.get_pv(auct.pv_name), auct.value.choices, auct.value.index));

    connect_pv(tfil, args.replace("$(P)$(R).TFIL"), MonitorOn);
    tfil.set_component(PVInput(pvgroup.get_pv(tfil.pv_name), tfil.value, PVPutType::String));
}

ftxui::Component AsynDisplay::get_container() {

    return ftxui::Container::Vertical({
        ftxui::Container::Horizontal({
            tmot.component(),
            tmod.component(),
        }),
        
        ftxui::Container::Horizontal({
            aout.component(),
            oeos.component(),
        }),
        ieos.component(),

        ftxui::Container::Horizontal({
            cnct.component(),
            enbl.component(),
            auct.component()
        }),

        ftxui::Container::Horizontal({
            ftxui::Container::Vertical({
                tb0.component(),
                tb1.component(),
                tb2.component(),
                tb3.component(),
                tb4.component(),
                tb5.component(),
            }),
            ftxui::Container::Vertical({
                tib0.component(),
                tib1.component(),
                tib2.component(),
                tinb0.component(),
                tinb1.component(),
                tinb2.component(),
                tinb3.component(),
            })
        }),
        tfil.component(),
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
            filler(),
            text("Transfer: ") | color(Color::Black),
            tmod.component()->Render() | EPICSColor::EDIT | size(WIDTH, LESS_THAN, 12),
        }),
        separator(),
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
        separator(),
        hbox({
            text("Err: ") | color(Color::Black),
            paragraph(errs.value) | bgcolor(Color::RGB(220,220,220)) | xflex,
        }),
        separatorEmpty(),
        hbox({
            cnct.component()->Render() | EPICSColor::EDIT,
            filler(),
            enbl.component()->Render() | EPICSColor::EDIT,
            filler(),
            auct.component()->Render() | EPICSColor::EDIT
        }),
        separatorEmpty(),
        hbox({
            text("I/O Status: ") | color(Color::Black),
            text(stat.value.choice) | EPICSColor::READBACK,
            filler(),
            text("I/O Severity: ") | color(Color::Black),
            text(sevr.value.choice) | EPICSColor::READBACK
        }),

        // separator() | color(Color::Black),
        separator(),

        // trace mask toggles
        hbox({
            vbox({
                text("traceMask")
                    | bold
                    | underlined
                    | size(WIDTH, EQUAL, 7)
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
            }),
            filler(),
            vbox({
                text("traceIOMask")
                    | bold
                    | underlined
                    | size(WIDTH, EQUAL, 9)
                    | color(Color::Black),
                hbox({
                    tib0.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceIOASCII") | color(Color::Black)
                }),
                hbox({
                    tib1.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceIOEscape") | color(Color::Black)
                }),
                hbox({
                    tib2.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceIOHex") | color(Color::Black)
                }),

                separatorEmpty(),

                text("traceInfoMask")
                    | bold
                    | underlined
                    | size(WIDTH, EQUAL, 11)
                    | color(Color::Black),
                hbox({
                    tinb0.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceInfoTime") | color(Color::Black)
                }),
                hbox({
                    tinb1.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceInfoPort") | color(Color::Black)
                }),
                hbox({
                    tinb2.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceInfoSource") | color(Color::Black)
                }),
                hbox({
                    tinb3.component()->Render()
                        | EPICSColor::EDIT
                        | size(WIDTH, EQUAL, 7),
                    separatorEmpty(),
                    text("traceInfoThread") | color(Color::Black)
                }),
            }),
        }),
        separatorEmpty(),
        hbox({
            text("Trace file: ") | color(Color::Black),
            tfil.component()->Render() | EPICSColor::EDIT,
        }),
        separatorEmpty(),
    }) | border | color(Color::Black) | size(WIDTH, EQUAL, 52) | center | EPICSColor::BACKGROUND;
}



int main(int argc, char *argv[]) {

    // Parse command line arguments and macros
    pvtui::ArgParser args(argc, argv);

    if (args.flag("help") or args.flag("h")) {
	std::cout << CLI_HELP_MSG << std::endl;
	return EXIT_SUCCESS;
    }

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
    // std::shared_ptr<PVGroup> pvgroup = std::make_shared<PVGroup>(provider);
    PVGroup pvgroup(provider);

    AsynDisplay display(pvgroup, args);

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
