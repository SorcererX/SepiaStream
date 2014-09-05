#ifndef SEPIA_COMM_MESSAGEHANDLER_H
#define SEPIA_COMM_MESSAGEHANDLER_H
#include <string>
#include <boost/interprocess/interprocess_fwd.hpp>

namespace sepia
{
namespace comm
{

class MessageHandler
{
public:
    MessageHandler( const std::string a_name );
    ~MessageHandler();
    void create();
    void open();
    void getMessage( char* a_buffer, size_t& a_receivedBytes );
    void putMessage( const char* a_buffer, const size_t& a_bytes );
    size_t getMaxSize();

private:
    boost::interprocess::message_queue* m_queue;
    std::string m_name;
    bool m_removeQueueOnClose;
    size_t m_maxMessages;
    size_t m_maxMessageSize;
};

}
}

#endif // SEPIA_COMM_MESSAGEHANDLER_H
