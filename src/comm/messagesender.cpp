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

#include <sepia/comm/messagesender.h>
#include "header.pb.h"
#include "internal.pb.h"
#include <sepia/comm/messagehandler.h>
#include <google/protobuf/message_lite.h>
#include <sepia/comm/observerbase.h>

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
        sm_messageHandler = new MessageHandler( sepia::comm::ObserverBase::commName() + std::string( "_outgoing" ) );
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

void MessageSender::rawSend( char* a_header, size_t a_headerSize, char* a_msg, size_t a_msgSize )
{
    char* offset = sm_buffer.data();
    memcpy( offset, reinterpret_cast< char* >( &a_headerSize ), sizeof( size_t ) );
    offset += sizeof( size_t );
    memcpy( offset, a_msg, a_msgSize );
    offset += a_msgSize;
    sm_messageHandler->putMessage( sm_buffer.data(), offset - sm_buffer.data() );
}

void MessageSender::rawSend( const google::protobuf::MessageLite* a_header, const google::protobuf::MessageLite* a_msg )
{
    size_t size = static_cast< size_t >( a_header->ByteSize() );
    char* offset = sm_buffer.data();
    memcpy( offset, reinterpret_cast< char* >( &size ), sizeof( size_t ) );
    offset += sizeof( size_t );
    a_header->SerializeToArray( offset, size );
    offset += size;
    size = a_msg->ByteSize();
    a_msg->SerializeToArray( offset, size );
    offset += size;
    sm_messageHandler->putMessage( sm_buffer.data(), offset - sm_buffer.data() );
}

}
}
