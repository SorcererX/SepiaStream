#ifndef SEPIA_COMM2_OBSERVER_H
#define SEPIA_COMM2_OBSERVER_H
#include <sepia/comm2/observerbase.h>
#include <iostream>
#include <flatbuffers/flatbuffers.h>

namespace sepia
{
namespace comm2
{

template< class MessageName >
class Observer : public ObserverBase
{
public:
    ~Observer()
    {
    }

protected:
    Observer()
    {
    }

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

