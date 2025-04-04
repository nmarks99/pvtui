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


ftxui::Component PVButton(pvac::ClientChannel &pv_channel, const std::string &label, int value) {
    return ftxui::Button(ftxui::ButtonOption({
	.label = label,
	.on_click = [&pv_channel, value](){
	    pv_channel.put().set("value", value).exec();
	}
    }));
};


// ftxui::Component PVInput(pvac::ClientChannel &pv_channel, const std::string &strval) {
    // return ftxui::Input(ftxui::InputOption({
	// .on_enter = [&pv_channel, strval](){
	    // const double doubleval = std::stod(strval);
	    // pv_channel.put().set("value", doubleval).exec();
	    // // TODO: get() pv value and set displayed input to it
	// },
	// .
    // }));
// }
