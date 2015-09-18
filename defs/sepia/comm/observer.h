#ifndef SEPIA_COMM_OBSERVER_H
#define SEPIA_COMM_OBSERVER_H
#include <string>
#include <map>
#include <vector>
#include "header.pb.h"
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <iostream>
#include <thread>
#include <list>

namespace sepia
{
namespace comm
{

class ObserverBase
{
public:
    typedef struct {
        std::shared_ptr< std::mutex > mutex;
        std::shared_ptr< std::condition_variable > cond;
        std::vector< char > buffer;
        int32_t length;
        Header header;
        bool data_ready;
        bool terminateReceiver;
    } ThreadMessageData;

    static bool threadReceiver();
    static void stopThreadReceiver( std::thread::id a_threadId );
    static void enableDebug( bool a_enable );
    static bool routeMessageToThreads( const Header* a_header, char* a_buffer, const size_t a_size );
    static bool resendAllSubscriptions();
    static bool debugEnabled();

protected:
    void initReceiver();
    virtual void process( const Header* a_header, const char* a_buffer, size_t a_size ) = 0;
    void addObserver( const std::string a_name, ObserverBase* a_Observer );
    void removeObserver( const std::string a_name );
    void addRouter( ObserverBase* a_Observer );
    ObserverBase();
    static bool routeToNode( std::thread::id a_node, const Header* a_header, char* a_buffer, const size_t a_size );
    static bool handleReceive( const Header* a_header, const char* a_buffer, const size_t a_size );

private:

    typedef std::map< std::string, ObserverBase* > ObserverMap;
    typedef std::map< std::thread::id, ThreadMessageData > ThreadMap;
    typedef std::map< std::string, std::unordered_set< std::thread::id > > MessageNameMap;
    static MessageNameMap sm_messageNameToThread;
    static std::unordered_set< std::thread::id > sm_routerThreads;
    static thread_local ObserverMap stm_observers;
    static thread_local ThreadMessageData* stm_ownData;
    static ThreadMap sm_threadData;
    static std::shared_ptr< std::mutex > sm_globalMutex;
    static thread_local ObserverBase* stm_router;
    static std::list< std::string > sm_subscriptionList;
    static bool sm_debugEnabled;
    static bool sm_gotIdResponse;
};


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

class ObserverRouter : protected ObserverBase
{
public:
    ~ObserverRouter() {
       //TODO implement destructor.
    }
protected:
    ObserverRouter() {
        ObserverBase::addRouter( this );
    }
    void process( const Header* a_header, const char* a_buffer, size_t a_size )
    {
        route( a_header, a_buffer, a_size );
    }
    virtual void route( const Header* a_header, const char* a_buffer, size_t a_size ) = 0;
};


}
}
#endif // SEPIA_COMM_OBSERVER_H
