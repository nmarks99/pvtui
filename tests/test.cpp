#include <ostream>
#include <pv/caProvider.h>
#include <pva/client.h>

#include <sstream>
#include <stdexcept>
#include <thread>
#include <csignal>
#include <optional>

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
	"xxx:m1.VAL",
	"xxx:m1.RBV",
	"xxx:m1.TWV",
	"xxx:m1.TWF",
	"xxx:m1.TWR",
	"xxx:m1.PREC",
	"xxx:m1.DESC",
	"xxx:m1.SPMG",
	"xxx:long",
	"xxx:string",
	"xxx:float",
	"xxx:mbbo",
    });

    double rbv = 0;
    pvgroup.set_monitor("xxx:m1.RBV", rbv);

    int prec = 0;
    pvgroup.set_monitor("xxx:m1.PREC", prec);

    std::string desc = "";
    pvgroup.set_monitor("xxx:m1.DESC", desc);

    int spmg = 0;
    pvgroup.set_monitor("xxx:m1.SPMG", spmg);

    while(g_signal_caught == 0) {
	pvgroup.update();
	std::cout << "rbv = " << rbv << std::endl;
	std::cout << "desc = " << desc << std::endl;
	std::cout << "prec = " << prec << std::endl;
	std::cout << "spmg = " << spmg << std::endl;
	std::cout << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

}
