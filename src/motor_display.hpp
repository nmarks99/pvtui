#pragma once
#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"

template <typename T>
struct Widget {
    T value; 
    ftxui::Component component;
};

class SmallMotorDisplay : public DisplayBase {
  public:
    SmallMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args);
    ~SmallMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;
    bool pv_update() override;

  private:
    Widget<std::string> desc;
};
