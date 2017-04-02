#ifndef SEPIA_COMM2_DISPATCHER_H
#define SEPIA_COMM2_DISPATCHER_H
#include <sepia/comm2/messagesender.h>
#include <iostream>
#include <google/protobuf/message_lite.h>

namespace sepia
{
namespace comm2
{
   template< typename MessageName, typename Enable = void >
   class Dispatcher : protected MessageSender
   {
      public:
         static void send( const MessageName* a_message, bool a_local = false );
         static void localSend( const MessageName* a_message );
   };

   template< typename MessageName >
   class Dispatcher< MessageName, typename std::enable_if< std::is_base_of< google::protobuf::MessageLite, MessageName >::value >::type > : protected MessageSender
   {
      public:
         static void send( const MessageName* a_message, bool a_local = false )
         {
            std::vector< char > buffer( a_message->ByteSize() );
            a_message->SerializeToArray( buffer.data(), a_message->ByteSize() );

            rawSend( MessageName::default_instance().GetTypeName(), reinterpret_cast< const unsigned char* >( buffer.data() ), buffer.size(), a_local );
         }

         static void localSend( const MessageName* a_message )
         {
             send( a_message, true );
         }
   };

   template< typename MessageName >
   class Dispatcher< MessageName, typename std::enable_if< std::is_base_of< flatbuffers::NativeTable, MessageName >::value >::type > : protected MessageSender
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
   };
}
}

#endif // SEPIA_COMM2_DISPATCHER_H
