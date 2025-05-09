#include <charconv>
#include "pvtui.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"

namespace pvtui {

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
    op.label = label;
    op.on_click = [&](){
	if (pv.connected()) {
	    pv.channel.put().set("value", value).exec();
	}
    };
    return ftxui::Button(op);
};


ftxui::Component PVInput(ProcessVariable &pv, std::string &disp_str, PVPutType put_type) {
    auto style = ftxui::InputOption::Default();
    return ftxui::Input(ftxui::InputOption({
	.content = &disp_str,
	.transform = [](ftxui::InputState s) {
	    return s.element | ftxui::center | ftxui::xflex;
	},
	.multiline = false,
	.on_enter = [&pv, &disp_str, put_type](){

	    if (put_type == PVPutType::Double) {
		double val_double;
		auto res = std::from_chars(disp_str.data(), disp_str.data()+disp_str.size(), val_double);
		if (res.ec == std::errc()) {
		    if (pv.connected()) {
			pv.channel.put().set("value", val_double).exec();
		    }
		}
	    } else if (put_type == PVPutType::String) {
		if (pv.connected()) {
		    pv.channel.put().set("value", disp_str).exec();
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


ArgParser::ArgParser(int argc, char *argv[]) {
    cmdl_.add_params({"-m", "--macro", "--macros"});
    cmdl_.add_params({"--provider"});
    cmdl_.parse(argc, argv);
    this->macros = get_macro_dict(cmdl_({"-m", "--macro", "--macros"}).str());
    this->provider = cmdl_("--provider").str().empty() ? "ca" : cmdl_("--provider").str();
};

bool ArgParser::macros_present(const std::vector<std::string> &macro_list) const {
    for (const auto &m : macro_list) {
	if (!this->macros.count(m)) {
	    return false;
	}
    }
    return true;
};

bool ArgParser::flag(const std::string &f) const {
    return cmdl_[f];
}

std::vector<std::string> ArgParser::split_string(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(std::move(item));
    }
    return result;
}

std::unordered_map<std::string, std::string> ArgParser::get_macro_dict(std::string all_macros) {
    all_macros.erase(std::remove_if(all_macros.begin(), all_macros.end(), [](unsigned char s){
	return std::isspace(s);
    }), all_macros.end());

    std::unordered_map<std::string, std::string> map_out;
    for (const auto &m : split_string(all_macros, ',')) {
	auto pair = split_string(m, '=');
	if (pair.size() != 2) {
	    return std::unordered_map<std::string, std::string>{};
	}
	map_out.emplace(std::move(pair.at(0)), std::move(pair.at(1)));
    }
    return map_out;
}

}
