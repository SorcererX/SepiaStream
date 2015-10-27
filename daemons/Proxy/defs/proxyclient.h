#ifndef __sepia_daemons_client_h
#define __sepia_daemons_client_h
#include <string>

namespace sepia
{
    namespace network
    {
        class TcpSocket;
    }
}

class Session;

class ProxyClient
{
public:
    ProxyClient( const std::string& a_host, int a_port );
    ~ProxyClient();

private:
    sepia::network::TcpSocket* m_socket;
    Session* m_session;
};
#endif // __sepia_daemons_client_h
