#ifndef SEPIA_COMM_OBSERVER_H
#define SEPIA_COMM_OBSERVER_H
#include <sepia/comm/observerbase.h>
#include <string>
#include <map>
#include <iostream>
#include <list>

namespace sepia
{
namespace comm
{



template< class MessageName >
class Observer : protected ObserverBase
{
public:
    ~Observer() {
        if( m_initialized )
        {
            std::cerr << "Observer: forgot to destroy receiver before deconstruction!" << std::endl;
        }
    }

protected:
    Observer() : m_initialized( false ) {
    }
    void process( const Header* a_header, const char* a_buffer, size_t a_size )
    {
        if( !a_header )
        {
            m_message.ParseFromArray( a_buffer, a_size );
            if( ObserverBase::debugEnabled() )
            {
                std::cout << "Observer: " << m_message.GetTypeName() << " " << m_message.ShortDebugString() << std::endl;
            }
            receive( &m_message );
        }
    }
    virtual void receive( const MessageName* msg ) = 0;
    void initReceiver()
    {
        sm_globalMutex->lock();
        m_initialized = true;
        ObserverBase::addObserver( MessageName::default_instance().GetTypeName(), this );
        ObserverBase::initReceiver();
        sm_globalMutex->unlock();
    }

    void destroyReceiver()
    {
        if( m_initialized )
        {
            sm_globalMutex->lock();
            m_initialized = false;
            ObserverBase::removeObserver( MessageName::default_instance().GetTypeName() );
            sm_globalMutex->unlock();
        }
    }

private:
    MessageName m_message;
    bool m_initialized;
};

}
}
#endif // SEPIA_COMM_OBSERVER_H
