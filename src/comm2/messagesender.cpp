#include <sepia/comm2/messagesender.h>
#include <sepia/comm2/internals.h>
#include <zmq.hpp>
#include <iostream>

namespace sepia
{
namespace comm2
{

//zmq::context_t MessageSender::sm_context( 1 );
zmq::socket_t  MessageSender::sm_externalSocket( Internals::sm_context, ZMQ_PUB );
zmq::socket_t  MessageSender::sm_localSocket( Internals::sm_context, ZMQ_PUB ) ;

void MessageSender::init()
{
    sm_externalSocket.connect( Internals::sm_externalPub );
    sm_localSocket.connect( Internals::sm_internalPub );
}

void MessageSender::destroy()
{
    sm_externalSocket.close();
    sm_localSocket.close();
}

void MessageSender::rawSend( const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize, bool a_local )
{
    //std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
    bool ok = false;
    if( !a_local )
    {
        try
        {
            ok = sm_externalSocket.send( a_name.data(), a_name.size(), ZMQ_SNDMORE );
            ok = sm_externalSocket.send( a_msg, a_msgSize );
        }
        catch( const zmq::error_t& ex )
        {
            //std::cout << __PRETTY_FUNCTION__ << " " << ex.what() << std::endl;
        }
    }
    else
    {
        try
        {
            ok = sm_localSocket.send( a_name.data(), a_name.size(), ZMQ_SNDMORE );
            ok = sm_localSocket.send( a_msg, a_msgSize );
        }
        catch( const zmq::error_t& ex )
        {
            //std::cout << __PRETTY_FUNCTION__ << " " << ex.what() << std::endl;
        }
    }
}
}

}
