#ifndef SEPIA_COMM2_OBSERVER_H
#define SEPIA_COMM2_OBSERVER_H
#include "observerbase.h"
#include <iostream>
#include <flatbuffers/flatbuffers.h>
#include <google/protobuf/message_lite.h>

namespace sepia
{
    namespace comm2
    {
        template< class MessageName, typename Enable = void >
class Observer : public ObserverBase
{
public:
    ~Observer();
protected:
    Observer();
    void initReceiver();
    void destroyReceiver();
    void process( const char* a_buffer );
    virtual void receive( const std::unique_ptr< MessageName >& msg ) = 0;
};

        template< typename MessageName >
        class Observer< MessageName, typename std::enable_if< std::is_base_of< google::protobuf::MessageLite, MessageName >::value >::type > : public ObserverBase
        {
            public:
                virtual ~Observer()
                {
                    if( m_destroyAtDestruction )
                    {
                        ObserverBase::removeObserver( MessageName::default_instance().GetTypeName(), this );
                    }
                }

            protected:
                Observer() : m_msg( std::unique_ptr< MessageName >( new MessageName ) ) , m_destroyAtDestruction( false ) {}
                Observer( unsigned int a_id ) : m_msg( std::unique_ptr< MessageName >( new MessageName ) ), m_destroyAtDestruction( true )
                {
                    ObserverBase::addObserver( MessageName::default_instance().GetTypeName(), this, a_id );
                }

                void initReceiver( unsigned int a_id = 0 ) // 0 is all ids.
                {
                    ObserverBase::addObserver( MessageName::default_instance().GetTypeName(), this, a_id );
                }

                void destroyReceiver()
                {
                    ObserverBase::removeObserver( MessageName::default_instance().GetTypeName(), this );
                }

                virtual void process( const char* a_buffer, size_t a_size, unsigned int a_id )
                {
                    m_msg->ParseFromArray( a_buffer, static_cast< int >( a_size ) );
                    receive( *m_msg, a_id );
                    receive( *m_msg );
                }

                virtual void receive( const MessageName& ) {}
                virtual void receive( const MessageName&, unsigned int ) {}
            private:
                std::unique_ptr< MessageName > m_msg;
                bool m_destroyAtDestruction;
        };
template< typename MessageName >
class Observer< MessageName, typename std::enable_if< std::is_base_of< flatbuffers::NativeTable, MessageName >::value >::type > : public ObserverBase
{
public:
    ~Observer() {}

protected:
    Observer() {}

    void initReceiver( unsigned int a_id = 0 )
    {
        ObserverBase::addObserver( MessageName::GetFullyQualifiedName(), this );
    }

    void destroyReceiver()
    {
        ObserverBase::removeObserver( MessageName::GetFullyQualifiedName(), this );
    }

    void process( const char* a_buffer, size_t a_size, unsigned int a_id )
    {
        std::unique_ptr< MessageName > message = std::unique_ptr< MessageName >( flatbuffers::GetRoot< typename MessageName::TableType >( a_buffer )->UnPack(nullptr) );
        receive( *message, a_id );
        receive( *message );
    }

    virtual void receive( const MessageName& msg ) {};
    virtual void receive( const MessageName& msg, unsigned int ) {};
};
    }
}
#endif // SEPIA_COMM2_OBSERVER_H

