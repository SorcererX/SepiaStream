#include "messagerouter.h"
#include <messagehandler.h>
#include <sepia/comm/observer.h>
#include <sepia/comm/dispatcher.h>
#include "processmonitor.h"

MessageRouter::MessageRouter( const std::string a_incomingQueueName )
    : m_incomingQueueName( a_incomingQueueName ),
      m_processMonitor( NULL ),
      m_currentNodeId( 0 ),
      m_routerId( 0 )
{
}

MessageRouter::~MessageRouter()
{
    // do nothing.
}

void MessageRouter::setProcessMonitor( ProcessMonitor* a_processMonitor )
{
    m_processMonitor = a_processMonitor;
}

void MessageRouter::route( const sepia::comm::Header *a_header, const char *a_buffer, size_t a_size )
{
    std::cout << "source: " << a_header->source_node() << " message: " << a_header->message_name() << " ";
    std::cout << "-> ";

    std::cout << "destinations: " << std::flush;
    MessageNameMap::iterator it = m_messageNameToSubscribers.find( a_header->message_name() );
    if( it != m_messageNameToSubscribers.end() )
    {
        for( unsigned int node : it->second )
        {
            MessageHandlerList::iterator mh_it = m_outputMessageQueues.find( node );
            std::cout << std::to_string( node );
            if( mh_it != m_outputMessageQueues.end() )
            {
                mh_it->second->putMessage( a_buffer,
                                           a_size );
            }
            else
            {
               std::cout << "{NO_ROUTE}" << std::flush;
                // no route to node.
            }
            std::cout << "," << std::flush;
        }
    }
    else
    {
       std::cout << "{EMPTY}";
        // no subscribers.
    }
    std::cout << " " << std::endl;
}


void MessageRouter::receive( const sepia::comm::internal::IdNotify* a_message )
{
    // a_message->pid();
    sepia::comm::internal::IdResponse response;
    response.set_pid( a_message->pid() );
    addRoute( a_message->pid(), a_message->queue_name() );

    sepia::comm::MessageSender::selectOutput( m_outputMessageQueues[ a_message->pid() ] );

    sepia::comm::Dispatcher< sepia::comm::internal::IdResponse >::send( &response );
}

void MessageRouter::receive( const sepia::comm::internal::Subscribe* a_message )
{
    for( int i = 0; i < a_message->message_name_size(); i++ )
    {
        MessageNameMap::iterator it = m_messageNameToSubscribers.find( a_message->message_name(i) );
        if( it != m_messageNameToSubscribers.end() )
        {
            it->second.insert( a_message->source_node() );
        }
        else
        {
            std::unordered_set< unsigned int > temp;
            temp.insert( a_message->source_node() );
            m_messageNameToSubscribers[ a_message->message_name(i) ] = temp;
        }
    }
}

void MessageRouter::receive( const sepia::comm::internal::UnSubscribe* a_message )
{
    for( int i = 0; i < a_message->message_name_size(); i++ )
    {
        MessageNameMap::iterator it = m_messageNameToSubscribers.find( a_message->message_name(i) );
        if( it != m_messageNameToSubscribers.end() )
        {
            std::unordered_set< unsigned int >::iterator list_it = it->second.find( a_message->source_node() );
            it->second.erase( list_it );

            if( it->second.size() == 0 )
            {
               m_messageNameToSubscribers.erase( it );
            }
        }
        else
        {
            // not subscribed.
        }
    }
}

void MessageRouter::receive( const sepia::comm::internal::ProcessDied* a_message )
{
    std::cout << "Process died: " << a_message->pid() << " cleaning up..." << std::endl;
    MessageNameMap::iterator it;
    for( it = m_messageNameToSubscribers.begin(); it != m_messageNameToSubscribers.end(); ++it )
    {
        std::unordered_set< unsigned int >::iterator list_it = it->second.find( a_message->pid() );
        it->second.erase( list_it );

        if( it->second.size() == 0 )
        {
            m_messageNameToSubscribers.erase( it );
        }
    }
    removeRoute( a_message->pid() );
}

void MessageRouter::addRoute( const unsigned int a_identifier, const std::string a_queue )
{
    std::cout << "addRoute( " << a_identifier << ", " << a_queue << " ) " << std::endl;
    sepia::comm::MessageHandler* mh = new sepia::comm::MessageHandler( a_queue );
    m_outputMessageQueues[ a_identifier ] = mh;
    mh->open();
    if( m_processMonitor )
    {
        m_processMonitor->add( a_identifier );
    }
}

void MessageRouter::removeRoute( const unsigned int a_identifier )
{
   std::cout << "removeRoute( " << a_identifier  << " ) " << std::endl;
    MessageHandlerList::iterator it;
    it = m_outputMessageQueues.find( a_identifier );

    if( it != m_outputMessageQueues.end() )
    {
        delete it->second;
        m_outputMessageQueues.erase( it );
    }
    if( m_processMonitor )
    {
        m_processMonitor->remove( a_identifier );
    }
}

