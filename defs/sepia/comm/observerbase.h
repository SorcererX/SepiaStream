#ifndef SEPIA_COMM_OBSERVERBASE_H
#define SEPIA_COMM_OBSERVERBASE_H

// thread
#include <thread>
#include <mutex>
#include <condition_variable>

// containers
#include <vector>
#include <list>
#include <unordered_set>

// protobuf
#include "header.pb.h"

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
    static bool routeMessageToThreads( const Header* a_header, char* a_buffer, const std::size_t a_size );
    static bool resendAllSubscriptions();
    static bool debugEnabled();
    static std::string commName();

protected:
    void initReceiver();
    virtual void process( const Header* a_header, const char* a_buffer, std::size_t a_size ) = 0;
    void addObserver( const std::string a_name, ObserverBase* a_Observer );
    void removeObserver( const std::string a_name );
    ObserverBase();
    static bool routeToNode( std::thread::id a_node, const Header* a_header, char* a_buffer, const std::size_t a_size );
    static bool handleReceive( const Header* a_header, const char* a_buffer, const std::size_t a_size );
    static std::shared_ptr< std::mutex > sm_globalMutex;

private:

    typedef std::map< std::string, ObserverBase* > ObserverMap;
    typedef std::map< std::thread::id, ThreadMessageData > ThreadMap;
    typedef std::map< std::string, std::unordered_set< std::thread::id > > MessageNameMap;
    static MessageNameMap sm_messageNameToThread;
    static thread_local ObserverMap stm_observers;
    static thread_local ThreadMessageData* stm_ownData;
    static ThreadMap sm_threadData;
    static thread_local ObserverBase* stm_router;
    static std::list< std::string > sm_subscriptionList;
    static bool sm_debugEnabled;
    static bool sm_gotIdResponse;
    static std::string sm_commName;
};
}
}
#endif // SEPIA_COMM_OBSERVERBASE_H
