#include <pv/caProvider.h>
#include <pva/client.h>

#include <csignal>
#include <ostream>
#include <chrono>
#include <thread>

#include <pvtui/pvgroup.hpp>

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
	"nmarksTestIOC:m1.VAL",
	"nmarksTestIOC:m1.RBV",
	"nmarksTestIOC:m1.TWV",
	"nmarksTestIOC:m1.TWF",
	"nmarksTestIOC:m1.TWR",
	"nmarksTestIOC:m1.PREC",
	"nmarksTestIOC:m1.DESC",
	"nmarksTestIOC:m1.SPMG",
	"nmarksTestIOC:m1.VELO",
	"nmarksTestIOC:long",
	"nmarksTestIOC:string",
	"nmarksTestIOC:float",
	"nmarksTestIOC:enum",
	"nmarksTestIOC:double_array",
	"nmarksTestIOC:long_array",
	"nmarksTestIOC:string_array"
    });

    double rbv = 0;
    pvgroup.set_monitor("nmarksTestIOC:m1.RBV", rbv);

    int prec = 0;
    pvgroup.set_monitor("nmarksTestIOC:m1.PREC", prec);

    std::string desc = "";
    pvgroup.set_monitor("nmarksTestIOC:m1.DESC", desc);

    PVEnum spmg;
    pvgroup.set_monitor("nmarksTestIOC:m1.SPMG", spmg);

    // almost any PV's value can be dumped to a string
    std::string velo_any;
    pvgroup.set_monitor("nmarksTestIOC:m1.VELO", velo_any);

    int long_val;
    pvgroup.set_monitor("nmarksTestIOC:long", long_val);

    std::vector<double> double_arr;
    pvgroup.set_monitor("nmarksTestIOC:double_array", double_arr);

    std::vector<int> long_arr;
    pvgroup.set_monitor("nmarksTestIOC:long_array", long_arr);

    std::vector<std::string> string_arr;
    pvgroup.set_monitor("nmarksTestIOC:string_array", string_arr);

    while(g_signal_caught == 0) {
	std::cout << "nmarksTestIOC:m1.RBV = " << rbv << std::endl;

	std::cout << "nmarksTestIOC:m1.DESC = " << desc << std::endl;

	std::cout << "nmarksTestIOC:m1.PREC = " << prec << std::endl;

	std::cout << "nmarksTestIOC:m1.SPMG = " << spmg.choice << std::endl;

	std::cout << "nmarksTestIOC:m1.VELO (string) = " << velo_any << std::endl;

	std::cout << "nmarksTestIOC:long = " << long_val << std::endl;

	std::cout << "nmarksTestIOC:double_array = ";
	for (const auto &v : double_arr) {
	    std::cout << v << " ";
	}
	std::cout << "\n";

	std::cout << "nmarksTestIOC:long_array = ";
	for (const auto &v : long_arr) {
	    std::cout << v << " ";
	}
	std::cout << "\n";

	std::cout << "nmarksTestIOC:string_array = ";
	for (const auto &v : string_arr) {
	    std::cout << v << " ";
	}
	std::cout << "\n";

	std::cout << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

