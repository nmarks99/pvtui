#include "display_base.hpp"

DisplayBase::DisplayBase(const std::shared_ptr<PVGroup> &pvgroup) : pvgroup(pvgroup) {}

bool DisplayBase::pv_update() { return pvgroup->update(); }
