#include "motor_display.hpp"
#include "ftxui/component/component.hpp"
#include "pvtui.hpp"
#include <ftxui/dom/elements.hpp>

void SmallMotorDisplay::init() {
    using namespace pvtui;

    connect_pv(desc, args.replace("$(P)$(M).DESC"), MonitorOn);
    desc.set_component(PVInput(pvgroup->get_pv(desc.pv_name), desc.value, PVPutType::String));
    
    connect_pv(val, args.replace("$(P)$(M).VAL"), MonitorOn);
    val.set_component(PVInput(pvgroup->get_pv(val.pv_name), val.value, PVPutType::Double));

    connect_pv(twr, args.replace("$(P)$(M).TWR"), MonitorOff);
    twr.set_component(PVButton(pvgroup->get_pv(twr.pv_name), " < ", 1));

    connect_pv(twv, args.replace("$(P)$(M).TWV"), MonitorOn);
    twv.set_component(PVInput(pvgroup->get_pv(twv.pv_name), twv.value, PVPutType::Double));

    connect_pv(twf, args.replace("$(P)$(M).TWF"), MonitorOff);
    twf.set_component(PVButton(pvgroup->get_pv(twf.pv_name), " > ", 1));

    connect_pv(rbv, args.replace("$(P)$(M).RBV"), MonitorOn);

    connect_pv(egu, args.replace("$(P)$(M).EGU"), MonitorOn);

    connect_pv(hls, args.replace("$(P)$(M).HLS"), MonitorOn);

    connect_pv(lls, args.replace("$(P)$(M).LLS"), MonitorOn);

    connect_pv(en_dis, args.replace("$(P)$(M)_able"), MonitorOn);

    connect_pv(use_set, args.replace("$(P)$(M).SET"), MonitorOn);
    use_set.set_component(PVChoiceH(pvgroup->get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));

    connect_pv(dmov, args.replace("$(P)$(M).DMOV"), MonitorOn);

    connect_pv(stop, args.replace("$(P)$(M).STOP"), MonitorOff);
    stop.set_component(PVButton(pvgroup->get_pv(stop.pv_name), " STOP ", 1));
}

SmallMotorDisplay::SmallMotorDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {
    this->init();
}

ftxui::Component SmallMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component(),
	val.component(),
	Container::Horizontal({
	    twr.component(),
	    twv.component(),
	    twf.component(),
	}),
	Container::Horizontal({
	    use_set.component(), stop.component()
	})
    });
}

ftxui::Element SmallMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;
    Decorator ColorDisabled = bgcolor(Color::RGBA(80,10,4,230)) | color(Color::Black);
    return ftxui::vbox({

	desc.component()->Render() 
	    | EPICSColor::EDIT
	    | size(WIDTH, EQUAL, 20)
	    | underlined
	    | center,

	separatorEmpty(),

	hbox({
	    filler() | size(WIDTH, EQUAL, egu.value.size()+1),
	    text(lls.value ? unicode::rectangle(1) : "  ") | color(Color::Red),
	    separatorEmpty(), separatorEmpty(),
	    text(rbv.value) | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)),
	    separatorEmpty(), separatorEmpty(),
	    text(hls.value ? unicode::rectangle(1) : "  ") | color(Color::Red),
	    separatorEmpty(),
	    text(egu.value) | color(Color::Black)
	}) | center,

	hbox({
	    val.component()->Render()
		| (en_dis.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
		| size(WIDTH, EQUAL, 10)
		| (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty),
	}) | center,

	hbox({
	    twr.component()->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component()->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
	    separatorEmpty(),
	    twf.component()->Render() | color(Color::Black),
	}) | center,
	separatorEmpty(),
	hbox({
	    use_set.component()->Render() | EPICSColor::EDIT,
	    separatorEmpty(),
	    separatorEmpty(),
	    stop.component()->Render() | color(Color::Red) | bold,
	}) | center,
	separatorEmpty()

    })
    | size(WIDTH, EQUAL, 24)
    | border 
    | color(Color::Black) 
    | center 
    | bgcolor(Color::RGB(196,196,196));
}


void MediumMotorDisplay::init() {
    using namespace pvtui;

    connect_pv(desc, args.replace("$(P)$(M).DESC"), MonitorOn);
    desc.set_component(PVInput(pvgroup->get_pv(desc.pv_name), desc.value, PVPutType::String));
    
    connect_pv(val, args.replace("$(P)$(M).VAL"), MonitorOn);
    val.set_component(PVInput(pvgroup->get_pv(val.pv_name), val.value, PVPutType::Double));

    connect_pv(twr, args.replace("$(P)$(M).TWR"), MonitorOff);
    twr.set_component(PVButton(pvgroup->get_pv(twr.pv_name), " < ", 1));
    
    connect_pv(twv, args.replace("$(P)$(M).TWV"), MonitorOn);
    twv.set_component(PVInput(pvgroup->get_pv(twv.pv_name), twv.value, PVPutType::Double));

    connect_pv(twf, args.replace("$(P)$(M).TWF"), MonitorOff);
    twf.set_component(PVButton(pvgroup->get_pv(twf.pv_name), " > ", 1));

    connect_pv(rbv, args.replace("$(P)$(M).RBV"), MonitorOn);

    connect_pv(egu, args.replace("$(P)$(M).EGU"), MonitorOn);

    connect_pv(hls, args.replace("$(P)$(M).HLS"), MonitorOn);
    
    connect_pv(lls, args.replace("$(P)$(M).LLS"), MonitorOn);

    connect_pv(use_set, args.replace("$(P)$(M).SET"), MonitorOn);
    use_set.set_component(PVChoiceH(pvgroup->get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));

    connect_pv(dmov, args.replace("$(P)$(M).DMOV"), MonitorOn);

    connect_pv(spmg, args.replace("$(P)$(M).SPMG"), MonitorOn);
    spmg.set_component(PVChoiceV(pvgroup->get_pv(spmg.pv_name), spmg.value.choices, spmg.value.index));

    connect_pv(able, args.replace("$(P)$(M)_able"), MonitorOn);
    able.set_component(PVChoiceH(pvgroup->get_pv(able.pv_name), able.value.choices, able.value.index));

    connect_pv(dval, args.replace("$(P)$(M).DVAL"), MonitorOn);
    dval.set_component(PVInput(pvgroup->get_pv(dval.pv_name), dval.value, PVPutType::Double));

    connect_pv(hlm, args.replace("$(P)$(M).HLM"), MonitorOn);
    hlm.set_component(PVInput(pvgroup->get_pv(hlm.pv_name), hlm.value, PVPutType::Double));
    
    connect_pv(llm, args.replace("$(P)$(M).LLM"), MonitorOn);
    llm.set_component(PVInput(pvgroup->get_pv(llm.pv_name), llm.value, PVPutType::Double));
    
    connect_pv(dhlm, args.replace("$(P)$(M).DHLM"), MonitorOn);
    dhlm.set_component(PVInput(pvgroup->get_pv(dhlm.pv_name), dhlm.value, PVPutType::Double));
    
    connect_pv(dllm, args.replace("$(P)$(M).DLLM"), MonitorOn);
    dllm.set_component(PVInput(pvgroup->get_pv(dllm.pv_name), dllm.value, PVPutType::Double));

    connect_pv(drbv, args.replace("$(P)$(M).DRBV"), MonitorOn);
}

MediumMotorDisplay::MediumMotorDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args)
    : DisplayBase(pvgroup), args(args) {
    this->init();
}

ftxui::Component MediumMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component(),
	Container::Horizontal({
	    Container::Vertical({
		hlm.component(),
		val.component(),
		llm.component(),
	    }),
	    Container::Vertical({
		dhlm.component(),
		dval.component(),
		dllm.component(),
	    }),
	    Container::Vertical({
		spmg.component(),
	    }),
	}),
	Container::Horizontal({
	    twr.component(), twv.component(), twf.component()
	}),
	Container::Horizontal({
	    able.component(), use_set.component(),
	})
    });

}

ftxui::Element MediumMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;

    Decorator ColorDisabled = bgcolor(Color::RGBA(80,10,4,230)) | color(Color::Black);
    auto &desc_pv = pvgroup->get_pv(desc.pv_name);
    return ftxui::vbox({
	(desc_pv.connected() ? text("") : text("Disconnected") | color(Color::Red)) | center,

	desc.component()->Render() | color(Color::Black) | bgcolor(Color::RGB(210,210,210)) | size(WIDTH, EQUAL, 22) | center,
	separatorEmpty(),

	// 6 column hbox of vbox's
	// none | none | user | dial | lims/egu | spmg
	hbox({
	    filler() | size(WIDTH, EQUAL, egu.value.length()+8),
	    vbox({
		text("User") | center,
		hlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(rbv.value)
		    | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty)
		    | center,
		val.component()->Render()
		    | center
		    | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		separatorEmpty(), 	
		llm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(), 	
	    vbox({
		text("Dial") | center,
		dhlm.component()->Render()
		    | center
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
		text(drbv.value)
		    | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2))
		    | (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty)
		    | center,
		dval.component()->Render()
		    | center
		    | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
		    | size(WIDTH, EQUAL, 10)
		    | size(HEIGHT, EQUAL, 2),
		separatorEmpty(), 	
		dllm.component()->Render()
		    | EPICSColor::EDIT
		    | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(),
	    vbox({
		separatorEmpty(),
		text(hls.value ? unicode::rectangle(2) : "") | color(Color::Red),
		filler(),
		text(egu.value) | EPICSColor::READBACK,
		separatorEmpty(),
		text(lls.value ? unicode::rectangle(2) : "") | color(Color::Red),
	    }) | size(WIDTH, EQUAL, egu.value.length()),
	    separatorEmpty(),
	    spmg.component()->Render()
		| EPICSColor::EDIT
		| size(HEIGHT, EQUAL, 2),
	}) | center,
	
	separatorEmpty(), 	

	hbox({
	    twr.component()->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component()->Render()
		| center
		| EPICSColor::EDIT
		| size(WIDTH, EQUAL, 11),
	    separatorEmpty(),
	    twf.component()->Render() | color(Color::Black),
	}) | center,

	separatorEmpty(),

	hbox({
	    separatorEmpty(),
	    use_set.component()->Render() | EPICSColor::EDIT,
	    separatorEmpty(),
	    able.component()->Render() | EPICSColor::EDIT,
	}) | center,
	separatorEmpty(),

    })| center | EPICSColor::BACKGROUND;
}
