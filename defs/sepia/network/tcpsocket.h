#ifndef SEPIA_SOCKET_H
#define SEPIA_SOCKET_H
#include <string>
#include <errno.h>
#include <iostream>
#include <vector>
#include <cstring>

namespace sepia
{
namespace network
{
class TcpSocket
{
public:
    TcpSocket();
    TcpSocket( int a_socket );
    ~TcpSocket();
    void fromRawSocket( int a_socket );
    std::string accept( int a_socket );
    void bind( int a_port );
    void listen( int a_maxVal );
    void connect( const std::string& a_host, int a_port );
    int getFD() const;
    template< class T > void receive( T& a_data );
    template< class T > void send( T& a_data );

    void receive( char* a_data, size_t a_size );
    void send( const char* a_data, size_t a_size );
    void receiveString( std::string& a_string );
    void sendString( const std::string& a_string );

    void throwError( int a_error );
    void close();

protected:
    void init();
    int rawSend( const char* a_data, size_t a_size );
    int rawReceive( char* a_data, size_t a_size );

private:
    int m_sock;

};

template< class T > void TcpSocket::receive( T& a_data )
{
    if( ( rawReceive( reinterpret_cast<char*>( &a_data ), sizeof( T ) ) ) < 0 )
    {
        throwError( errno );
    }
}

template< class T > void TcpSocket::send( T& a_data )
{
    if( ( rawSend( reinterpret_cast<char*>( &a_data ), sizeof( T ) ) ) < 0 )
    {
        throwError( errno );
    }
}

}
}
#endif // SEPIA_SOCKET_H
