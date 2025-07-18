#pragma once
#include "pvgroup.hpp"
#include "pvtui.hpp"
#include "ftxui/component/component_base.hpp"
#include <pv/caProvider.h>
#include <pva/client.h>

class DisplayBase {
  
public:
    DisplayBase(pvac::ClientProvider &provider, const pvtui::ArgParser &args, const std::vector<std::string> pv_names);
    DisplayBase(pvac::ClientProvider &provider, const pvtui::ArgParser &args);
    virtual ~DisplayBase() = default;
    virtual bool pv_update();
    virtual ftxui::Element get_renderer() = 0;
    virtual ftxui::Component get_container() = 0;

  protected:
    PVGroup pvgroup;
    const pvtui::ArgParser &args;
};
