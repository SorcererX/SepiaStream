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
            std::cout << "Dispatching: " << a_message->GetTypeName() << " " << a_message->ShortDebugString() << std::endl;
            // reserve space for size of header.
            size_t send_bytes = sm_header->ByteSize();
            size_t accum_bytes = sizeof( size_t );

            // write header size to start of buffer.
            memcpy( sm_buffer.data(), &send_bytes, sizeof( size_t ) );

            sm_header->SerializeToArray( sm_buffer.data() + accum_bytes, sm_header->ByteSize() );
            accum_bytes += send_bytes;

            send_bytes = a_message->ByteSize();

            a_message->SerializeToArray( sm_buffer.data() + accum_bytes, send_bytes );
            accum_bytes += send_bytes;
            sm_messageHandler->putMessage( sm_buffer.data(), accum_bytes );
         }

         static void localSend( const MessageName* a_message )
         {
             Header header;
             header.set_message_name( a_message->GetTypeName() );
             std::vector< char > msg_buffer;
             msg_buffer.resize( a_message->ByteSize() );
             a_message->SerializeToArray( msg_buffer.data(), msg_buffer.size() );
             bool handled = ObserverBase::routeMessageToThreads( &header, msg_buffer.data(), msg_buffer.size() );
         }

      private:

   };
}
}
#endif // SEPIA_COMM_DISPATCHER_H
