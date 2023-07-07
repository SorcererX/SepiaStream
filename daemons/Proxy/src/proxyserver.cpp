#include "proxyserver.h"
#include <sepia/network/tcpsocket.h>
#include "session.h"
#include <functional>

ProxyServer::ProxyServer( int a_port )
{
    m_socket = new sepia::network::TcpSocket();
    m_socket->bind( a_port );
}

ProxyServer::~ProxyServer()
{
    m_socket->close();
    for( int i = 0; i < m_sessions.size(); i++ )
    {
        m_sessions[ i ]->stop();
    }
    for( int i = 0; i < m_sessions.size(); i++ )
    {
        m_sessions[ i ]->join();
    }
    delete m_socket;
}

void ProxyServer::own_thread()
{
    m_socket->listen( 5 );

    sepia::network::TcpSocket* socket;

    while( !m_terminate )
    {
        std::cout << "waiting.." << std::endl;
        sepia::network::TcpSocket* socket = new sepia::network::TcpSocket();
        socket->accept( m_socket->getFD() );

        Session* connection = new Session( socket );

        m_sessions.push_back( connection );
        connection->start();
    }
}
