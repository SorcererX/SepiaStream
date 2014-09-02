#include <sepia/comm/messagesender.h>
#include "header.pb.h"
#include "internal.pb.h"
#include <sepia/comm/messagehandler.h>

namespace sepia
{
namespace comm
{

MessageHandler* MessageSender::sm_messageHandler = NULL;
Header* MessageSender::sm_header = NULL;
std::vector< char > MessageSender::sm_buffer;


void MessageSender::initClient()
{
    if( !sm_messageHandler )
    {
        sm_messageHandler = new MessageHandler( "cuttlefish_outgoing" );
        sm_messageHandler->open();
    }

    if( sm_buffer.size() == 0 )
    {
        sm_buffer.reserve( sm_messageHandler->getMaxSize() );
        sm_buffer.resize( sm_messageHandler->getMaxSize() );
    }

    if( !sm_header )
    {
        sm_header = new Header();
        sm_header->set_source_node( getpid() );
        sm_header->set_source_router( 0 );
    }
}

void MessageSender::selectOutput( MessageHandler* a_handler )
{
    sm_messageHandler = a_handler;

    if( sm_buffer.size() < sm_messageHandler->getMaxSize() )
    {
        sm_buffer.reserve( sm_messageHandler->getMaxSize() );
        sm_buffer.resize(  sm_messageHandler->getMaxSize() );
    }

    if( !sm_header )
    {
        sm_header = new Header();
        sm_header->set_source_node( 0 );
        sm_header->set_source_router( 0 );
    }
}
}
}
