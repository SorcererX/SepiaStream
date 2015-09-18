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

#include <sepia/comm/messagehandler.h>
#include <boost/interprocess/ipc/message_queue.hpp>

using boost::interprocess::message_queue;
using boost::interprocess::create_only;
using boost::interprocess::open_only;
using boost::interprocess::open_or_create;


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

void MessageHandler::close()
{
    message_queue::remove( m_name.c_str() );
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

void MessageHandler::create_or_open()
{
    if( m_queue == NULL )
    {
        m_removeQueueOnClose = false;
        m_queue = new message_queue( open_or_create,
                                     m_name.c_str(),
                                     m_maxMessages,
                                     m_maxMessageSize );
    }
}

void MessageHandler::open()
{
    while( m_queue == NULL )
    {
        m_queue = new message_queue( open_only,
                                     m_name.c_str() );
        if( !m_queue )
        {
            usleep( 500000 ); // if queue does not exist, wait 500 ms
        }
    }
}

bool MessageHandler::getMessage( char* a_buffer, size_t& a_receivedBytes )
{
    unsigned int priority = 0;
    boost::posix_time::time_duration delay = boost::posix_time::seconds( 1 );
    boost::posix_time::ptime abs_time = boost::posix_time::microsec_clock::universal_time();

    abs_time += delay;

    return m_queue->timed_receive( a_buffer, m_maxMessageSize, a_receivedBytes, priority, abs_time );
    //m_queue->receive( a_buffer, m_maxMessageSize, a_receivedBytes, priority );
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
