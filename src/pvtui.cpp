#include <pvtui.hpp>

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


ftxui::Component PVButton(const std::string &label, pvac::ClientChannel &pv_channel, int value) {
    return Button(ftxui::ButtonOption({
	.label = label,
	.on_click = [&pv_channel, value](){
	    pv_channel.put().set("value", value).exec();
	}
    }));
};
