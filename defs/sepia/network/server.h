#ifndef SEPIA_NETWORK_TCPSERVER_H
#define SEPIA_NETWORK_TCPSERVER_H
#include <atomic>
#include <thread>
#include <string>
#include <vector>

namespace sepia
{
namespace network
{
    class TcpSocket;
}
}
namespace sepia
{
namespace network
{
class Server
{
    public:
        Server( const std::string& a_host, int a_port );
        Server( int a_port );
        ~Server();
        void start();
        void stop();
        void join();
    protected:
        void own_thread();
        void dispatchSocket( int hsock );
        void handleSocket( const std::string a_clientHostName, sepia::network::TcpSocket* a_socket );

    private:
        std::string m_host;
        int m_port;
        std::atomic_bool m_terminate;
        std::thread m_thread;
        std::vector< std::thread > m_connections;

};

}
}
#endif // SEPIA_NETWORK_TCPSERVER_H
