#ifndef __sepia_daemons_server_h
#define __sepia_daemons_server_h
#include <sepia/util/threadobject.h>
#include <string>
#include <vector>

namespace sepia
{
    namespace network
    {
        class TcpSocket;
    }
}

class Session;

class ProxyServer : public sepia::util::ThreadObject
{
public:
    ProxyServer( int a_port );
    ~ProxyServer();
    void start();
    void stop();

protected:
    void own_thread();

private:
    sepia::network::TcpSocket* m_socket;
    std::vector< Session* > m_sessions;
};
#endif // __sepia_daemons_server_h
