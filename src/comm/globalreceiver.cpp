#include <sepia/comm/globalreceiver.h>
#include <sepia/comm/messagehandler.h>
#include <sepia/comm/dispatcher.h>
#include "internal.pb.h"
#include "header.pb.h"
#include <sepia/comm/observer.h>
#include <boost/thread/thread.hpp>


namespace sepia
{
namespace comm
{
MessageHandler* GlobalReceiver::sm_messageHandler = NULL;
Header GlobalReceiver::sm_header;
std::vector< char > GlobalReceiver::sm_buffer;
bool GlobalReceiver::sm_isRouter = false;
size_t GlobalReceiver::sm_lastBufferSize = 0;


namespace
{
    void id_notify( const std::string a_name )
    {
        sepia::comm::internal::IdNotify msg;
        msg.set_pid( getpid() );
        msg.set_queue_name( std::string( "cuttlefish_incoming_" ) + std::to_string( getpid() ) );
        msg.set_node_name( a_name );
        Dispatcher< sepia::comm::internal::IdNotify >::send( &msg );
    }

}

GlobalReceiver::GlobalReceiver()
{
}

bool GlobalReceiver::isRouter()
{
    return sm_isRouter;
}

void GlobalReceiver::start()
{
    m_thread = new std::thread( std::bind( &GlobalReceiver::own_thread, this ) );
}

void GlobalReceiver::initClient( const std::string& a_name )
{
    sm_isRouter = false;
   if( !sm_messageHandler )
   {
      sm_messageHandler = new MessageHandler( std::string( "cuttlefish_incoming_" ) + std::to_string( getpid() ) );
      sm_messageHandler->create();
   }

   if( sm_buffer.size() == 0 )
   {
       sm_buffer.reserve( sm_messageHandler->getMaxSize() );
       sm_buffer.resize( sm_messageHandler->getMaxSize() );
       id_notify( a_name );
   }

}

void GlobalReceiver::initRouter()
{
    sm_isRouter = true;
    if( !sm_messageHandler )
    {
        sm_messageHandler = new MessageHandler( "cuttlefish_outgoing" );
        sm_messageHandler->create();
    }

    if( sm_buffer.size() == 0 )
    {
        sm_buffer.reserve( sm_messageHandler->getMaxSize() );
        sm_buffer.resize( sm_messageHandler->getMaxSize() );
    }
}

void GlobalReceiver::receiveData( char** a_buffer, size_t& remaining_bytes )
{
    remaining_bytes = 0;

    sm_messageHandler->getMessage( sm_buffer.data(), remaining_bytes );
    sm_lastBufferSize = remaining_bytes;
    size_t header_size = *((size_t*) sm_buffer.data());

    size_t offset = sizeof( size_t );

    sm_header.ParseFromArray( sm_buffer.data() + offset, header_size );

    // calculate size of remaining message.
    remaining_bytes -= ( sizeof( size_t) + header_size );

    *a_buffer = sm_buffer.data() + offset + header_size;
}

const Header* GlobalReceiver::getLastHeader()
{
    return &sm_header;
}

const char* GlobalReceiver::getLastMessageBuffer()
{
   return sm_buffer.data();
}

size_t GlobalReceiver::getLastMessageBufferSize()
{
   return sm_lastBufferSize;
}


void GlobalReceiver::own_thread()
{
    while( true )
    {
        size_t remaining_bytes = 0;
        char* msg_ptr = NULL;
        receiveData( &msg_ptr, remaining_bytes );
        bool handled = ObserverBase::routeMessageToThreads( getLastHeader(), msg_ptr, remaining_bytes );
    }
}
}
}
