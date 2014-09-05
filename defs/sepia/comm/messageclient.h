#ifndef SEPIA_COMM_MESSAGECLIENT_H
#define SEPIA_COMM_MESSAGECLIENT_H
#include <vector>
#include <string>

namespace google {
   namespace protobuf {
      class MessageLite;
   }
}

namespace cuttlefish_msgs {
   class Header;
}


namespace sepia
{
namespace comm
{
class MessageHandler;

class MessageClient
{
public:
    MessageClient( const std::string a_queue );
    void sendMessage( google::protobuf::MessageLite* a_message );
    void receiveMessage( cuttlefish_msgs::Header* a_header, google::protobuf::MessageLite* a_message );

protected:
    void sendIdRequest();
    void getIdResponse();
    void init( const std::string a_queueName );

private:
    cuttlefish::MessageHandler* m_incomingQueue;
    cuttlefish::MessageHandler* m_outgoingQueue;
    std::vector< char > m_sendBuffer;
    std::vector< char > m_receiveBuffer;
    std::string m_incomingQueueName;
    cuttlefish_msgs::Header* m_sendHeader;
    cuttlefish_msgs::Header* m_receiveHeader;

};
}
}

#endif // SEPIA_COMM_MESSAGECLIENT_H
