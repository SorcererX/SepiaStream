#include "proxyclient.h"
#include "session.h"
#include <sepia/network/tcpsocket.h>

ProxyClient::ProxyClient( const std::string& a_host, int a_port )
{
    m_socket = new sepia::network::TcpSocket();
    m_socket->connect( a_host, a_port );
    m_session = new Session( m_socket->getFD() );
    m_session->start();
}

ProxyClient::~ProxyClient()
{
    delete m_session;
    m_socket->close();
    delete m_socket;
}

