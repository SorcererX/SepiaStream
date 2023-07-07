#ifndef SEPIA_COMM2_REQUESTOR_H
#define SEPIA_COMM2_REQUESTOR_H
#include <zmq.hpp>
#include <sepia/comm2/messagesender.h>

namespace sepia
{
namespace comm2
{
    template< typename MessageName, typename Enable = void >
    class Requestor : protected MessageSender
    {
    public:
      void send( const MessageName* a_message );
      virtual void handleReply( const MessageName* a_message );
    };

    template< typename MessageName >
    class Requestor< MessageName, typename std::enable_if< std::is_base_of< google::protobuf::MessageLite, MessageName >::value >::type > : protected MessageSender
    {
    public:
        void send( const MessageName* a_message )
        {
           const std::size_t bytesize = a_message->ByteSize();

           if( bytesize > m_buffer.size() )
           {
               m_buffer.resize( bytesize * 1.5 );
           }
           a_message->SerializeToArray( m_buffer.data(), bytesize );

           rawRequestSend( MessageName::default_instance().GetTypeName(), reinterpret_cast< const unsigned char* >( m_buffer.data() ), bytesize );
        }
        virtual void handleReply( const MessageName* a_message );
    private:
          std::vector< char > m_buffer;
    };

    template< typename MessageName >
    class Requestor< MessageName, typename std::enable_if< std::is_base_of< flatbuffers::NativeTable, MessageName >::value >::type > : protected MessageSender
    {
        void send( const MessageName* a_message )
        {
           flatbuffers::FlatBufferBuilder fbb;
           auto offset = MessageName::TableType::Pack( fbb, a_message );
           fbb.Finish( offset );

           rawRequestSend( MessageName::GetFullyQualifiedName(), fbb.GetBufferPointer(), fbb.GetSize() );
        }
        virtual void handleReply( const MessageName* a_message );
    };


}

}

#endif // SEPIA_COMM2_REQUESTOR_H
