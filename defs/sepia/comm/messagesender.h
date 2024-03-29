#ifndef SEPIA_COMM_MESSAGESENDER_H
#define SEPIA_COMM_MESSAGESENDER_H
#include <vector>
#include <string>

namespace google
{
    namespace protobuf
    {
        class MessageLite;
    }
}

namespace sepia
{
namespace comm
{
class Header;

class MessageHandler;

class MessageSender
{
public:
    static void initClient();
    static void selectOutput( MessageHandler* a_handler );
    static void rawSend( char* a_header, std::size_t a_headerSize, const char* a_msg, std::size_t a_msgSize );
    static void rawSend( const google::protobuf::MessageLite* a_header, const char* a_msg, std::size_t a_msgSize );
    static void rawSend( const google::protobuf::MessageLite* a_header, const google::protobuf::MessageLite* a_msg );

protected:
    static sepia::comm::Header* sm_header;

private:
    MessageSender() = delete;
    static MessageHandler* sm_messageHandler;
    static std::vector< char > sm_buffer;
};
}
}

#endif // SEPIA_COMM_MESSAGESENDER_H
