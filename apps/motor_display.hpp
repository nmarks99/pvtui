#pragma once
#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"


class SmallMotorDisplay : public DisplayBase {
  public:
    SmallMotorDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args);
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
    MediumMotorDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args);
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

class AllMotorDisplay : public DisplayBase {
  public:
    AllMotorDisplay(const std::shared_ptr<PVGroup> &pvgroup, const pvtui::ArgParser &args);
    ~AllMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    // Create a PVWidget for each PV aware FTXUI component
    PVWidget<std::string> desc;
    PVWidget<std::string> vmax;
    PVWidget<std::string> velo;
    PVWidget<std::string> vbas;
    PVWidget<std::string> accl;
    PVWidget<std::string> mres;
    PVWidget<std::string> eres;
    PVWidget<std::string> rres;
    PVWidget<std::string> rtry;
    PVWidget<PVEnum> ueip;
    PVWidget<PVEnum> urip;
    PVWidget<PVEnum> use_set;
    PVWidget<std::string> off;
    PVWidget<PVEnum> dir;
    PVWidget<PVEnum> foff;
    PVWidget<std::string> prec;
    PVWidget<PVEnum> cnen;
    PVWidget<std::string> rbv;
    PVWidget<std::string> drbv;
    PVWidget<std::string> val;
    PVWidget<std::string> dval;
    PVWidget<std::string> hlm;
    PVWidget<std::string> llm;
    PVWidget<std::string> dhlm;
    PVWidget<std::string> dllm;
    PVWidget<int> dmov;
    PVWidget<PVEnum> able;
    PVWidget<PVEnum> spmg;
    PVWidget<std::string> rval;
    PVWidget<std::string> rrbv;
    PVWidget<std::string> twv;
    PVWidget<NoValue> twf;
    PVWidget<NoValue> twr;
    PVWidget<std::string> rlv;
    PVWidget<int> hls;
    PVWidget<int> lls;
    PVWidget<std::string> egu;

    // initializes the above PVWidgets
    void init();

    // Store macro arguments
    const pvtui::ArgParser &args;
};
