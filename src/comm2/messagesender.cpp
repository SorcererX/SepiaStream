#include <sepia/comm2/messagesender.h>
#include <zmq.hpp>
#include <iostream>

namespace sepia
{
namespace comm2
{

zmq::context_t MessageSender::sm_context( 1 );
zmq::socket_t  MessageSender::sm_externalSocket( sm_context, ZMQ_PUB );
zmq::socket_t  MessageSender::sm_localSocket( sm_context, ZMQ_PUB ) ;

void MessageSender::init()
{
    sm_externalSocket.connect( "tcp://127.0.0.1:31340" );
    sm_localSocket.connect( "tcp://127.0.0.1:31350" );
    //sm_socket.connect( "ipc:///tmp/cuttlefish_outgoing" );
}

void MessageSender::rawSend( const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize, bool a_local )
{
    //std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
    bool ok = false;
    if( !a_local )
    {
        ok = sm_externalSocket.send( a_name.data(), a_name.size(), ZMQ_SNDMORE );
        ok = sm_externalSocket.send( a_msg, a_msgSize );
    }
    else
    {
        ok = sm_localSocket.send( a_name.data(), a_name.size(), ZMQ_SNDMORE );
        ok = sm_localSocket.send( a_msg, a_msgSize );
    }
}
}

}
