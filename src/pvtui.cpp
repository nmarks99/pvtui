#include <pvtui.hpp>
#include <charconv>

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


ftxui::Component PVButton(pvac::ClientChannel &pv_channel, const std::string &label, int value) {
    return ftxui::Button(ftxui::ButtonOption({
	.label = label,
	.on_click = [&pv_channel, value](){
	    pv_channel.put().set("value", value).exec();
	}
    }));
};


ftxui::Component PVInput(pvac::ClientChannel &pv_channel, std::string &disp_str) {
    return ftxui::Input(ftxui::InputOption({
	.content = &disp_str,
	.multiline = false,
	.on_enter = [&pv_channel, &disp_str](){
	    double val_double;
	    auto res = std::from_chars(disp_str.data(), disp_str.data()+disp_str.size(), val_double);
	    if (res.ec == std::errc()) {
		pv_channel.put().set("value", val_double).exec();
	    }
	},
    }));
}
