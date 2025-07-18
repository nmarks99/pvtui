#pragma once
#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"

struct NoValue{};

template <typename T>
struct Widget {
    std::string pv_name;
    T value; 
    ftxui::Component component;
};

class SmallMotorDisplay : public DisplayBase {
  public:
    SmallMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args);
    ~SmallMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    void init();
    Widget<std::string> desc;
    Widget<std::string> val;
    Widget<NoValue> twr;
    Widget<std::string> twv;
    Widget<NoValue> twf;
    Widget<std::string> rbv;
    Widget<int> dmov;
    Widget<int> lls;
    Widget<int> hls;
    Widget<std::string> egu;
    Widget<PVEnum> en_dis;
    Widget<PVEnum> use_set;
    Widget<NoValue> stop;
};
