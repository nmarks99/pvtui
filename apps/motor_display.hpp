#pragma once
#include <pv/caProvider.h>
#include <pva/client.h>

#include "display_base.hpp"
#include "pvtui.hpp"

class SmallMotorDisplay : public DisplayBase {
  public:
    SmallMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args);
    ~SmallMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    pvtui::InputWidget desc;
    pvtui::InputWidget val;
    pvtui::ButtonWidget twr;
    pvtui::InputWidget twv;
    pvtui::ButtonWidget twf;
    pvtui::VarWidget<std::string> rbv;
    pvtui::VarWidget<int> dmov;
    pvtui::VarWidget<int> lls;
    pvtui::VarWidget<int> hls;
    pvtui::VarWidget<int> lvio;
    pvtui::VarWidget<std::string> egu;
    pvtui::ChoiceWidget able;
    pvtui::ChoiceWidget use_set;
    pvtui::ButtonWidget stop;

    const pvtui::ArgParser &args;
};


class MediumMotorDisplay : public DisplayBase {
  public:
    MediumMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args);
    ~MediumMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    pvtui::InputWidget desc;
    pvtui::InputWidget val;
    pvtui::ButtonWidget twr;
    pvtui::InputWidget twv;
    pvtui::ButtonWidget twf;
    pvtui::VarWidget<std::string> rbv;
    pvtui::VarWidget<int> dmov;
    pvtui::VarWidget<int> lls;
    pvtui::VarWidget<int> hls;
    pvtui::VarWidget<int> lvio;
    pvtui::VarWidget<std::string> egu;
    pvtui::ChoiceWidget use_set;
    pvtui::VarWidget<std::string> drbv;
    pvtui::InputWidget dval;
    pvtui::InputWidget hlm;
    pvtui::InputWidget dhlm;
    pvtui::InputWidget llm;
    pvtui::InputWidget dllm;
    pvtui::ChoiceWidget spmg;
    pvtui::ChoiceWidget able;

    const pvtui::ArgParser &args;
};

class AllMotorDisplay : public DisplayBase {
  public:
    AllMotorDisplay(PVGroup &pvgroup, const pvtui::ArgParser &args);
    ~AllMotorDisplay() override = default;
    ftxui::Element get_renderer() override;
    ftxui::Component get_container() override;

  private:
    pvtui::InputWidget desc;
    pvtui::InputWidget val;
    pvtui::ButtonWidget twr;
    pvtui::InputWidget twv;
    pvtui::ButtonWidget twf;
    pvtui::VarWidget<std::string> rbv;
    pvtui::VarWidget<int> dmov;
    pvtui::VarWidget<int> lls;
    pvtui::VarWidget<int> hls;
    pvtui::VarWidget<int> lvio;
    pvtui::InputWidget egu;
    pvtui::ChoiceWidget use_set;
    pvtui::VarWidget<std::string> drbv;
    pvtui::InputWidget dval;
    pvtui::InputWidget hlm;
    pvtui::InputWidget dhlm;
    pvtui::InputWidget llm;
    pvtui::InputWidget dllm;
    pvtui::ChoiceWidget spmg;
    pvtui::ChoiceWidget able;
    pvtui::InputWidget vmax;
    pvtui::InputWidget velo;
    pvtui::InputWidget vbas;
    pvtui::InputWidget accl;
    pvtui::InputWidget mres;
    pvtui::InputWidget eres;
    pvtui::InputWidget rres;
    pvtui::InputWidget rtry;
    pvtui::InputWidget off;
    pvtui::InputWidget prec;
    pvtui::InputWidget rlv;
    pvtui::InputWidget rval;
    pvtui::ChoiceWidget ueip;
    pvtui::ChoiceWidget urip;
    pvtui::ChoiceWidget dir;
    pvtui::ChoiceWidget cnen;
    pvtui::ChoiceWidget foff;
    pvtui::VarWidget<std::string> rrbv;

    const pvtui::ArgParser &args;
};
