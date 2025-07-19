#pragma once
#include "ftxui/component/component_base.hpp"
#include "pvgroup.hpp"
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
};

struct NoValue {};


// convenience struct for holding related pv name, component value, and component
template <typename T> struct PVWidget {
  public:
    std::string pv_name = "";
    T value;
    void set_component(ftxui::Component component) {
        component_ = component;
    };
    ftxui::Component component() {
        if (component_) {
            return component_;
        } else {
            throw std::runtime_error("No component defined for " + pv_name);
        }
    };

  private:
    ftxui::Component component_ = nullptr;
};
