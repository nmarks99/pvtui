#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <pvtui/pvtui.hpp>

namespace pvtui {

namespace unicode {

std::string rectangle(int len) {
    std::string out;
    out.reserve(len * full_block.length());
    for (int i = 0; i < len; i++) {
        out.append(full_block);
        out.append("\n");
    }
    return out;
}

} // namespace unicode

ftxui::Component make_button_widget(PVHandler &pv, const std::string &label, int value) {
    auto op = ftxui::ButtonOption::Ascii();
    op.label = label;
    op.on_click = [&pv, value]() {
        if (pv.connected()) {
            pv.channel.put().set("value", value).exec();
        }
    };
    return ftxui::Button(op);
};

ftxui::Component make_input_widget(PVHandler &pv, std::string &disp_str, PVPutType put_type,
                                   InputTransform tf = nullptr) {

    auto default_input_transform = [&pv, &disp_str](ftxui::InputState s) {
        if (not pv.connected()) {
            disp_str = "";
        }
        s.element |= ftxui::color(ftxui::Color::Black);
        if (s.is_placeholder) {
            s.element |= ftxui::dim;
        }
        if (s.focused) {
            s.element |= ftxui::inverted;
        } else if (s.hovered) {
            s.element |= ftxui::bgcolor(ftxui::Color::GrayDark);
        }
        return s.element;
    };
    return ftxui::Input(ftxui::InputOption({
        .content = &disp_str,
        .transform = tf ? tf : default_input_transform,
        .multiline = false,
        .on_enter =
            [&pv, &disp_str, put_type]() {
                if (pv.connected()) {
                    if (put_type == PVPutType::Double) {
                        try {
                            double val_double = std::stod(disp_str);
                            pv.channel.put().set("value", val_double).exec();
                        } catch (const std::exception &) {
                            // handle parse error if needed
                        }
                    } else if (put_type == PVPutType::String) {
                        pv.channel.put().set("value", disp_str).exec();
                    } else if (put_type == PVPutType::Integer) {
                        try {
                            int val_int = std::stoi(disp_str);
                            pv.channel.put().set("value", val_int).exec();
                        } catch (const std::exception &) {
                            // handle parse error if needed
                        }
                    }
                }
            },
    }));
}

ftxui::Component make_choice_h_widget(PVHandler &pv, const std::vector<std::string> &labels,
                                      int &selected) {
    ftxui::MenuOption op = ftxui::MenuOption::Toggle();
    op.entries = &labels;
    op.selected = &selected;
    op.on_change = [&]() {
        if (pv.connected()) {
            pv.channel.put().set("value.index", selected).exec();
        }
    };

    op.entries_option.transform = [&pv](const ftxui::EntryState &state) {
        ftxui::Element e = pv.connected() ? ftxui::text(state.label) : ftxui::text("    ");
        auto color = ftxui::color(ftxui::Color::Black);
        if (state.focused) {
            e |= color | ftxui::inverted;
        }
        if (!state.focused && !state.active) {
            e |= color | ftxui::dim;
        }
        return e;
    };

    return ftxui::Menu(op);
}

ftxui::Component make_choice_v_widget(PVHandler &pv, const std::vector<std::string> &labels,
                                      int &selected) {
    ftxui::MenuOption op = ftxui::MenuOption::Vertical();
    op.entries = &labels;
    op.selected = &selected;
    op.on_change = [&]() {
        if (pv.connected()) {
            pv.channel.put().set("value.index", selected).exec();
        }
    };
    op.entries_option.transform = [&pv](const ftxui::EntryState &state) {
        ftxui::Element e = pv.connected() ? ftxui::text(state.label) : ftxui::text("    ");
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

ftxui::Component make_dropdown_widget(PVHandler &pv, const std::vector<std::string> &labels,
                                      int &selected) {
    using namespace ftxui;
    auto dropdown_op = ftxui::DropdownOption({
        .radiobox = {.entries = &labels,
                     .selected = &selected,
                     .on_change =
                         [&]() {
                             if (pv.connected()) {
                                 pv.channel.put().set("value.index", selected).exec();
                             }
                         }},
        .transform =
            [](bool open, ftxui::Element checkbox, ftxui::Element radiobox) {
                if (open) {
                    return ftxui::vbox({
                        checkbox | inverted,
                        radiobox | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 10),
                        filler(),
                    });
                }
                return vbox({
                    checkbox,
                    filler(),
                });
            },

    });
    return ftxui::Dropdown(dropdown_op);
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

std::string ArgParser::replace(const std::string &str) const {
    std::string out = str;
    size_t ind = 0;
    for (auto &[k, v] : this->macros) {
        std::string pholder = "$(" + k + ")";
        while ((ind = out.find(pholder)) != std::string::npos) {
            out.replace(ind, 4, v);
        }
    }
    return out;
}

bool ArgParser::flag(const std::string &f) const { return cmdl_[f]; }

std::vector<std::string> ArgParser::split_string(const std::string &input, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(std::move(item));
    }
    return result;
}

std::unordered_map<std::string, std::string> ArgParser::get_macro_dict(std::string all_macros) {
    all_macros.erase(std::remove_if(all_macros.begin(), all_macros.end(),
                                    [](unsigned char s) { return std::isspace(s); }),
                     all_macros.end());

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

WidgetBase::WidgetBase(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name)
    : pv_name_(args.replace(pv_name)) {
    pvgroup.add(pv_name_);
    connection_monitor_ = std::make_unique<ConnectionMonitor>();
    pvgroup[pv_name_].channel.addConnectListener(connection_monitor_.get());
};

WidgetBase::WidgetBase(PVGroup &pvgroup, const std::string &pv_name) : pv_name_(pv_name) {
    pvgroup.add(pv_name_);
    connection_monitor_ = std::make_unique<ConnectionMonitor>();
    pvgroup[pv_name_].channel.addConnectListener(connection_monitor_.get());
};

std::string WidgetBase::pv_name() const { return pv_name_; }

bool WidgetBase::connected() const { return connection_monitor_->connected(); }

ftxui::Component WidgetBase::component() const {
    if (component_) {
        return component_;
    } else {
        throw std::runtime_error("No component defined for " + pv_name_);
    }
}

InputWidget::InputWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name,
                         PVPutType put_type, InputTransform tf)
    : WidgetBase(pvgroup, args, pv_name) {
    pvgroup.set_monitor(pv_name_, pv_value_);
    component_ = make_input_widget(pvgroup.get_pv(pv_name_), ui_value_, put_type, tf);
}

InputWidget::InputWidget(PVGroup &pvgroup, const std::string &pv_name, PVPutType put_type)
    : WidgetBase(pvgroup, pv_name) {
    pvgroup.set_monitor(pv_name_, pv_value_);
    component_ = make_input_widget(pvgroup.get_pv(pv_name_), ui_value_, put_type);
}

std::string InputWidget::value() const { return ui_value_; }

ChoiceWidget::ChoiceWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name,
                           ChoiceStyle style)
    : WidgetBase(pvgroup, args, pv_name) {
    pvgroup.set_monitor(pv_name_, pv_value_);
    switch (style) {
    case pvtui::ChoiceStyle::Vertical:
        component_ = make_choice_v_widget(pvgroup.get_pv(pv_name_), ui_value_.choices, ui_value_.index);
        break;
    case pvtui::ChoiceStyle::Horizontal:
        component_ = make_choice_h_widget(pvgroup.get_pv(pv_name_), ui_value_.choices, ui_value_.index);
        break;
    case pvtui::ChoiceStyle::Dropdown:
        component_ = make_dropdown_widget(pvgroup.get_pv(pv_name_), ui_value_.choices, ui_value_.index);
        break;
    }
}

ChoiceWidget::ChoiceWidget(PVGroup &pvgroup, const std::string &pv_name, ChoiceStyle style)
    : WidgetBase(pvgroup, pv_name) {
    pvgroup.set_monitor(pv_name_, pv_value_);
    switch (style) {
    case pvtui::ChoiceStyle::Vertical:
        component_ = make_choice_v_widget(pvgroup.get_pv(pv_name_), ui_value_.choices, ui_value_.index);
        break;
    case pvtui::ChoiceStyle::Horizontal:
        component_ = make_choice_h_widget(pvgroup.get_pv(pv_name_), ui_value_.choices, ui_value_.index);
        break;
    case pvtui::ChoiceStyle::Dropdown:
        component_ = make_dropdown_widget(pvgroup.get_pv(pv_name_), ui_value_.choices, ui_value_.index);
        break;
    }
}

PVEnum ChoiceWidget::value() const { return ui_value_; }

ButtonWidget::ButtonWidget(PVGroup &pvgroup, const ArgParser &args, const std::string &pv_name,
                           const std::string &label, int press_val)
    : WidgetBase(pvgroup, args, pv_name) {
    component_ = make_button_widget(pvgroup.get_pv(pv_name_), label, press_val);
}

ButtonWidget::ButtonWidget(PVGroup &pvgroup, const std::string &pv_name, const std::string &label,
                           int press_val)
    : WidgetBase(pvgroup, pv_name) {
    component_ = make_button_widget(pvgroup.get_pv(pv_name_), label, press_val);
}

} // namespace pvtui
