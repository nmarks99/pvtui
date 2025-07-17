#pragma once
#include "pvgroup.hpp"
#include "pvtui.hpp"
#include "ftxui/component/component_base.hpp"
#include <pv/caProvider.h>
#include <pva/client.h>

class DisplayBase {
  public:
    DisplayBase(pvac::ClientProvider &provider, const pvtui::ArgParser &args, const std::vector<std::string> pv_names);
    virtual ~DisplayBase() = default;
    // virtual void pv_init() = 0;
    virtual bool pv_update() = 0;
    // PVGroup& get_pvs() { return pvgroup_; };
    virtual ftxui::Element get_renderer() = 0;
    virtual ftxui::Component get_container() = 0;
  protected:
    PVGroup pvgroup_;
    const pvtui::ArgParser &args_;
    // virtual std::vector<std::string> get_pv_names() = 0;
};
