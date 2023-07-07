#ifndef SEPIA_COMM_OBSERVERRAW_H
#define SEPIA_COMM_OBSERVERRAW_H
#include <sepia/comm/observerbase.h>

namespace sepia
{
namespace comm
{

class ObserverRaw : protected ObserverBase
{
public:
    ~ObserverRaw() {
        // TODO implement destructor.
    }
protected:
    void process( const Header* a_header, const char* a_buffer, std::size_t a_size )
    {
        receiveRaw( a_header, a_buffer, a_size );
    }

    void initRawReceiver( const std::string a_name )
    {
        sm_globalMutex->lock();
        ObserverBase::addObserver( a_name, this );
        ObserverBase::initReceiver();
        sm_globalMutex->unlock();
    }

    void destroyRawReceiver( const std::string a_name )
    {
        sm_globalMutex->lock();
        ObserverBase::removeObserver( a_name );
        sm_globalMutex->unlock();
    }
    virtual void receiveRaw( const Header* a_header, const char* a_buffer, std::size_t a_size ) = 0;

private:
    bool m_initialized;
    typedef std::unordered_set< std::string > MessageSet;
    MessageSet m_messages;
};

}
}

#endif // SEPIA_COMM_OBSERVERRAW_H
