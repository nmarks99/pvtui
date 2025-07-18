#include "motor_display.hpp"
#include "ftxui/component/component.hpp"
#include "pvtui.hpp"
#include <ftxui/dom/elements.hpp>

void SmallMotorDisplay::init() {
    using namespace pvtui;

    desc.pv_name = pvgroup.add("$(P)$(M).DESC", args.macros);
    pvgroup.set_monitor(desc.pv_name, desc.value);
    desc.component = PVInput(pvgroup[desc.pv_name], desc.value, PVPutType::String);
    
    val.pv_name = pvgroup.add("$(P)$(M).VAL", args.macros);
    pvgroup.set_monitor(val.pv_name, val.value);
    val.component = PVInput(pvgroup[val.pv_name], val.value, PVPutType::Double);

    twr.pv_name = pvgroup.add("$(P)$(M).TWR", args.macros);
    twr.component = PVButton(pvgroup[twr.pv_name], " < ", 1);
    
    twv.pv_name = pvgroup.add("$(P)$(M).TWV", args.macros);
    pvgroup.set_monitor(twv.pv_name, twv.value);
    twv.component = PVInput(pvgroup[twv.pv_name], twv.value, PVPutType::Double);

    twf.pv_name = pvgroup.add("$(P)$(M).TWF", args.macros);
    twf.component = PVButton(pvgroup[twf.pv_name], " > ", 1);

    rbv.pv_name = pvgroup.add("$(P)$(M).RBV", args.macros);
    pvgroup.set_monitor(rbv.pv_name, rbv.value);

    egu.pv_name = pvgroup.add("$(P)$(M).EGU", args.macros);
    pvgroup.set_monitor(egu.pv_name, egu.value);

    hls.pv_name = pvgroup.add("$(P)$(M).HLS", args.macros);
    pvgroup.set_monitor(hls.pv_name, hls.value);
    
    lls.pv_name = pvgroup.add("$(P)$(M).LLS", args.macros);
    pvgroup.set_monitor(lls.pv_name, lls.value);

    en_dis.pv_name = pvgroup.add("$(P)$(M)_able", args.macros);
    pvgroup.set_monitor(en_dis.pv_name, en_dis.value);

    use_set.pv_name = pvgroup.add("$(P)$(M).SET", args.macros);
    use_set.component = PVChoiceH(pvgroup[use_set.pv_name], use_set.value.choices, use_set.value.index);
    pvgroup.set_monitor(use_set.pv_name, use_set.value);

    dmov.pv_name = pvgroup.add("$(P)$(M).DMOV", args.macros);
    pvgroup.set_monitor(dmov.pv_name, dmov.value);

    stop.pv_name = pvgroup.add("$(P)$(M).STOP", args.macros);
    stop.component = PVButton(pvgroup[stop.pv_name], " STOP ", 1);
}

SmallMotorDisplay::SmallMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args)
    : DisplayBase(provider, args) {
    this->init();
}

ftxui::Component SmallMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component,
	val.component,
	Container::Horizontal({
	    twr.component,
	    twv.component,
	    twf.component,
	}),
	Container::Horizontal({
	    use_set.component, stop.component
	})
    });
}

ftxui::Element SmallMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;
    Decorator ColorDisabled = bgcolor(Color::RGBA(80,10,4,230)) | color(Color::Black);
    return ftxui::vbox({

	desc.component->Render() 
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
	    val.component->Render()
		| (en_dis.value.index==0 ? EPICSColor::EDIT : ColorDisabled)
		| size(WIDTH, EQUAL, 10)
		| (dmov.value == 0 ? borderHeavy | color(Color::Green) : borderEmpty),
	}) | center,

	hbox({
	    twr.component->Render() | color(Color::Black),
	    separatorEmpty(),
	    twv.component->Render() | EPICSColor::EDIT | size(WIDTH, EQUAL, 10),
	    separatorEmpty(),
	    twf.component->Render() | color(Color::Black),
	}) | center,
	separatorEmpty(),
	hbox({
	    use_set.component->Render() | EPICSColor::EDIT,
	    separatorEmpty(),
	    separatorEmpty(),
	    stop.component->Render() | color(Color::Red) | bold,
	}) | center,
	separatorEmpty()

    })
    | size(WIDTH, EQUAL, 25)  
    | border 
    | color(Color::Black) 
    | center 
    | bgcolor(Color::RGB(196,196,196));
}
