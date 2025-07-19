#pragma once
#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"


class SmallMotorDisplay : public DisplayBase {
  public:
    SmallMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args);
    ~SmallMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    // Create a PVWidget for each PV aware FTXUI component
    PVWidget<std::string> desc;
    PVWidget<std::string> val;
    PVWidget<NoValue> twr;
    PVWidget<std::string> twv;
    PVWidget<NoValue> twf;
    PVWidget<std::string> rbv;
    PVWidget<int> dmov;
    PVWidget<int> lls;
    PVWidget<int> hls;
    PVWidget<std::string> egu;
    PVWidget<PVEnum> en_dis;
    PVWidget<PVEnum> use_set;
    PVWidget<NoValue> stop;

    // initializes the above PVWidgets
    void init();

    // Store macro arguments
    const pvtui::ArgParser &args;
};


class MediumMotorDisplay : public DisplayBase {
  public:
    MediumMotorDisplay(pvac::ClientProvider &provider, const pvtui::ArgParser &args);
    ~MediumMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    // Create a PVWidget for each PV aware FTXUI component
    PVWidget<std::string> desc;
    PVWidget<std::string> val;
    PVWidget<NoValue> twr;
    PVWidget<std::string> twv;
    PVWidget<NoValue> twf;
    PVWidget<std::string> rbv;
    PVWidget<int> dmov;
    PVWidget<int> lls;
    PVWidget<int> hls;
    PVWidget<std::string> egu;
    PVWidget<PVEnum> use_set;
    PVWidget<std::string> drbv;
    PVWidget<std::string> dval;
    PVWidget<std::string> hlm;
    PVWidget<std::string> dhlm;
    PVWidget<std::string> llm;
    PVWidget<std::string> dllm;
    PVWidget<PVEnum> spmg;
    PVWidget<PVEnum> able;

    // initializes the above PVWidgets
    void init();

    // Store macro arguments
    const pvtui::ArgParser &args;
};
