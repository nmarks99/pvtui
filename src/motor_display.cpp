#include "motor_display.hpp"
#include "ftxui/component/component.hpp"

SmallMotorDisplay::SmallMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args)
    : DisplayBase(provider, args)
{

    using namespace pvtui;

    desc.pv_name = pvgroup_.add("$(P)$(M).DESC", args.macros);
    desc.component = PVInput(pvgroup_[desc.pv_name], desc.value, PVPutType::String);
    pvgroup_.set_monitor<std::string>(desc.pv_name, desc.value);

    rbv.pv_name = pvgroup_.add("$(P)$(M).RBV", args.macros);
    pvgroup_.set_monitor(rbv.pv_name, rbv.value);

    val.pv_name = pvgroup_.add("$(P)$(M).VAL", args.macros);
    val.component = PVInput(pvgroup_[val.pv_name], val.value, PVPutType::Double);
    pvgroup_.set_monitor<std::string>(val.pv_name, val.value);
}

ftxui::Component SmallMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component,
	val.component
    });
}

ftxui::Element SmallMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;
    return ftxui::vbox({
	desc.component->Render() | EPICSColor::EDIT,
	separatorEmpty(),
	text(std::to_string(rbv.value)) | EPICSColor::READBACK | center,
	separatorEmpty(),
	val.component->Render(),
	separatorEmpty(),
    }) | center | bgcolor(Color::RGB(196,196,196));
}
