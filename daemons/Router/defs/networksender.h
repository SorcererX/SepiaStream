#include <sepia/comm/observerraw.h>
#include <sepia/comm/observer.h>
#include <sepia/util/threadobject.h>
#include "header.pb.h"
#include "internal.pb.h"

class NetworkSender : public sepia::comm::ObserverRaw
                    , public sepia::comm::Observer< sepia::comm::internal::ForwardSubscribe >
                    , public sepia::comm::Observer< sepia::comm::internal::ForwardUnsubscribe >
                    , public sepia::comm::Observer< sepia::comm::internal::RemoteSubscription >
                    , public sepia::comm::Observer< sepia::comm::internal::RemoteUnsubscription >
                    , public sepia::util::ThreadObject
{
public:
    NetworkSender();
    ~NetworkSender();
    //NetworkSender( const NetworkSender& a_object );
    void start();
    void stop();

protected:
    void receive( const sepia::comm::internal::ForwardSubscribe* a_message );
    void receive( const sepia::comm::internal::ForwardUnsubscribe* a_message );
    void receive( const sepia::comm::internal::RemoteSubscription* a_message );
    void receive( const sepia::comm::internal::RemoteUnsubscription* a_message );
    void receiveRaw( const sepia::comm::Header *a_header, const char *a_buffer, size_t a_size );
    void own_thread();

private:
    typedef std::unordered_set< std::string > MessageSet;
    MessageSet m_messages;
    MessageSet m_remoteSubscriptions;
};
