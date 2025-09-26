#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <csignal>

#include <pv/caProvider.h>
#include <pva/client.h>
#include <pvtui/pvgroup.hpp>

// To catch CTRL+C to quit
volatile std::sig_atomic_t g_signal_caught = 0;
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nKeyboard interrupt (Ctrl+C)" << std::endl;
        g_signal_caught = 1;
    }
}


template <typename T>
struct DoubleBuffer {
    T pv_value;
    T app_value;
    std::mutex mutex;
};

DoubleBuffer<double> rbv;
DoubleBuffer<std::string> desc;

int main(int argc, char *argv[]) {

    std::string prefix;
    if (argc < 2) {
	std::cout << "Please provide IOC prefix" << std::endl;
	return EXIT_FAILURE;
    } else {
	prefix = argv[1];
    }

    signal(SIGINT, signal_handler);

    // Start the client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // Create the group and add our PVs
    PVGroup pvgroup(provider);

    pvgroup.add(prefix + "m1.DESC");
    pvgroup.set_monitor(prefix + "m1.DESC", desc.pv_value);
    pvgroup.add_sync_callback(prefix+"m1.DESC", [&](){
	std::lock_guard<std::mutex> lock(desc.mutex);
	desc.app_value = desc.pv_value;
    });

    pvgroup.add(prefix + "m1.RBV");
    pvgroup.set_monitor(prefix + "m1.RBV", rbv.pv_value);
    pvgroup.add_sync_callback(prefix+"m1.RBV", [&](){
	std::lock_guard<std::mutex> lock(rbv.mutex);
	rbv.app_value = rbv.pv_value;
    });

    while(g_signal_caught == 0) {
	if (pvgroup.data_available()) {
	    std::cout << "DESC = " << desc.app_value << std::endl;
	    std::cout << "RBV = " << rbv.app_value << std::endl;
	}
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
