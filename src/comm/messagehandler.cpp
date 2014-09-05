#include <sepia/comm/messagehandler.h>
#include <boost/interprocess/ipc/message_queue.hpp>

using boost::interprocess::message_queue;
using boost::interprocess::create_only;
using boost::interprocess::open_only;


namespace sepia
{
namespace comm
{

MessageHandler::MessageHandler( const std::string name )
    : m_queue( NULL ),
      m_name( name ),
      m_removeQueueOnClose( false ),
      m_maxMessages( 100 ),
      m_maxMessageSize( 1000 )
{
}

MessageHandler::~MessageHandler()
{
    if( m_removeQueueOnClose )
    {
        message_queue::remove( m_name.c_str() );
    }
}

void MessageHandler::create()
{
    if( m_queue == NULL )
    {
        message_queue::remove( m_name.c_str() );
        m_removeQueueOnClose = true;
        m_queue = new message_queue( create_only,
                                     m_name.c_str(),
                                     m_maxMessages,
                                     m_maxMessageSize );
    }
}

void MessageHandler::open()
{
    if( m_queue == NULL )
    {
        m_queue = new message_queue( open_only,
                                     m_name.c_str() );
    }
}

void MessageHandler::getMessage( char* a_buffer, size_t& a_receivedBytes )
{
    unsigned int priority = 0;
    m_queue->receive( a_buffer, m_maxMessageSize, a_receivedBytes, priority );
}

void MessageHandler::putMessage( const char* a_buffer, const size_t& a_bytes )
{
    m_queue->send( a_buffer, a_bytes, 0 );
}

size_t MessageHandler::getMaxSize()
{
    return m_maxMessageSize;
}

}
}
