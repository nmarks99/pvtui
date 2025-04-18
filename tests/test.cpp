#include <pv/caProvider.h>
#include <pva/client.h>

#include <csignal>
#include <ostream>
#include <chrono>
#include <thread>

#include "pvgroup.hpp"

volatile std::sig_atomic_t g_signal_caught = 0;
// Signal handler function
void signal_handler(int signal) {
    if (signal == SIGINT) {
	std::cout << "\nKeyboard interrupt (Ctrl+C)" << std::endl;
	g_signal_caught = 1;
    }
}

// std::vector<std::string> get_vector(pvac::ClientChannel &chan) {
    // auto pfield = chan.get();
    // std::cout << pfield << std::endl;
    // auto res = pfield->getSubFieldT<epics::pvData::PVStringArray>("value.choices");
    // epics::pvData::shared_vector<const std::string> data_view = res->view();
    // return std::vector<std::string>(data_view.begin(), data_view.end());
// }

int main() {

    // Register the signal handler for SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
	perror("signal");
	return 1;
    }

    // Instantiate EPICS client
    epics::pvAccess::ca::CAClientFactory::start();
    pvac::ClientProvider provider("ca");

    // std::ostringstream oss;
    // auto chan = provider.connect("xxx:mbbo");
    // auto pfield = chan.get();
    // auto res_value = pfield->getSubFieldT<epics::pvData::PVInt>("value.index");
    // auto res_choices = pfield->getSubFieldT<epics::pvData::PVStringArray>("value.choices");
    // epics::pvData::shared_vector<const std::string> choices = res_choices->view();
    // int index = res_value->getAs<int>();
    // std::cout << index << "\n";
    // if (choices.size() > index) {
	// std::cout << choices.at(index) << "\n";
    // } else {
	// std::cout << "No enum string for index " << index << std::endl;
    // }

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

    PVEnum spmg;
    pvgroup.set_monitor("xxx:m1.SPMG", spmg);

    while(g_signal_caught == 0) {
	pvgroup.update();
	std::cout << "rbv = " << rbv << std::endl;
	std::cout << "desc = " << desc << std::endl;
	std::cout << "prec = " << prec << std::endl;
	std::cout << "spmg.index = " << spmg.index << std::endl;
	std::cout << "spmg.choice = " << spmg.choice << std::endl;
	std::cout << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

