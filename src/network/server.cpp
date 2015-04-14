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
