#include <ostream>
#include <pv/caProvider.h>
#include <pva/client.h>

#include <thread>
#include <csignal>

#include "pvgroup.hpp"

volatile std::sig_atomic_t g_signal_caught = 0;
// Signal handler function
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nKeyboard interrupt (Ctrl+C)" << std::endl;
        g_signal_caught = 1;
    }
}

int main() {

    // Register the signal handler for SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }
    
    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    PVGroup pvgroup(provider, {
	"namSoft:m1.VAL",
	"namSoft:m1.RBV",
	"namSoft:m1.TWV",
	"namSoft:m1.TWF",
	"namSoft:m1.TWR",
	"namSoft:m1.PREC",
	"namSoft:m1.DESC",
	"test:int",
	"test:string",
	"test:float",
    });

    bool okay = false;
    
    double rbv = 0;
    okay = pvgroup.create_monitor("namSoft:m1.RBV", rbv);
    if (not okay) {
	std::cout << "Failed to create monitor rbv" << std::endl;
    }

    int prec = 0;
    okay = pvgroup.create_monitor("namSoft:m1.PREC", prec);
    if (not okay) {
	std::cout << "Failed to create monitor prec" << std::endl;
    }

    std::string desc = "";
    okay = pvgroup.create_monitor("namSoft:m1.DESC", desc);
    if (not okay) {
	std::cout << "Failed to create monitor desc" << std::endl;
    }

    while(g_signal_caught == 0) {
	pvgroup.update();
	std::cout << "rbv = " << rbv << std::endl;
	std::cout << "desc = " << desc << std::endl;
	std::cout << "prec = " << prec << std::endl;
	std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}
