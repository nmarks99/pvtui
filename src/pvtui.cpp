#include <charconv>
#include "pvtui.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"

namespace unicode {
std::string rectangle(int width, int height) {
    std::string out;
    out.reserve(width*height*full_block.length()+height);
    for (int i = 0; i < height; i++){
	for (int j = 0; j < width; j++){
	   out.append(full_block);
	}
	out.append("\n");
    }
    return out;
}
}


ftxui::Component PVButton(ProcessVariable &pv, const std::string &label, int value) {
    auto op = ftxui::ButtonOption::Ascii();
    op.label = &label;
    op.on_click = [&](){
	if (pv.connected()) {
	    pv.channel.put().set("value", value).exec();
	}
    };
    return ftxui::Button(op);
};


ftxui::Component PVInput(ProcessVariable &pv, std::string &disp_str) {
    auto style = ftxui::InputOption::Default();
    return ftxui::Input(ftxui::InputOption({
	.content = &disp_str,
	.transform = [](ftxui::InputState s) {
	    return s.element | ftxui::center;
	},
	.multiline = false,
	.on_enter = [&pv, &disp_str](){
	    double val_double;
	    auto res = std::from_chars(disp_str.data(), disp_str.data()+disp_str.size(), val_double);
	    if (res.ec == std::errc()) {
		if (pv.connected()) {
		    pv.channel.put().set("value", val_double).exec();
		}
	    }
	},
    }));
}

ftxui::Component PVChoiceH(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected) {
    ftxui::MenuOption op = ftxui::MenuOption::Toggle();
    op.entries = &labels;
    op.selected = &selected;
    op.on_change = [&](){
	if (pv.connected()) {
	    // TODO: ProcessVariable struct should have put method than handles channel.put?
	    pv.channel.put().set("value.index", selected).exec();
	}
    };
    return ftxui::Menu(op);
}

ftxui::Component PVChoiceV(ProcessVariable &pv, const std::vector<std::string> &labels, int &selected) {
    ftxui::MenuOption op = ftxui::MenuOption::Vertical();
    op.entries = &labels;
    op.selected = &selected;
    op.on_change = [&](){
	if (pv.connected()) {
	    // TODO: ProcessVariable struct should have put method than handles channel.put?
	    pv.channel.put().set("value.index", selected).exec();
	}
    };
    op.entries_option.transform = [](const ftxui::EntryState& state) {
	ftxui::Element e = ftxui::text(state.label);
	if (state.focused) {
	    e |= ftxui::inverted;
	}
	if (state.active) {
	    e |= ftxui::bold;
	}
	if (!state.focused && !state.active) {
	    e |= ftxui::dim;
	}
	return e;
    };
    return ftxui::Menu(op);
}
