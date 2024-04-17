#include <epicsEvent.h>
#include <iostream>
#include <pva/client.h>

static volatile bool done;

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        std::cout << "Must provide a single PV to monitor" << std::endl;
        return 1;
    }

    pvac::ClientProvider provider("pva");
    pvac::ClientChannel channel(provider.connect(argv[1]));
    pvac::MonitorSync mon(channel.monitor());

    int ret = 0;
    while (not done) {
        if (not mon.wait()) {
            continue;
        }

        switch (mon.event.event) {
        case pvac::MonitorEvent::Fail:
            std::cout << mon.name() << "Error: " << mon.event.message << std::endl;
            ret = 1;
            done = true;
            break;
        case pvac::MonitorEvent::Cancel:
            std::cout << mon.name() << "<Cancel>" << std::endl;
            done = true;
            break;
        case pvac::MonitorEvent::Disconnect:
            std::cout << mon.name() << "Disconnect" << std::endl;
            break;
        case pvac::MonitorEvent::Data:
            while (mon.poll()) {
                std::cout << mon.name() << " : " << mon.root->getSubField("value") << std::endl;
            }
            if (mon.complete()) {
                done = true;
                std::cout << "\n\nCOMPLETE\n\n" << std::endl;
                // std::cout << mon.name() << " : <Complete>" << std::endl;
            }
            break;
        }
    }

    return ret;
}
