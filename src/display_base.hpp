#pragma once
#include "pvgroup.hpp"
#include "pvtui.hpp"
#include "ftxui/component/component_base.hpp"
#include <pv/caProvider.h>
#include <pva/client.h>

class DisplayBase {
  public:
    DisplayBase(pvac::ClientProvider &provider, const std::vector<std::string> pv_names);
    DisplayBase(pvac::ClientProvider &provider);
    virtual ~DisplayBase() = default;
    virtual bool pv_update();
    virtual ftxui::Element get_renderer() = 0;
    virtual ftxui::Component get_container() = 0;

  protected:
    PVGroup pvgroup;
    // const pvtui::ArgParser &args;
};


struct NoValue{};

template <typename T>
struct PVWidget {
    std::string pv_name;
    T value; 
    ftxui::Component component = nullptr;
    // ftxui::Component get_component() {
        // if (component) {
            // return component;
        // } else {
            // throw std::runtime_error("No component for " + pv_name);
        // }
    // };
};
