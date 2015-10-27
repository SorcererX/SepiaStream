#include "proxyserver.h"
#include <sepia/network/tcpsocket.h>
#include "session.h"

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

void ProxyServer::start()
{
    m_thread = new std::thread( std::bind( &ProxyServer::own_thread, this ) );
}

void ProxyServer::own_thread()
{
    m_socket->listen( 5 );

    while( !m_terminate )
    {
        std::cout << "waiting.." << std::endl;
        int sock;
        m_socket->accept( sock );
        Session* connection = new Session( sock );
        m_sessions.push_back( connection );
        connection->start();
    }
}
