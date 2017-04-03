#ifndef SEPIA_COMM2_OBSERVER_H
#define SEPIA_COMM2_OBSERVER_H
#include <sepia/comm2/observerbase.h>
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
    ~Observer() {}

protected:
    Observer() : m_msg( std::unique_ptr< MessageName >( new MessageName ) ) { }

    void initReceiver()
    {
        ObserverBase::addObserver( MessageName::default_instance().GetTypeName(), this );
    }

    void destroyReceiver()
    {
        ObserverBase::removeObserver( MessageName::default_instance().GetTypeName(), this );
    }

    void process( const char* a_buffer )
    {
        m_msg->ParseFromArray( a_buffer, strlen( a_buffer ) );
        receive( m_msg );
    }

    virtual void receive( const std::unique_ptr< MessageName >& msg ) = 0;
private:
    std::unique_ptr< MessageName > m_msg;
};


template< typename MessageName >
class Observer< MessageName, typename std::enable_if< std::is_base_of< flatbuffers::NativeTable, MessageName >::value >::type > : public ObserverBase
{
public:
    ~Observer() {}

protected:
    Observer() {}

    void initReceiver()
    {
        ObserverBase::addObserver( MessageName::GetFullyQualifiedName(), this );
    }

    void destroyReceiver()
    {
        ObserverBase::removeObserver( MessageName::GetFullyQualifiedName(), this );
    }

    void process( const char* a_buffer )
    {
        std::unique_ptr< MessageName > message = std::unique_ptr< MessageName >( flatbuffers::GetRoot< typename MessageName::TableType >( a_buffer )->UnPack(nullptr) );
        receive( message );
    }

    virtual void receive( const std::unique_ptr< MessageName >& msg ) = 0;
};

}
}
#endif // SEPIA_COMM2_OBSERVER_H

