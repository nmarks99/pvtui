#include "motor_display.hpp"
#include "ftxui/component/component.hpp"

SmallMotorDisplay::SmallMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args)
    : DisplayBase(provider, args, {"namSoft:m1.DESC"}) {

    using namespace pvtui;

    desc.value = "";
    desc.component = PVInput(pvgroup_["namSoft:m1.DESC"], desc.value, PVPutType::String);
    pvgroup_.set_monitor("namSoft:m1.DESC", desc.value);
}

ftxui::Component SmallMotorDisplay::get_container() {
    using namespace ftxui;
    return Container::Vertical({
	desc.component,
    });
}

ftxui::Element SmallMotorDisplay::get_renderer() {
    using namespace ftxui;
    using namespace pvtui;
    return ftxui::vbox({
	desc.component->Render() | EPICSColor::EDIT
    });
}

bool SmallMotorDisplay::pv_update() {
    return pvgroup_.update();
};
