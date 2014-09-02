#include <iostream>
#include <sepia/comm/scom.h>
#include <messagerouter.h>
#include <processmonitor.h>
#include <sepia/comm/observer.h>

int main()
{
    sepia::comm::initRouter();

    MessageRouter router( "cuttlefish_outgoing" );
    ProcessMonitor* monitor = new ProcessMonitor();
    router.setProcessMonitor( monitor );
    monitor->start();

    sepia::comm::GlobalReceiver recv;
    recv.start();
    while( true )
    {
        sepia::comm::ObserverBase::threadReceiver();
    }
}

