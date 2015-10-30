#ifndef MESSAGEROUTER_H
#define MESSAGEROUTER_H
#include <sepia/util/threadobject.h>
#include <map>
#include <unordered_set>
#include "internal.pb.h"
#include "header.pb.h"
#include <sepia/comm/observer.h>
#include <sepia/comm/observerraw.h>


class ProcessMonitor;

namespace sepia
{
namespace comm
{
    class MessageHandler;
}
}

class MessageRouter : public sepia::comm::ObserverRaw,
                      public sepia::comm::Observer< sepia::comm::internal::IdNotify >,
                      public sepia::comm::Observer< sepia::comm::internal::Subscribe >,
                      public sepia::comm::Observer< sepia::comm::internal::UnSubscribe >,
                      public sepia::comm::Observer< sepia::comm::internal::ProcessDied >
{
public:
    MessageRouter( const std::string a_incomingQueueName );
    ~MessageRouter();
    void setProcessMonitor( ProcessMonitor* a_processMonitor );

protected:
    void own_thread();
    void receive( const sepia::comm::internal::IdNotify*    a_message );
    void receive( const sepia::comm::internal::Subscribe*   a_message );
    void receive( const sepia::comm::internal::UnSubscribe* a_message );
    void receive( const sepia::comm::internal::ProcessDied* a_message );

    void receiveRaw( const sepia::comm::Header *a_header, const char *a_buffer, size_t a_size );
    bool addRoute( const unsigned int a_id, const std::string a_queue );
    void removeRoute( const unsigned int a_id );
    template< class MessageName > void routeMessage( MessageName* msg );

private:
    std::string m_incomingQueueName;
    typedef std::map< unsigned int, sepia::comm::MessageHandler* > MessageHandlerList;
    MessageHandlerList m_outputMessageQueues;

    typedef std::map< std::string, std::unordered_set< unsigned int > > MessageNameMap;
    MessageNameMap m_messageNameToSubscribers;

    sepia::comm::MessageHandler* m_managementQueue;

    ProcessMonitor* m_processMonitor;

    char* m_buffer;
    unsigned int m_currentNodeId;
    unsigned int m_routerId;
};

#endif // MESSAGEROUTER_H
