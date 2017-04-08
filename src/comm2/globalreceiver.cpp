#include <sepia/comm2/globalreceiver.h>
#include <sepia/comm2/internals.h>
#include <iostream>

namespace sepia
{
namespace comm2
{

GlobalReceiver::GlobalReceiver() : m_internalSocket( Internals::sm_context, ZMQ_XPUB )
                                 , m_externalSocket( Internals::sm_context, ZMQ_XSUB )
{
}

GlobalReceiver::~GlobalReceiver()
{

}

void GlobalReceiver::start()
{
    m_thread = new std::thread( std::bind( &GlobalReceiver::own_thread, this ) );
}

void GlobalReceiver::own_thread()
{
    m_internalSocket.bind( "inproc://internal_pubsub" );
    m_externalSocket.connect( "tcp://127.0.0.1:31339" );
    zmq::proxy( static_cast< void* >( m_externalSocket ), static_cast< void* >( m_internalSocket ), NULL );
    std::cout << "Proxy completed." << std::endl;
}

void GlobalReceiver::stop()
{
    // TODO: how to stop gracefully.
}


}
}
