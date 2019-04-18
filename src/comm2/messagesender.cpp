#include <sepia/comm2/messagesender.h>
#include <sepia/comm2/internals.h>
#include <zmq.hpp>
#include <iostream>

namespace sepia
{
namespace comm2
{

//zmq::context_t MessageSender::sm_context( 1 );
zmq::socket_t  MessageSender::sm_externalPubSocket( Internals::sm_context, ZMQ_PUB );
zmq::socket_t  MessageSender::sm_localPubSocket( Internals::sm_context, ZMQ_PUB ) ;
zmq::socket_t  MessageSender::sm_externalRequestSocket( Internals::sm_context, ZMQ_REQ );

void MessageSender::init()
{
    sm_externalPubSocket.connect( Internals::sm_externalPub );
    sm_localPubSocket.connect( Internals::sm_internalPub );
    sm_externalRequestSocket.connect( Internals::sm_externalRequest );
}

void MessageSender::destroy()
{
    sm_externalPubSocket.close();
    sm_localPubSocket.close();
    sm_externalRequestSocket.close();
}

void MessageSender::rawSocketSend( zmq::socket_t& a_socket, const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize )
{
    bool ok = false;
    try
    {
        ok = a_socket.send( a_name.data(), a_name.size(), ZMQ_SNDMORE );
        ok = a_socket.send( a_msg, a_msgSize );
    }
    catch( const zmq::error_t& ex )
    {
        //std::cout << __PRETTY_FUNCTION__ << " " << ex.what() << std::endl;
    }
}

void MessageSender::rawSend( const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize, bool a_local )
{
    if( !a_local )
    {
        rawSocketSend( sm_externalPubSocket, a_name, a_msg, a_msgSize );
    }
    else
    {
        rawSocketSend( sm_localPubSocket, a_name, a_msg, a_msgSize );
    }
}

void MessageSender::rawRequestSend( const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize )
{
    rawSocketSend( sm_externalRequestSocket, a_name, a_msg, a_msgSize );
}

}
}
