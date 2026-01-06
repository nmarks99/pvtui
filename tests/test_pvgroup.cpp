#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include <pv/caProvider.h>
#include <pva/client.h>
#include <pvtui/pvtui.hpp>

// To catch CTRL+C to quit
volatile std::sig_atomic_t g_signal_caught = 0;
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nKeyboard interrupt (Ctrl+C)" << std::endl;
        g_signal_caught = 1;
    }
}

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
    pvtui::PVGroup pvgroup(provider, {
	prefix+"m1.DESC",
	prefix+"m1.RBV",
    });

    double rbv;
    pvgroup.set_monitor<double>(prefix+"m1.RBV", rbv);

    std::string desc;
    pvgroup.set_monitor<std::string>(prefix+"m1.DESC", desc);

    while (g_signal_caught == 0) {
        if (pvgroup.sync()) {
	    std::cout << "DESC = " << desc << std::endl;
            std::cout << "RBV1 = " << rbv << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
