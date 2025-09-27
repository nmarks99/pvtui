#include <chrono>
#include <csignal>
#include <iostream>
#include <mutex>
#include <thread>

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

    PVMonitorValue<std::string> desc(pvgroup, prefix + "m1.DESC");
    PVMonitorValue<double> rbv(pvgroup, prefix + "m1.RBV");

    while (g_signal_caught == 0) {
        if (pvgroup.data_available()) {
            std::cout << "DESC = " << desc.value << std::endl;
            std::cout << "RBV = " << rbv.value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
