#include <iostream>
#include <sepia/network/server.h>

#include <sepia/network/tcpsocket.h>
#include <sepia/reader.h>

namespace sepia
{
namespace network
{

Server::Server( const std::string& a_host, int a_port ) : m_port( a_port )
                                                        , m_host( a_host )
                                                        , m_terminate( true )
{
}

Server::Server( int a_port ) : m_port( a_port )
                             , m_host( "0.0.0.0" )
                             , m_terminate( true )
{
}

Server::~Server()
{

}

void Server::start()
{
    if( m_terminate )
    {
        m_terminate = false;
        m_thread = std::thread( &Server::own_thread, this );
    }
}

void Server::stop()
{
    m_terminate = true;
}

void Server::join()
{
    m_thread.join();
}

void Server::own_thread()
{
    TcpSocket sock;
    sock.bind( m_port );
    sock.listen( 5 );

    while( !m_terminate )
    {
        std::cout << "waiting.." << std::endl;
        dispatchSocket( sock.getFD() );
    }
}

void Server::dispatchSocket( int hsock )
{
    TcpSocket* sock = new TcpSocket();
    std::string hostname = sock->accept( hsock );

    std::cout << "Got connection from: " << hostname << std::endl;
    m_connections.push_back( std::thread( std::bind( &Server::handleSocket, this, hostname, sock ) ) );
}

void Server::handleSocket( const std::string a_clientHostName, TcpSocket* a_socket )
{
    std::string name;
    try
    {
        a_socket->receive( name );
    }
    catch( std::string a_error )
    {
        std::cerr << "NAME: " << a_error << std::endl;
        delete a_socket;
        return ;
    }

    sepia::Reader data( name );

    unsigned int images = data.getGroupHeader()->count;
    unsigned int width = data.getHeader(0)->width;
    unsigned int height = data.getHeader(0)->height;
    unsigned int bpp = data.getHeader(0)->bpp;
    std::cout << "DEBUG: " << "images: " << images << " w: " << width << " h: " << height << " bpp: " << bpp << std::endl;
    try
    {
        a_socket->send( images );
        a_socket->send( width );
        a_socket->send( height );
        a_socket->send( bpp );
    }
    catch( std::string a_error )
    {
        std::cerr << "INITIAL: " << a_error << std::endl;
        delete a_socket;
        return ;
    }

    while( !m_terminate )
    {
        for( int i = 0; i < data.getGroupHeader()->count; i++ )
        {
            try
            {
                a_socket->send( *data.getHeader( i ) );
                a_socket->send( data.getAddress( i ), data.getSize( i ) );
            }
            catch( std::string a_error )
            {
                std::cerr << "TRANSFER: " << a_error << std::endl;
                delete a_socket;
                return ;
            }
        }
        data.update();
    }
}
}
}
