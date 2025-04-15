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
    return ftxui::Button(ftxui::ButtonOption({
	.label = label,
	.on_click = [&](){
	    if (pv.connected()) {
		pv.channel.put().set("value", value).exec();
	    } 
	}
    }));
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
