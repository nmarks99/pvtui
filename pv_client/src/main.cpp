#include <iostream>
#include <epicsEvent.h>
#include <pva/client.h>

constexpr char PV_NAME[] = "nam:value";
static volatile bool done;

int main(int argc, char *argv[]) {
    
    pvac::ClientProvider provider("pva");
    pvac::ClientChannel channel(provider.connect(PV_NAME));
    pvac::MonitorSync mon(channel.monitor());
    
    int ret = 0;
    while (not done) {
        if (not mon.wait()) {
            continue;
        }

        switch(mon.event.event) {
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
                    std::cout << mon.name() << " : " << mon.root;
                }
                if (mon.complete()) {
                    done = true;
                    std::cout << mon.name() << " : <Complete>" << std::endl;
                }
                break;
        }
        return ret;
    }


    return 0;
}
