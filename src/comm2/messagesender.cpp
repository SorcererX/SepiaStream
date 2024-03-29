#include <sepia/comm2/messagesender.h>
#include <sepia/comm2/global.h>
#include <zmq.hpp>
#include <iostream>

namespace sepia
{
    namespace comm2
    {

        // zmq::context_t MessageSender::sm_context( 1 );
        thread_local zmq::socket_t MessageSender::stm_externalSocket( Global::instance()->getContext(), ZMQ_PUB );

        void MessageSender::init()
        {
            stm_externalSocket.connect( Global::instance()->getSubscriberUrl() );
        }

        void MessageSender::destroy()
        {
            stm_externalSocket.close();
        }

        void MessageSender::rawSend( const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize )
        {
            bool ok = false;
            try
            {
                ok = stm_externalSocket.send( a_name.data(), a_name.size(), ZMQ_SNDMORE );
                ok = stm_externalSocket.send( a_msg, a_msgSize );
            }
            catch( const zmq::error_t& ex )
            {
                std::cout << "MessageSender rawSend failed with: " << ex.what() << std::endl;
            }
        }
    }
}
