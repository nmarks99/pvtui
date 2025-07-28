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
    InputWidget aout;
    InputWidget oeos;
    InputWidget ieos;
    InputWidget nowt;
    InputWidget tmot;
    InputWidget tfil;
    VarWidget<PVEnum> stat;
    VarWidget<PVEnum> sevr;
    VarWidget<std::string> tinp;
    VarWidget<std::string> nawt;
    VarWidget<std::string> nord;
    VarWidget<std::string> errs;
    DropdownWidget tmod;
    DropdownWidget cnct;
    DropdownWidget enbl;
    DropdownWidget auct;
    ChoiceHWidget tb0;
    ChoiceHWidget tb1;
    ChoiceHWidget tb2;
    ChoiceHWidget tb3;
    ChoiceHWidget tb4;
    ChoiceHWidget tb5;
    ChoiceHWidget tib0;
    ChoiceHWidget tib1;
    ChoiceHWidget tib2;
    ChoiceHWidget tinb0;
    ChoiceHWidget tinb1;
    ChoiceHWidget tinb2;
    ChoiceHWidget tinb3;

    const pvtui::ArgParser &args;
};


AsynDisplay::AsynDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args),
    aout(pvgroup, args, "$(P)$(R).AOUT", PVPutType::String),
    oeos(pvgroup, args, "$(P)$(R).OEOS", PVPutType::String),
    ieos(pvgroup, args, "$(P)$(R).IEOS", PVPutType::String),
    tinp(pvgroup, args, "$(P)$(R).TINP"),
    nawt(pvgroup, args, "$(P)$(R).NAWT"),
    nord(pvgroup, args, "$(P)$(R).NORD"),
    tmot(pvgroup, args, "$(P)$(R).TMOT", PVPutType::Double),
    tmod(pvgroup, args, "$(P)$(R).TMOD"),
    tb0(pvgroup, args, "$(P)$(R).TB0"),
    tb1(pvgroup, args, "$(P)$(R).TB1"),
    tb2(pvgroup, args, "$(P)$(R).TB2"),
    tb3(pvgroup, args, "$(P)$(R).TB3"),
    tb4(pvgroup, args, "$(P)$(R).TB4"),
    tb5(pvgroup, args, "$(P)$(R).TB5"),
    stat(pvgroup, args, "$(P)$(R).STAT"),
    sevr(pvgroup, args, "$(P)$(R).SEVR"),
    tib0(pvgroup, args, "$(P)$(R).TIB0"),
    tib1(pvgroup, args, "$(P)$(R).TIB1"),
    tib2(pvgroup, args, "$(P)$(R).TIB2"),
    tinb0(pvgroup, args, "$(P)$(R).TINB0"),
    tinb1(pvgroup, args, "$(P)$(R).TINB1"),
    tinb2(pvgroup, args, "$(P)$(R).TINB2"),
    tinb3(pvgroup, args, "$(P)$(R).TINB3"),
    cnct(pvgroup, args, "$(P)$(R).CNCT"),
    enbl(pvgroup, args, "$(P)$(R).ENBL"),
    auct(pvgroup, args, "$(P)$(R).AUCT"),
    tfil(pvgroup, args, "$(P)$(R).TFIL", PVPutType::String),
    errs(pvgroup, args, "$(P)$(R).ERRS"),
    nowt(pvgroup, args, "$(P)$(R).NOWT", PVPutType::Int)
{}

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
            text(nawt.value()) | color(Color::Black) | size(WIDTH, EQUAL, 3),
        }),
        separatorEmpty(),
        hbox({
            text(" In: ") | color(Color::Black),
            text(tinp.value()) | bgcolor(Color::RGB(220,220,220)) | xflex,
            separatorEmpty(),
            ieos.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 5),
            separatorEmpty(),
            text(nord.value()) | color(Color::Black) | size(WIDTH, EQUAL, 3),
        }),
        separator(),
        hbox({
            text("Err: ") | color(Color::Black),
            paragraph(errs.value()) | bgcolor(Color::RGB(220,220,220)) | xflex,
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
            text(stat.value().choice) | EPICSColor::READBACK,
            filler(),
            text("I/O Severity: ") | color(Color::Black),
            text(sevr.value().choice) | EPICSColor::READBACK
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
