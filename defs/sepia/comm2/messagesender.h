#ifndef SEPIA_COMM2_MESSAGESENDER_H
#define SEPIA_COMM2_MESSAGESENDER_H
#include <vector>
#include <string>
#include <flatbuffers/flatbuffers.h>
#include <zmq.hpp>

namespace sepia
{
namespace comm2
{
class Header;

class MessageSender
{
public:
    static void init();
    static void rawSend( const std::string& a_name, const unsigned char* a_msg, size_t a_msgSize, bool a_local = false );

private:
    MessageSender() = delete;
    //static zmq::context_t sm_context;
    static zmq::socket_t  sm_localSocket;
    static zmq::socket_t  sm_externalSocket;
};

}
}

#endif // SEPIA_COMM2_MESSAGESENDER_H
