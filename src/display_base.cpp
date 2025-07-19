#include "display_base.hpp"

DisplayBase::DisplayBase(pvac::ClientProvider &provider, const std::vector<std::string> pv_names)
    : pvgroup(provider, pv_names)
{}

DisplayBase::DisplayBase(pvac::ClientProvider &provider)
    : pvgroup(provider)
{}

bool DisplayBase::pv_update() { return pvgroup.update(); }
