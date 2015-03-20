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

#include <sepia/comm/observer.h>
#include <sepia/comm/dispatcher.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/thread/mutex.hpp>
#include <sepia/comm/globalreceiver.h>
#include "internal.pb.h"
#include "header.pb.h"

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

   int gettid()
   {
       return syscall( SYS_gettid );
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
boost::shared_ptr< boost::mutex > ObserverBase::sm_globalMutex;
std::unordered_set< unsigned int > ObserverBase::sm_routerThreads;
thread_local ObserverBase* ObserverBase::stm_router = NULL;

ObserverBase::ObserverBase()
{
    if( !stm_ownData )
    {
        ThreadMap::iterator it = sm_threadData.find( gettid() );
        if( it == sm_threadData.end() )
        {
            ThreadMessageData tmp;
            sm_threadData[ gettid() ] = tmp;
        }
        stm_ownData = &sm_threadData[ gettid() ];
        //stm_ownData->mutex = boost::shared_ptr< boost::mutex >( new boost::mutex );
        //stm_ownData->cond = boost::shared_ptr< boost::condition_variable >( new boost::condition_variable );
        stm_ownData->barrier = boost::shared_ptr< boost::barrier > ( new boost::barrier( 2 ) );
        stm_ownData->mutex = boost::shared_ptr< boost::mutex >( new boost::mutex() );
        stm_ownData->buffer.reserve( 1024 );
        stm_ownData->buffer.resize( 1024 );
    }
}

void ObserverBase::addObserver( const std::string a_name, ObserverBase* a_Observer )
{
   stm_observers[ a_name ] = a_Observer;

   MessageNameMap::iterator it = sm_messageNameToThread.find( a_name );

   unsigned int thread_id = gettid();

   if( it != sm_messageNameToThread.end() )
   {
       it->second.insert( thread_id );
   }
   else
   {
       std::unordered_set< unsigned int > temp;
       temp.insert( thread_id );
       sm_messageNameToThread[ a_name ] = temp;
       if( !GlobalReceiver::isRouter() )
       {
            subscribe( a_name );
       }
   }
}

void ObserverBase::addRouter( ObserverBase* a_Observer )
{
    sm_routerThreads.insert( gettid() );
    if( !stm_router )
    {
        stm_router = a_Observer;
    }
}


void ObserverBase::removeObserver( const std::string a_name )
{
    ObserverMap::iterator it = stm_observers.find( a_name );
    if( !GlobalReceiver::isRouter() )
    {
       unsubscribe( a_name );
    }
    stm_observers.erase( it );
}

bool ObserverBase::routeMessageToThreads( const Header* a_header, char* a_buffer, const size_t a_size )
{
    MessageNameMap::iterator it = sm_messageNameToThread.find( a_header->message_name() );

    if( it != sm_messageNameToThread.end() )
    {
        for( unsigned int node : it->second )
        {
            routeToNode( node, a_header, a_buffer, a_size );
        }
    }
    else
    {
        for( unsigned int node : sm_routerThreads )
        {
            routeToNode( node, a_header, a_buffer, a_size );
        }
    }
    return true;
}

bool ObserverBase::routeToNode( unsigned int a_node, const Header* a_header, char* a_buffer, const size_t a_size )
{
    ThreadMap::iterator thread_it = sm_threadData.find( a_node );

    if( thread_it != sm_threadData.end() )
    {
        ThreadMessageData* thr = &thread_it->second;
        thr->mutex->lock();
        //boost::unique_lock< boost::mutex > lock( *thr->mutex );
        thr->barrier->wait(); // routeToNode critical section start

        memcpy( thr->buffer.data(), a_buffer, a_size );
        thr->header = *a_header;
        thr->length = a_size;

        thr->barrier->wait(); // routeToNode critical section end
        thr->barrier->wait(); // ThreadReceiver critical section end
        thr->mutex->unlock();
        //thr->cond->notify_all();
        return true;
    }
    return false;
}

bool ObserverBase::handleReceive( const Header* a_header, const char* a_buffer, const size_t a_size )
{
    ObserverMap::iterator it = stm_observers.find( a_header->message_name() );
    if( it != stm_observers.end() )
    {
        it->second->process( NULL, a_buffer, a_size );
        return true;
    }
    else
    {
        if( stm_router )
        {
            stm_router->process( a_header, GlobalReceiver::getLastMessageBuffer(), GlobalReceiver::getLastMessageBufferSize() );
            return true;
        }
        return false;
    }
}


bool ObserverBase::threadReceiver()
{
    if( !stm_ownData )
    {
        std::cerr << "threadReceiver() - no observer(s) attached to this thread!" << std::endl;
        return false;
    }
    stm_ownData->barrier->wait(); // routeToNode critical section start
    stm_ownData->barrier->wait(); // routeToNode critical section end
    //boost::unique_lock< boost::mutex > lock( *stm_ownData->mutex );
    //stm_ownData->cond->wait( lock );
    bool retval = handleReceive( &stm_ownData->header, stm_ownData->buffer.data(), stm_ownData->length );

    // TODO: temporary workaround (essentially disables ability to handle messages in parallel with several threads.
    //       This means only one thread is able to do processing at once.
    stm_ownData->barrier->wait(); // threadReceiver critical section end
    return retval;
}

}
}
