#include "motor_display.hpp"
#include "ftxui/component/component.hpp"
#include "pvtui.hpp"
#include <ftxui/dom/elements.hpp>

void SmallMotorDisplay::init() {
    using namespace pvtui;

    desc.pv_name = pvgroup->add("$(P)$(M).DESC", args.macros);
    pvgroup->set_monitor(desc.pv_name, desc.value);
    desc.set_component(PVInput(pvgroup->get_pv(desc.pv_name), desc.value, PVPutType::String));
    
    val.pv_name = pvgroup->add("$(P)$(M).VAL", args.macros);
    pvgroup->set_monitor(val.pv_name, val.value);
    val.set_component(PVInput(pvgroup->get_pv(val.pv_name), val.value, PVPutType::Double));

    twr.pv_name = pvgroup->add("$(P)$(M).TWR", args.macros);
    twr.set_component(PVButton(pvgroup->get_pv(twr.pv_name), " < ", 1));
    
    twv.pv_name = pvgroup->add("$(P)$(M).TWV", args.macros);
    pvgroup->set_monitor(twv.pv_name, twv.value);
    twv.set_component(PVInput(pvgroup->get_pv(twv.pv_name), twv.value, PVPutType::Double));

    twf.pv_name = pvgroup->add("$(P)$(M).TWF", args.macros);
    twf.set_component(PVButton(pvgroup->get_pv(twf.pv_name), " > ", 1));

    rbv.pv_name = pvgroup->add("$(P)$(M).RBV", args.macros);
    pvgroup->set_monitor(rbv.pv_name, rbv.value);

    egu.pv_name = pvgroup->add("$(P)$(M).EGU", args.macros);
    pvgroup->set_monitor(egu.pv_name, egu.value);

    hls.pv_name = pvgroup->add("$(P)$(M).HLS", args.macros);
    pvgroup->set_monitor(hls.pv_name, hls.value);
    
    lls.pv_name = pvgroup->add("$(P)$(M).LLS", args.macros);
    pvgroup->set_monitor(lls.pv_name, lls.value);

    en_dis.pv_name = pvgroup->add("$(P)$(M)_able", args.macros);
    pvgroup->set_monitor(en_dis.pv_name, en_dis.value);

    use_set.pv_name = pvgroup->add("$(P)$(M).SET", args.macros);
    use_set.set_component(PVChoiceH(pvgroup->get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));
    pvgroup->set_monitor(use_set.pv_name, use_set.value);

    dmov.pv_name = pvgroup->add("$(P)$(M).DMOV", args.macros);
    pvgroup->set_monitor(dmov.pv_name, dmov.value);

    stop.pv_name = pvgroup->add("$(P)$(M).STOP", args.macros);
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
	    filler() | size(WIDTH, EQUAL, egu.value.length()+1),
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
    | size(WIDTH, EQUAL, 25)  
    | border 
    | color(Color::Black) 
    | center 
    | bgcolor(Color::RGB(196,196,196));
}


void MediumMotorDisplay::init() {
    using namespace pvtui;

    desc.pv_name = pvgroup->add("$(P)$(M).DESC", args.macros);
    pvgroup->set_monitor(desc.pv_name, desc.value);
    desc.set_component(PVInput(pvgroup->get_pv(desc.pv_name), desc.value, PVPutType::String));
    
    val.pv_name = pvgroup->add("$(P)$(M).VAL", args.macros);
    pvgroup->set_monitor(val.pv_name, val.value);
    val.set_component(PVInput(pvgroup->get_pv(val.pv_name), val.value, PVPutType::Double));

    twr.pv_name = pvgroup->add("$(P)$(M).TWR", args.macros);
    twr.set_component(PVButton(pvgroup->get_pv(twr.pv_name), " < ", 1));
    
    twv.pv_name = pvgroup->add("$(P)$(M).TWV", args.macros);
    pvgroup->set_monitor(twv.pv_name, twv.value);
    twv.set_component(PVInput(pvgroup->get_pv(twv.pv_name), twv.value, PVPutType::Double));

    twf.pv_name = pvgroup->add("$(P)$(M).TWF", args.macros);
    twf.set_component(PVButton(pvgroup->get_pv(twf.pv_name), " > ", 1));

    rbv.pv_name = pvgroup->add("$(P)$(M).RBV", args.macros);
    pvgroup->set_monitor(rbv.pv_name, rbv.value);

    egu.pv_name = pvgroup->add("$(P)$(M).EGU", args.macros);
    pvgroup->set_monitor(egu.pv_name, egu.value);

    hls.pv_name = pvgroup->add("$(P)$(M).HLS", args.macros);
    pvgroup->set_monitor(hls.pv_name, hls.value);
    
    lls.pv_name = pvgroup->add("$(P)$(M).LLS", args.macros);
    pvgroup->set_monitor(lls.pv_name, lls.value);

    use_set.pv_name = pvgroup->add("$(P)$(M).SET", args.macros);
    use_set.set_component(PVChoiceH(pvgroup->get_pv(use_set.pv_name), use_set.value.choices, use_set.value.index));
    pvgroup->set_monitor(use_set.pv_name, use_set.value);

    dmov.pv_name = pvgroup->add("$(P)$(M).DMOV", args.macros);
    pvgroup->set_monitor(dmov.pv_name, dmov.value);

    spmg.pv_name = pvgroup->add("$(P)$(M).SPMG", args.macros);
    spmg.set_component(PVChoiceV(pvgroup->get_pv(spmg.pv_name), spmg.value.choices, spmg.value.index));
    pvgroup->set_monitor(spmg.pv_name, spmg.value);

    able.pv_name = pvgroup->add("$(P)$(M)_able", args.macros);
    able.set_component(PVChoiceH(pvgroup->get_pv(able.pv_name), able.value.choices, able.value.index));
    pvgroup->set_monitor(able.pv_name, able.value);

    dval.pv_name = pvgroup->add("$(P)$(M).DVAL", args.macros);
    dval.set_component(PVInput(pvgroup->get_pv(dval.pv_name), dval.value, PVPutType::Double));
    pvgroup->set_monitor(dval.pv_name, dval.value);

    hlm.pv_name = pvgroup->add("$(P)$(M).HLM", args.macros);
    hlm.set_component(PVInput(pvgroup->get_pv(hlm.pv_name), hlm.value, PVPutType::Double));
    pvgroup->set_monitor(hlm.pv_name, hlm.value);
    
    llm.pv_name = pvgroup->add("$(P)$(M).LLM", args.macros);
    llm.set_component(PVInput(pvgroup->get_pv(llm.pv_name), llm.value, PVPutType::Double));
    pvgroup->set_monitor(llm.pv_name, llm.value);
    
    dhlm.pv_name = pvgroup->add("$(P)$(M).DHLM", args.macros);
    dhlm.set_component(PVInput(pvgroup->get_pv(dhlm.pv_name), dhlm.value, PVPutType::Double));
    pvgroup->set_monitor(dhlm.pv_name, dhlm.value);
    
    dllm.pv_name = pvgroup->add("$(P)$(M).DLLM", args.macros);
    dllm.set_component(PVInput(pvgroup->get_pv(dllm.pv_name), dllm.value, PVPutType::Double));
    pvgroup->set_monitor(dllm.pv_name, dllm.value);

    drbv.pv_name = pvgroup->add("$(P)$(M).DRBV", args.macros);
    pvgroup->set_monitor(drbv.pv_name, drbv.value);

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

	desc.component()->Render() | color(Color::Black) | bgcolor(Color::RGB(210,210,210)) | size(WIDTH, EQUAL, 21) | center,
	separatorEmpty(),

	// 6 column hbox of vbox's
	// none | none | user | dial | lims/egu | spmg
	hbox({
	    filler() | size(WIDTH, EQUAL, egu.value.length()+8),
	    vbox({
		text("User") | center,
		hlm.component()->Render() | center | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		separatorEmpty(), 	
		text(rbv.value) | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)) | center,
		separatorEmpty(), 	
		val.component()->Render() | center | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		separatorEmpty(), 	
		llm.component()->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
	    }),
	    separatorEmpty(), 	
	    vbox({
		text("Dial") | center,
		dhlm.component()->Render() | center | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
		separatorEmpty(), 	
		text(drbv.value) | (use_set.value.index==0 ? EPICSColor::READBACK : color(Color::Yellow2)) | center,
		separatorEmpty(), 	
		dval.component()->Render() | center | (able.value.index==0 ? EPICSColor::EDIT : ColorDisabled) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 2),
		separatorEmpty(), 	
		dllm.component()->Render()  | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
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
	    vbox({
		text(dmov.value ? "" : "Moving") | color(Color::DarkGreen) | italic | bold,
		spmg.component()->Render() | EPICSColor::EDIT | center,
		separatorEmpty(),
	    }),
	    separatorEmpty(),
	}) | center,
	
	separatorEmpty(), 	

	hbox({
	    twr.component()->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component()->Render() | center | EPICSColor::EDIT | size(WIDTH, EQUAL, 11) | center,
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
    })
    | center 
    | bgcolor(Color::RGB(196,196,196));
}
