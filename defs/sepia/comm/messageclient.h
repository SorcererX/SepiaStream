#ifndef SEPIA_COMM_MESSAGECLIENT_H
#define SEPIA_COMM_MESSAGECLIENT_H
#include <vector>
#include <string>

namespace google {
   namespace protobuf {
      class MessageLite;
   }
}

namespace sepia
{
namespace comm
{
class MessageHandler;
class Header;

class MessageClient
{
public:
    MessageClient( const std::string a_queue );
    void sendMessage( google::protobuf::MessageLite* a_message );
    void receiveMessage( sepia::comm::Header* a_header, google::protobuf::MessageLite* a_message );

protected:
    void sendIdRequest();
    void getIdResponse();
    void init( const std::string a_queueName );

private:
    sepia::comm::MessageHandler* m_incomingQueue;
    sepia::comm::MessageHandler* m_outgoingQueue;
    std::vector< char > m_sendBuffer;
    std::vector< char > m_receiveBuffer;
    std::string m_incomingQueueName;
    sepia::comm::Header* m_sendHeader;
    sepia::comm::Header* m_receiveHeader;

};
}
}

#endif // SEPIA_COMM_MESSAGECLIENT_H
