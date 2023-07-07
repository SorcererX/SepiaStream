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
    static void destroy();
    static void rawSend( const std::string& a_name, const unsigned char* a_msg, std::size_t a_msgSize, bool a_local = false );
    static void rawRequestSend( const std::string& a_name, const unsigned char* a_msg, std::size_t a_msgSize );

protected:
    static void rawSocketSend( zmq::socket_t& a_socket, const std::string& a_name, const unsigned char* a_msg, std::size_t a_msgSize );

private:
    MessageSender() = delete;
    static zmq::socket_t  sm_localPubSocket;
    static zmq::socket_t  sm_externalPubSocket;
    static zmq::socket_t  sm_externalRequestSocket;
};

}
}

#endif // SEPIA_COMM2_MESSAGESENDER_H
