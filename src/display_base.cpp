#include "display_base.hpp"

DisplayBase::DisplayBase(pvac::ClientProvider &provider, const pvtui::ArgParser &args, const std::vector<std::string> pv_names)
    : pvgroup_(provider, pv_names), args_(args)
{}
