#ifndef SEPIA_COMM2_DISPATCHER_H
#define SEPIA_COMM2_DISPATCHER_H
#include <sepia/comm2/messagesender.h>
#include <iostream>

namespace sepia
{
namespace comm2
{
   template< class MessageName >
   class Dispatcher : protected MessageSender
   {
      public:
         static void send( const MessageName* a_message, bool a_local = false )
         {
            flatbuffers::FlatBufferBuilder fbb;
            auto offset = MessageName::TableType::Pack( fbb, a_message );

            fbb.Finish( offset );

            rawSend( MessageName::GetFullyQualifiedName(), fbb.GetBufferPointer(), fbb.GetSize(), a_local );
         }

         static void localSend( const MessageName* a_message )
         {
             send( a_message, true );
         }


      private:
   };
}
}
#endif // SEPIA_COMM2_DISPATCHER_H
