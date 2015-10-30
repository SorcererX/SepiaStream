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
    sepia::comm::Observer< sepia::comm::internal::IdNotify >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::Subscribe >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::UnSubscribe >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::ProcessDied >::initReceiver();
}

MessageRouter::~MessageRouter()
{
    // do nothing.
}

void MessageRouter::setProcessMonitor( ProcessMonitor* a_processMonitor )
{
    m_processMonitor = a_processMonitor;
}

void MessageRouter::receiveRaw( const sepia::comm::Header *a_header, const char *a_buffer, size_t a_size )
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
    bool retval = addRoute( a_message->pid(), a_message->queue_name() );

    if( retval )
    {
        sepia::comm::MessageSender::selectOutput( m_outputMessageQueues[ a_message->pid() ] );
        sepia::comm::Dispatcher< sepia::comm::internal::IdResponse >::send( &response );
    }
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
            ObserverRaw::initRawReceiver( a_message->message_name(i) );
            std::unordered_set< unsigned int > temp;
            temp.insert( a_message->source_node() );
            m_messageNameToSubscribers[ a_message->message_name(i) ] = temp;
            // Forward subscription
            sepia::comm::internal::ForwardSubscribe fwd_msg;
            fwd_msg.set_message_name( a_message->message_name(i) );
            routeMessage( &fwd_msg );
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

            if( list_it != it->second.end() )
            {
                it->second.erase( list_it );
            }

            if( it->second.size() == 0 )
            {
                ObserverRaw::destroyRawReceiver( a_message->message_name(i) );
                m_messageNameToSubscribers.erase( it );

                // Forward unsubscription
                sepia::comm::internal::ForwardUnsubscribe fwd_msg;
                fwd_msg.set_message_name( a_message->message_name(i) );
                routeMessage( &fwd_msg );
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

        if( list_it != it->second.end() )
        {
            it->second.erase( list_it );
        }

        if( it->second.size() == 0 )
        {
            m_messageNameToSubscribers.erase( it );
        }
    }
    removeRoute( a_message->pid() );
}

bool MessageRouter::addRoute( const unsigned int a_identifier, const std::string a_queue )
{
    if( m_outputMessageQueues.find( a_identifier ) != m_outputMessageQueues.end() )
    {
        // already added route for this.
        return false;
    }

    std::cout << "addRoute( " << a_identifier << ", " << a_queue << " ) " << std::endl;
    sepia::comm::MessageHandler* mh = new sepia::comm::MessageHandler( a_queue );
    m_outputMessageQueues[ a_identifier ] = mh;
    mh->open();
    if( m_processMonitor )
    {
        m_processMonitor->add( a_identifier );
    }
    return true;
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

template< class MessageName >
void MessageRouter::routeMessage( MessageName* msg )
{
    MessageNameMap::iterator it = m_messageNameToSubscribers.find( msg->GetTypeName() );
    if( it != m_messageNameToSubscribers.end() )
    {
        for( unsigned int node : it->second )
        {
            MessageHandlerList::iterator mh_it = m_outputMessageQueues.find( node );
            if( mh_it != m_outputMessageQueues.end() )
            {
                sepia::comm::MessageSender::selectOutput( mh_it->second );
                sepia::comm::Dispatcher< MessageName >::send( msg );
            }
        }
    }
}


