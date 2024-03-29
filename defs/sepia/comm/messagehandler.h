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
    void create_or_open();
    void open();
    void close();
    bool getMessage( char* a_buffer, std::size_t& a_receivedBytes );
    void putMessage( const char* a_buffer, const std::size_t& a_bytes );
    std::size_t getMaxSize();

private:
    boost::interprocess::message_queue* m_queue;
    std::string m_name;
    bool m_removeQueueOnClose;
    std::size_t m_maxMessages;
    std::size_t m_maxMessageSize;
};

}
}

#endif // SEPIA_COMM_MESSAGEHANDLER_H
