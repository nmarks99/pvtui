#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

#include <pvtui/pvtui.hpp>

namespace pvtui {

/**
 * @brief Functions to generate FTXUI decorators for EPICS-style UI elements.
 * To align stylistically with MEDM, caQtDM etc, when PVs are disconnected, the widget
 * is drawn as a white rectangle
 */
namespace EPICSColor {
using namespace ftxui;

static const Decorator WHITE_ON_WHITE = bgcolor(Color::White) | color(Color::White);

template <typename Conn> inline Decorator edit(const Conn &c) {
    return c.connected() ? bgcolor(Color::RGB(87, 202, 228)) | color(Color::Black) : WHITE_ON_WHITE;
}

template <typename Conn> inline Decorator menu(const Conn &c) {
    return c.connected() ? bgcolor(Color::RGB(16, 105, 25)) | color(Color::White) : WHITE_ON_WHITE;
}

template <typename Conn> inline Decorator readback(const Conn &c) {
    return c.connected() ? bgcolor(Color::RGB(196, 196, 196)) | color(Color::DarkBlue) : WHITE_ON_WHITE;
}

template <typename Conn> inline Decorator link(const Conn &c) {
    return c.connected() ? bgcolor(Color::RGB(148, 148, 228)) | color(Color::Black) : WHITE_ON_WHITE;
}

template <typename Conn> inline Decorator custom(const Conn &c, Decorator style) {
    return c.connected() ? style : WHITE_ON_WHITE;
}

inline Decorator background() { return bgcolor(Color::RGB(196, 196, 196)); }
} // namespace EPICSColor

using ThemeInputTransform = std::function<ftxui::Element(ftxui::InputState, PVHandler&)>;
struct WidgetStyles {
    ThemeInputTransform input_style;
    // TODO: other widget styles
};

static WidgetStyles g_widget_styles {
    .input_style = [](ftxui::InputState s, PVHandler &pv) {
	s.element |= EPICSColor::edit(pv);
	if (s.is_placeholder) {
	    s.element |= ftxui::dim;
	}
	if (s.focused) {
	    s.element |= ftxui::inverted;
	} else if (s.hovered) {
	    s.element |= ftxui::bgcolor(ftxui::Color::GrayDark);
	}
	return s.element;
    }
};



} // namespace pvtui
