#include <sepia/network/tcpsocket.h>
#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

namespace sepia
{
namespace network
{

TcpSocket::TcpSocket()
{
}

TcpSocket::~TcpSocket()
{
    ::close( m_sock );
}

void TcpSocket::init()
{
    m_sock = ::socket( AF_INET, SOCK_STREAM, 0 );

    if( m_sock == -1)
    {
        std::cerr << "error initializing socket " << errno << std::endl;
        return ;
    }

    int value = 1;

    if( setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_REUSEADDR: " << strerror( errno ) << std::endl;
    }

    if( setsockopt( m_sock, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_KEEPALIVE: " << strerror( errno ) << std::endl;
    }

    value = 2 * 1280 * 1024;

    if( setsockopt( m_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_RCVBUF: " << strerror( errno ) << std::endl;
    }

    if( setsockopt( m_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_SNDBUF: " << strerror( errno ) << std::endl;
    }

}

void TcpSocket::connect( const std::string& a_host, int a_port )
{
    init();
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons( a_port );

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = inet_addr( a_host.c_str() );

    if( ::connect( m_sock, (struct sockaddr*)&my_addr, sizeof(my_addr) ) < 0 )
    {
        std::cerr << "Error connecting socket " << strerror( errno ) << std::endl;
        return ;
    }
}

void TcpSocket::bind( int a_port )
{
    init();
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons( a_port );
    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = INADDR_ANY ;

    if( ::bind( m_sock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
    {
        std::cerr << "Error binding to socket, make sure nothing else is listening on this port" << errno << std::endl;
        return ;
    }
}

void TcpSocket::listen( int a_maxval )
{
    if( ::listen( m_sock, a_maxval ) == -1 )
    {
        std::cerr << "Error listening " << errno << std::endl;
        return ;
    }
}

std::string TcpSocket::accept( int a_socket )
{
    sockaddr_in addr;
    socklen_t addr_size = sizeof( sockaddr_in );

    if( ( m_sock = ::accept( a_socket, (sockaddr*) &addr, &addr_size ) ) < 0 )
    {
        std::cerr << "error accepting " << errno << std::endl;
        return std::string( "ERROR" );
    }
    return  std::string( inet_ntoa( addr.sin_addr ) );
}

template<> void TcpSocket::send< std::string >( std::string& a_string )
{
    short str_size;
    str_size = static_cast< short >( a_string.size() );

    int err = 0;
    send( str_size );

    if( rawSend( a_string.c_str(), str_size ) < 0 )
    {
        err = errno;
        close();
        std::string error = "Socket::send( string ) - unable to send string : ";
        error.append( strerror( err ) );
        throw error;
    }
}

template<> void TcpSocket::receive< std::string >( std::string& a_string )
{
    short str_size = 0;

    int err = 0;
    receive( str_size );

    std::vector< char > array;
    array.assign( str_size, 0 );

    if( rawReceive( &array[0], array.size() ) < 0 )
    {
        err = errno;
        close();
        std::string error = "Socket::receive( string ) - unable to receive string : ";
        error.append( strerror( err ) );
        throw error;
    }

    a_string.assign( array.begin(), array.end() );
}

int TcpSocket::getFD() const
{
    return m_sock;
}

void TcpSocket::send( char* a_data, size_t a_size )
{
    if( rawSend( a_data, a_size ) < 0 )
    {
        throwError( errno );
    }
}

void TcpSocket::receive( char* a_data, size_t a_size )
{
    if( rawReceive( a_data, a_size ) < 0 )
    {
        throwError( errno );
    }
}

int TcpSocket::rawReceive( char* a_data, size_t a_size )
{
    int bytes = ::recv( m_sock, a_data, a_size, MSG_WAITALL );
    if( bytes != a_size )
    {
        std::cerr << "WARN: " << "receive does not match expected. got: " << bytes << " expected: " << a_size << std::endl;
    }
    return bytes;
}

int TcpSocket::rawSend( const char* a_data, size_t a_size )
{
    int bytes = ::send( m_sock, a_data, a_size, MSG_WAITALL );
    if( bytes != a_size )
    {
        std::cerr << "WARN: " << "send does not match expected. got: " << bytes << " expected: " << a_size << std::endl;
    }
    return bytes;
}

void TcpSocket::throwError( int a_error )
{
    close();
    std::string error = "Socket::receive() - error: ";
    error.append( strerror( a_error ) );
    throw error;
}

void TcpSocket::close()
{
    ::close( m_sock );
}

}
}
