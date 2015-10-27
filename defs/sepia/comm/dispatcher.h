#ifndef SEPIA_COMM_DISPATCHER_H
#define SEPIA_COMM_DISPATCHER_H
#include <sepia/comm/messagesender.h>
#include <sepia/comm/messagehandler.h>
#include "header.pb.h"
#include <sepia/comm/observer.h>

namespace sepia
{
namespace comm
{
   template< class MessageName >
   class Dispatcher : protected MessageSender
   {
      public:
         static void send( const MessageName* a_message )
         {
            sm_header->set_message_name( a_message->GetTypeName() );
            if( ObserverBase::debugEnabled() )
            {
                std::cout << "Dispatching: " << a_message->GetTypeName() << " " << a_message->ShortDebugString() << std::endl;
            }
            rawSend( sm_header, a_message );
         }

         static void localSend( const MessageName* a_message )
         {
             Header header;
             header.set_message_name( a_message->GetTypeName() );
             std::vector< char > msg_buffer;
             msg_buffer.resize( a_message->ByteSize() );
             a_message->SerializeToArray( msg_buffer.data(), msg_buffer.size() );
             bool handled = ObserverBase::routeMessageToThreads( &header, msg_buffer.data(), msg_buffer.size() );
             if( !handled )
             {
                 std::cout << "Dispatcher::localSend - not handled." << std::endl;
             }
         }

      private:
   };
}
}
#endif // SEPIA_COMM_DISPATCHER_H
