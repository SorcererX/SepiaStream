#ifndef SEPIA_COMM_MESSAGESENDER_H
#define SEPIA_COMM_MESSAGESENDER_H
#include <vector>
#include <string>
#include <messages.pb.h>

namespace sepia
{
namespace comm
{
class MessageHandler;

class MessageSender
{
public:
    static void initClient();
    static void selectOutput( MessageHandler* a_handler );

private:
    MessageSender() = delete;
    static MessageHandler* sm_messageHandler;
    static cuttlefish_msgs::Header* sm_header;
    static std::vector< char > sm_buffer;
};
}
}

#endif // SEPIA_COMM_MESSAGESENDER_H
