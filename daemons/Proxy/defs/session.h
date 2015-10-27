#ifndef __sepia_daemons_proxy_session_h
#define __sepia_daemons_proxy_session_h

#include <sepia/comm/observerraw.h>
#include <sepia/comm/observer.h>
#include <sepia/util/threadobject.h>
#include "header.pb.h"
#include "internal.pb.h"

class MsgSocket;

class Session : public sepia::comm::ObserverRaw
              , public sepia::comm::Observer< sepia::comm::internal::ForwardSubscribe >
              , public sepia::comm::Observer< sepia::comm::internal::ForwardUnsubscribe >
              , public sepia::comm::Observer< sepia::comm::internal::RemoteSubscription >
              , public sepia::comm::Observer< sepia::comm::internal::RemoteUnsubscription >
              , public sepia::util::ThreadObject
{
public:
    Session( int a_socket );
    ~Session();
    void start();

protected:
    void receive( const sepia::comm::internal::ForwardSubscribe* a_message );
    void receive( const sepia::comm::internal::ForwardUnsubscribe* a_message );
    void receive( const sepia::comm::internal::RemoteSubscription* a_message );
    void receive( const sepia::comm::internal::RemoteUnsubscription* a_message );
    void receiveRaw( const sepia::comm::Header *a_header, const char *a_buffer, size_t a_size );
    void own_thread();

private:
    Session() = delete;
    typedef std::unordered_set< std::string > MessageSet;
    MessageSet m_messages;
    MsgSocket* m_socket;
    std::vector< char > m_sendBuffer;
    sepia::comm::Header* m_header;
};
#endif // __sepia_daemons_proxy_session_h
