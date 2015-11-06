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

#include <sepia/comm/observerbase.h>
#include <sepia/comm/dispatcher.h>
#include <unistd.h>
#include <sepia/comm/globalreceiver.h>
#include "internal.pb.h"
#include "header.pb.h"
#include <iostream>

namespace {
   void subscribe( const std::string a_messageName )
   {
      sepia::comm::internal::Subscribe msg;
      msg.add_message_name( a_messageName );
      msg.set_source_node( getpid() );
      msg.set_source_router( 0 );
      sepia::comm::Dispatcher< sepia::comm::internal::Subscribe >::send( &msg );
   }

   void unsubscribe( const std::string a_messageName )
   {
      sepia::comm::internal::UnSubscribe msg;
      msg.add_message_name( a_messageName );
      msg.set_source_node( getpid() );
      msg.set_source_router( 0 );
      sepia::comm::Dispatcher< sepia::comm::internal::UnSubscribe >::send( &msg );
   }
}

namespace sepia
{
namespace comm
{

thread_local ObserverBase::ObserverMap ObserverBase::stm_observers;

ObserverBase::ThreadMap ObserverBase::sm_threadData;
ObserverBase::MessageNameMap ObserverBase::sm_messageNameToThread;
thread_local ObserverBase::ThreadMessageData* ObserverBase::stm_ownData = NULL;
std::shared_ptr< std::mutex > ObserverBase::sm_globalMutex = std::shared_ptr< std::mutex >( new std::mutex() );
bool ObserverBase::sm_debugEnabled = false;
bool ObserverBase::sm_gotIdResponse = false;
thread_local ObserverBase* ObserverBase::stm_router = NULL;
std::list< std::string > ObserverBase::sm_subscriptionList;
std::string ObserverBase::sm_commName;

ObserverBase::ObserverBase()
{
}

void ObserverBase::enableDebug( bool a_enable )
{
    sm_debugEnabled = a_enable;
}

bool ObserverBase::debugEnabled()
{
    return sm_debugEnabled;
}

std::string ObserverBase::commName()
{
    if( sm_commName.size() == 0 )
    {
        char* name = getenv( "SEPIACOMM" );
        if( name )
        {
            sm_commName = name;
        }
        else
        {
            sm_commName = "cuttlefish";
        }
    }
    return sm_commName;
}

void ObserverBase::stopThreadReceiver( std::thread::id a_threadId )
{
    ThreadMap::iterator it = sm_threadData.find( a_threadId );

    if( it != sm_threadData.end() )
    {
        it->second.terminateReceiver = true;
        it->second.cond->notify_all();
    }
}

void ObserverBase::initReceiver()
{
    if( !stm_ownData )
    {
        ThreadMap::iterator it = sm_threadData.find( std::this_thread::get_id() );
        if( it == sm_threadData.end() )
        {
            ThreadMessageData tmp;
            sm_threadData[ std::this_thread::get_id() ] = tmp;
        }
        stm_ownData = &sm_threadData[ std::this_thread::get_id() ];
        stm_ownData->cond = std::shared_ptr< std::condition_variable >( new std::condition_variable );
        stm_ownData->mutex = std::shared_ptr< std::mutex >( new std::mutex() );
        stm_ownData->buffer.reserve( 1024 );
        stm_ownData->buffer.resize( 1024 );
        stm_ownData->data_ready = false;
    }
    stm_ownData->terminateReceiver = false;
}

void ObserverBase::addObserver( const std::string a_name, ObserverBase* a_Observer )
{
   stm_observers[ a_name ] = a_Observer;

   MessageNameMap::iterator it = sm_messageNameToThread.find( a_name );

   std::thread::id thread_id = std::this_thread::get_id();

   if( it != sm_messageNameToThread.end() )
   {
       it->second.insert( thread_id );
   }
   else
   {
       std::unordered_set< std::thread::id > temp;
       temp.insert( thread_id );
       sm_messageNameToThread[ a_name ] = temp;
       if( !GlobalReceiver::isRouter() )
       {
           if( sm_gotIdResponse )
           {
               subscribe( a_name );
           }
            sm_subscriptionList.push_back( a_name );
       }
   }
}

void ObserverBase::removeObserver( const std::string a_name )
{
    MessageNameMap::iterator message_it = sm_messageNameToThread.find( a_name );

    if( message_it != sm_messageNameToThread.end() )
    {
        std::unordered_set< std::thread::id >::const_iterator thread_it = message_it->second.find( std::this_thread::get_id() );

        if( thread_it != message_it->second.end() )
        {
            message_it->second.erase( thread_it );

            ObserverMap::iterator observer_it = stm_observers.find( a_name );

            if( observer_it != stm_observers.end() )
            {
                stm_observers.erase( observer_it );
            }

            if( message_it->second.size() == 0 )
            {
                sm_messageNameToThread.erase( message_it );
                if( !GlobalReceiver::isRouter() )
                {
                    unsubscribe( a_name );
                    sm_subscriptionList.remove( a_name );
                }
            }
        }
        else
        {
            std::cout << "ObserverBase::removeObserver - attempted to remove message not assigned to current thread." << std::endl;
        }
    }
}

bool ObserverBase::resendAllSubscriptions()
{
    sm_gotIdResponse = true;

    sm_globalMutex->lock();
    for( const std::string msg_name : sm_subscriptionList )
    {
        subscribe( msg_name );
    }
    sm_globalMutex->unlock();
}

bool ObserverBase::routeMessageToThreads( const Header* a_header, char* a_buffer, const size_t a_size )
{
    MessageNameMap::iterator it = sm_messageNameToThread.find( a_header->message_name() );

    if( it != sm_messageNameToThread.end() )
    {
        for( std::thread::id node : it->second )
        {
            routeToNode( node, a_header, a_buffer, a_size );
        }
        return true;
    }
    return false;
}

bool ObserverBase::routeToNode( std::thread::id a_node, const Header* a_header, char* a_buffer, const size_t a_size )
{
    ThreadMap::iterator thread_it = sm_threadData.find( a_node );

    if( thread_it != sm_threadData.end() )
    {
        ThreadMessageData* thr = &thread_it->second;
        {
            std::unique_lock< std::mutex > lock( *thr->mutex );
            while( thr->data_ready && !thr->terminateReceiver )
            {
                thr->cond->wait( lock );
            }

            if( thr->terminateReceiver )
            {
                return false;
            }

            memcpy( thr->buffer.data(), a_buffer, a_size );
            thr->header = *a_header;
            thr->length = a_size;
            thr->data_ready = true;
        }
        thr->cond->notify_one();
        return true;
    }

    return false;
}

bool ObserverBase::handleReceive( const Header* a_header, const char* a_buffer, const size_t a_size )
{
    ObserverMap::iterator it = stm_observers.find( a_header->message_name() );
    if( it != stm_observers.end() )
    {
        it->second->process( a_header, a_buffer, a_size );
        return true;
    }
    return false;
}


bool ObserverBase::threadReceiver()
{
    if( !stm_ownData )
    {
        std::cerr << "threadReceiver() - no observer(s) attached to this thread!" << std::endl;
        return false;
    }
    std::unique_lock< std::mutex > lock( *stm_ownData->mutex );
    while( !stm_ownData->data_ready && !stm_ownData->terminateReceiver )
    {
        stm_ownData->cond->wait( lock );
    }

    if( stm_ownData->terminateReceiver )
    {
        return false;
    }

    stm_ownData->data_ready = false;
    bool retval = handleReceive( &stm_ownData->header, stm_ownData->buffer.data(), stm_ownData->length );
    stm_ownData->cond->notify_all();
    return retval;
}

}
}
