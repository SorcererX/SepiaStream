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
            const std::size_t bytesize = a_message->ByteSizeLong();

            if( bytesize > stm_buffer.size() )
            {
                stm_buffer.resize( bytesize * 1.5 );
            }
            a_message->SerializeToArray( stm_buffer.data(), bytesize );

            rawSend( MessageName::default_instance().GetTypeName(), reinterpret_cast< const unsigned char* >( stm_buffer.data() ), bytesize, a_local );
         }

         static void localSend( const MessageName* a_message )
         {
             send( a_message, true );
         }
   private:
         static thread_local std::vector< char > stm_buffer;
   };

   template< typename MessageName >
   thread_local std::vector< char > Dispatcher< MessageName, typename std::enable_if< std::is_base_of< google::protobuf::MessageLite, MessageName >::value >::type >::stm_buffer;

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
