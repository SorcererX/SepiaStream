#ifndef SEPIA_COMM_OBSERVER_H
#define SEPIA_COMM_OBSERVER_H
#include <string>
#include <map>
#include <vector>
#include "header.pb.h"
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <unordered_set>


namespace sepia
{
namespace comm
{

class ObserverBase
{
public:
    typedef struct {
        boost::shared_ptr< boost::mutex > mutex;
        //boost::shared_ptr< boost::condition_variable > cond;
        boost::shared_ptr< boost::barrier > barrier;
        std::vector< char > buffer;
        int32_t length;
        Header header;
    } ThreadMessageData;
    static bool threadReceiver();
    static bool routeMessageToThreads( const Header* a_header, char* a_buffer, const size_t a_size );

protected:
    virtual void process( const Header* a_header, const char* a_buffer, size_t a_size ) = 0;
    void addObserver( const std::string a_name, ObserverBase* a_Observer );
    void removeObserver( const std::string a_name );
    void addRouter( ObserverBase* a_Observer );
    ObserverBase();
    static bool routeToNode( unsigned int node, const Header* a_header, char* a_buffer, const size_t a_size );
    static bool handleReceive( const Header* a_header, const char* a_buffer, const size_t a_size );

private:

    typedef std::map< std::string, ObserverBase* > ObserverMap;
    typedef std::map< int, ThreadMessageData > ThreadMap;
    typedef std::map< std::string, std::unordered_set< unsigned int > > MessageNameMap;
    static MessageNameMap sm_messageNameToThread;
    static std::unordered_set< unsigned int > sm_routerThreads;
    static thread_local ObserverMap stm_observers;
    static thread_local ThreadMessageData* stm_ownData;
    static ThreadMap sm_threadData;
    static boost::shared_ptr< boost::mutex > sm_globalMutex;
    static thread_local ObserverBase* stm_router;
};


template< class MessageName >
class Observer : protected ObserverBase
{
public:
    ~Observer() {
        ObserverBase::removeObserver( MessageName::default_instance().GetTypeName() );
    }

protected:
    Observer() {
        ObserverBase::addObserver( MessageName::default_instance().GetTypeName(), this );
    }
    void process( const Header* a_header, const char* a_buffer, size_t a_size )
    {
        if( !a_header )
        {
            m_message.ParseFromArray( a_buffer, a_size );
            std::cout << "Observer: " << m_message.GetTypeName() << " " << m_message.ShortDebugString() << std::endl;
            receive( &m_message );
        }
    }
    virtual void receive( const MessageName* msg ) = 0;
private:
    MessageName m_message;
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
