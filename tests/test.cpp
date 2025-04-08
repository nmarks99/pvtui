#include <iostream>

// #if !defined(_WIN32)
// #include <signal.h>
// #define USE_SIGNAL
// #endif

#include <epicsEvent.h>
#include <pva/client.h>

static volatile bool done;

// #ifdef USE_SIGNAL
// static pvac::MonitorSync * volatile subscription;
// static
// void handler(int num)
// {
    // (void)num;
    // done = true;
    // pvac::MonitorSync *mon = subscription;
    // if(mon)
        // mon->wake();
// }
// #endif

int main(int argc, char *argv[]) {
    try {
        if(argc<=1) {
            std::cerr<<"Usage: "<<argv[0]<<" <pvname>\n";
            return 1;
        }

        pvac::ClientProvider provider("pva");

        pvac::ClientChannel channel(provider.connect(argv[1]));

        pvac::MonitorSync mon(channel.monitor());

        double val = 0.0;

// #ifdef USE_SIGNAL
        // subscription = &mon;
        // signal(SIGINT, handler);
        // signal(SIGTERM, handler);
        // signal(SIGQUIT, handler);
// #endif

        int ret = 0;

        while(!done) {
            if(!mon.wait()) // updates mon.event
                continue;

            switch(mon.event.event) {
            // Subscription network/internal error
            case pvac::MonitorEvent::Fail:
                std::cerr<<mon.name()<<" : Error : "<<mon.event.message<<"\n";
                ret = 1;
                done = true;
                break;
            // explicit call of 'mon.cancel' or subscription dropped
            case pvac::MonitorEvent::Cancel:
                std::cout<<mon.name()<<" <Cancel>\n";
                done = true;
                break;
            // Underlying channel becomes disconnected
            case pvac::MonitorEvent::Disconnect:
                std::cout<<mon.name()<<" <Disconnect>\n";
                break;
            // Data queue becomes not-empty
            case pvac::MonitorEvent::Data:
                // We drain event FIFO completely
                while(mon.poll()) {
                    // std::cout << mon.name() << " : " << mon.root;
                    // std::cout << "\n\n";
                    // std::cout << mon.root->getPVFields().at(0) << "\n";
                    val = mon.root.get()->getSubFieldT<epics::pvData::PVDouble>("value")->get();
                    std::cout << val << "\n";
                    // std::cout << mon.root->getPVFields().at(1) << "\n";
                    // std::cout << mon.root->getPVFields().at(2) << "\n";
                    // std::cout << mon.root->getPVFields().at(3) << "\n";
                    // std::cout<<mon.name()<<" : "<<mon.root->getSubFieldT<epics::pvData::PVDouble>("value")->getAs<double>();
		    // rbv = motor.rbv.get()->getSubFieldT<epics::pvData::PVDouble>("value")->getAs<double>();
                }
                // check to see if more events might be sent
                if(mon.complete()) {
                    done = true;
                    std::cout<<mon.name()<<" : <Complete>\n";
                }
                break;
            }
        }

// #ifdef USE_SIGNAL
        // signal(SIGINT, SIG_DFL);
        // signal(SIGTERM, SIG_DFL);
        // signal(SIGQUIT, SIG_DFL);
        // subscription = 0;
// #endif
        return ret;

    } catch(std::exception& e){
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
}
