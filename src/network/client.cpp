#include <client.h>
#include <sepia/writer.h>
#include <limits>
#include <iostream>
#include <sepia/network/tcpsocket.h>

namespace sepia
{
namespace network
{

Client::Client( const std::string& a_host, int a_port, const std::string& a_name )
            : m_host( a_host )
            , m_port( a_port )
            , m_name( a_name )
            , m_terminate( true )
{

}

Client::~Client()
{

}

void Client::start()
{
    if( m_terminate )
    {
        m_terminate = false;
        if( m_name.size() > std::numeric_limits<short>::max() )
        {
            return ;
        }
        m_thread = std::thread( &Client::own_thread, this );
    }
}

void Client::stop()
{
    m_terminate = true;
}

void Client::join()
{
    m_thread.join();
}

void Client::own_thread()
{
    TcpSocket sock;
    sock.connect( m_host, m_port );

    // first send size of name.
    sock.send( m_name );

    unsigned int images = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int bpp = 0;
    try
    {
        sock.receive( images );
        sock.receive( width );
        sock.receive( height );
        sock.receive( bpp );
    } catch( std::string a_error )
    {
        std::cerr << a_error << std::endl;
        return ;
    }
    std::cout << "DEBUG: " << "images: " << images << " w: " << width << " h: " << height << " bpp: " << bpp << std::endl;

    sepia::Writer data( m_name + "_TEST", images, width, height, bpp );

    while( !m_terminate )
    {
        for( int i = 0; i < data.getGroupHeader()->count; i++ )
        {
            sock.receive( *data.getHeader( i ) );
            sock.receive( data.getAddress( i ), data.getSize( i ) );
        }
        data.update();
    }
}

}
}
