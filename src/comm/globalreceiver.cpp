/*
Copyright (c) 2012-2015, Kai Hugo Hustoft Endresen <kai.endresen@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <sepia/comm/globalreceiver.h>
#include <sepia/comm/messagehandler.h>
#include <sepia/comm/dispatcher.h>
#include "internal.pb.h"
#include "header.pb.h"
#include <sepia/comm/observer.h>

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

void GlobalReceiver::stop()
{
    sepia::util::ThreadObject::stop();

    if( sm_messageHandler )
    {
        sm_messageHandler->close();
    }
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

void GlobalReceiver::receiveData( char** a_buffer, size_t& a_messageSize )
{
    a_messageSize = 0;

    bool got_msg = sm_messageHandler->getMessage( sm_buffer.data(), a_messageSize );

    if( got_msg )
    {
        sm_lastBufferSize = a_messageSize;
        size_t header_size = *((size_t*) sm_buffer.data());

        size_t offset = sizeof( size_t );

        sm_header.ParseFromArray( sm_buffer.data() + offset, header_size );

        // calculate size of remaining message.
        a_messageSize -= ( sizeof( size_t) + header_size );

        *a_buffer = sm_buffer.data() + offset + header_size;
    }
    else
    {
        a_messageSize = 0;
        *a_buffer = NULL;
    }
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
    while( !m_terminate )
    {
        size_t messageSize = 0;
        char* msg_ptr = NULL;
        receiveData( &msg_ptr, messageSize );
        if( msg_ptr )
        {
            bool handled = ObserverBase::routeMessageToThreads( getLastHeader(), msg_ptr, messageSize );
        }
    }
}
}
}
