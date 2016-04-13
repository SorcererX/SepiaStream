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

#include <client.h>
#include <sepia/writer.h>
#include <limits>
#include <iostream>
#include <sepia/network/tcpsocket.h>
#include <string.h> // for memcpy

namespace
{
#ifdef LIBDE265_SUPPORT
    void write_image_to_data( const de265_image* a_input, char* a_output )
    {
        // only need channel = 0 for MONO
        int c = 0;
        int stride;
        const uint8_t* p = de265_get_image_plane( a_input, c, &stride);

        int width = de265_get_image_width( a_input, c );
        int height = de265_get_image_height( a_input, c );

        if( de265_get_bits_per_pixel( a_input, c ) <= 8 )
        {
            for( int y = 0; y < height; y++ )
            {
                memcpy( a_output + y * height, p+y*stride, width );
            }
        }
        else
        {
            // TODO: implement 16-bit support
        }
    }
#endif
}

namespace sepia
{
namespace network
{

Client::Client( const std::string& a_host, int a_port, const std::string& a_name, bool a_useCompression )
            : m_host( a_host )
            , m_port( a_port )
            , m_name( a_name )
            , m_useCompression( a_useCompression )
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

    unsigned int version = 1;
    unsigned int options = 0;

    bool enable_encoding = true; // should be determined at command-line.

    if( enable_encoding )
    {

#ifdef LIBDE265_SUPPORT
        de265_init();
        m_h265Decoder = de265_new_decoder();

        options |= (1 << 0); // enable encoding
#elif
        std::cerr << "Encoding support needs to be enabled at compile time, disabling encoding." << std::endl;
#endif

    }
    const de265_image* img_de265;
    de265_error err_de265;

    sock.send( version );
    sock.send( options );
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

    sepia::Writer data( m_name, images, width, height, bpp );


    int more = 0; // more data available if true.
    while( !m_terminate )
    {
        for( int i = 0; i < data.getGroupHeader()->count; i++ )
        {
            sock.receive( *data.getHeader( i ) );
            if( enable_encoding )
            {
#ifdef LIBDE265_SUPPORT
                int packet_size;
                sock.receive( reinterpret_cast< char* >( &packet_size ), sizeof( packet_size ) );
                sock.receive( data.getAddress( i ), packet_size ); // using destination as temporary storage.

                de265_push_data( m_h265Decoder,
                                 reinterpret_cast< void* >( *data.getAddress( i ) ),
                                 data.getSize( i ),
                                 1,
                                 NULL );
                err_de265 = de265_decode( m_h265Decoder, &more );

                if( err_de265 == DE265_OK )
                {
                    img_de265 = de265_get_next_picture( m_h265Decoder );
                    if( img_de265 )
                    {
                        write_image_to_data( img_de265, data.getAddress( i ) ); // writing image to destination
                    }
                }
#elif
                std::cerr << "Encoded data stream: compile with LIBDE265_SUPPORT enabled." << std::endl;
                return ;
#endif
            }
            else
            {
                sock.receive( data.getAddress( i ), data.getSize( i ) );
            }
        }
        data.update();
    }
}

}
}
