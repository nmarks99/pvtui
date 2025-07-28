#include "display_base.hpp"

DisplayBase::DisplayBase(PVGroup &pvgroup) : pvgroup(pvgroup) {}

bool DisplayBase::pv_update() { return pvgroup.update(); }
