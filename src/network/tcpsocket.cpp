/*
Copyright (c) 2012-2015, Kai Hugo Hustoft Endresen <kai.endresen@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
#include <netinet/tcp.h>

namespace sepia
{
namespace network
{

TcpSocket::TcpSocket( int a_socket ) : m_sock( a_socket )
{

}

TcpSocket::TcpSocket() : m_sock( 0 )
{
}

TcpSocket::~TcpSocket()
{
    close();
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

    /*
    if( setsockopt( m_sock, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_KEEPALIVE: " << strerror( errno ) << std::endl;
    }
    */

    value = 262144;


    if( setsockopt( m_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_RCVBUF: " << strerror( errno ) << std::endl;
    }

    if( setsockopt( m_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting SO_SNDBUF: " << strerror( errno ) << std::endl;
    }

/*
    if( setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>( &value ), sizeof( value ) ) != 0 )
    {
        std::cerr << "Error setting TCP_NODELAY: " << strerror( errno ) << std::endl;
    }
*/
}

void TcpSocket::fromRawSocket( int a_socket )
{
    if( !m_sock )
    {
        m_sock = a_socket;
    }
    else
    {
        close();
        m_sock = a_socket;
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

void TcpSocket::sendString( const std::string& a_string )
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

void TcpSocket::receiveString( std::string& a_string )
{
    short str_size = 0;

    int err = 0;
    receive( str_size );

    std::cout << "got size: " << str_size << std::endl;
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

void TcpSocket::send( const char* a_data, std::size_t a_size )
{
    if( rawSend( a_data, a_size ) < 0 )
    {
        throwError( errno );
    }
}

void TcpSocket::receive( char* a_data, std::size_t a_size )
{
    if( rawReceive( a_data, a_size ) < 0 )
    {
        throwError( errno );
    }
}

int TcpSocket::rawReceive( char* a_data, std::size_t a_size )
{
    int bytes_received = 0;

    do
    {
        int bytes = ::recv( m_sock, a_data+bytes_received, a_size-bytes_received, 0 );
        if( bytes < 0 )
        {
            std::cout << "Received: -1" << std::endl;
            return -1;
        }
        bytes_received += bytes;
        //std::cout << "DEBUG:" << " received bytes: " << bytes << " out of: " << bytes_received << std::endl;

    } while( bytes_received != a_size );

    return bytes_received;
}

int TcpSocket::rawSend( const char* a_data, std::size_t a_size )
{
    int bytes_sent = 0;
    do
    {
        int bytes = ::send( m_sock, a_data+bytes_sent, a_size-bytes_sent, 0 );
        if( bytes < 0 )
        {
            std::cout << "Sent: -1" << std::endl;
            return -1;
        }
        bytes_sent += bytes;
        //std::cout << "DEBUG:" << " sent bytes: " << bytes << " out of: " << bytes_sent << std::endl;
    } while( bytes_sent != a_size );

    return bytes_sent;
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
    if( m_sock )
    {
        ::close( m_sock );
    }
}

}
}
