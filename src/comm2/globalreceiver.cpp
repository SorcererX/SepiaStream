#include <sepia/comm2/globalreceiver.h>
#include <sepia/comm2/internals.h>
#include <iostream>
#include <zmq.h>
#include <sepia/comm2/messagesender.h>

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
    m_internalSocket.bind( Internals::sm_internalSub );
    m_externalSocket.connect( Internals::sm_externalSub );
    try
    {
        zmq::proxy( static_cast< void* >( m_externalSocket ), static_cast< void* >( m_internalSocket ), NULL );
    }
    catch( const zmq::error_t& ex )
    {
        std::cout << __PRETTY_FUNCTION__ << " " << ex.what() << std::endl;
    }

    m_internalSocket.close();
    m_externalSocket.close();
    //std::cout << "Proxy completed." << std::endl;
}

void GlobalReceiver::stop()
{
    MessageSender::destroy();
    Internals::sm_context.close();
}


}
}
