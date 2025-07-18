#include "display_base.hpp"

DisplayBase::DisplayBase(pvac::ClientProvider &provider, const pvtui::ArgParser &args, const std::vector<std::string> pv_names)
    : pvgroup(provider, pv_names), args(args)
{}

DisplayBase::DisplayBase(pvac::ClientProvider &provider, const pvtui::ArgParser &args)
    : pvgroup(provider), args(args)
{}

bool DisplayBase::pv_update() { return pvgroup.update(); }
