#include "pvgroup.hpp"

bool PVGroup::create_monitor(const std::string &pv_name, double &var) {
    if (channels.count(pv_name)) {
	if (!monitors.count(pv_name)) {
	    // create monitor if there isn't one
	    monitors[pv_name].first = channels.at(pv_name).monitor();
	}
	// if there is, set the variable which is updated
	monitors[pv_name].second = &var;
	return true;
    } else {
	return false;
    }
}

void PVGroup::update() {
    for (auto &[pv_name, pair] : monitors) {
	if (pair.first.test()) {
	    switch (pair.first.event.event) {
	    case pvac::MonitorEvent::Data:
		while(pair.first.poll()) {
		    *pair.second = pair.first.root.get()->getSubFieldT<epics::pvData::PVDouble>("value")->get();
		}
		break;
	    case pvac::MonitorEvent::Cancel:
		break;
	    case pvac::MonitorEvent::Disconnect:
		break;
	    case pvac::MonitorEvent::Fail:
		break;
	    }
	}
    }
}
