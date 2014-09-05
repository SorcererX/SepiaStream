#include <sepia/stream.h>
#include <sepia/databuffer.h>
#include <iostream>

namespace sepia
{
Stream::Stream( std::string name, u_int32_t images, u_int32_t width, u_int32_t height, u_int32_t bpp )
{
    m_element.data = NULL;
    m_element.info = NULL;
    size_t required_size = sizeof( group_header_t ) + images * ( sizeof( image_header_t ) + width*height*bpp/8 );
    m_buffer = new sepia::DataBuffer( name, 16, required_size );
}

Stream::Stream( std::string name )
{
    m_element.data = NULL;
    m_element.info = NULL;
    m_buffer = new sepia::DataBuffer( name );
}

Stream::group_header_t* Stream::getGroupHeaderFromElement( char* element )
{
    if( element )
    {
        void* addr = static_cast< void* >( element );
        return static_cast< group_header_t* >( addr );
    }
    else
    {
        return NULL;
    }
}

Stream::group_header_t* Stream::getGroupHeader()
{
    return getGroupHeaderFromElement( m_element.data );
}


char* Stream::getHeaderAddressFromElement( char* element, size_t id )
{
    if( element )
    {
        return element + sizeof( group_header_t ) + id * ( sizeof( image_header_t ) + getGroupHeaderFromElement( element )->max_size );
    }
    else
    {
        return NULL;
    }
}

char* Stream::getHeaderAddress( size_t id )
{
    if( id < getGroupHeader()->count )
    {
        return getHeaderAddressFromElement( m_element.data, id );
    }
    else
    {
        return NULL;
    }
}


char* Stream::getAddress( size_t id )
{
    char* hdr = getHeaderAddress( id );

    if( hdr )
    {
        return hdr + sizeof( image_header_t );
    }
    else
    {
        return NULL;
    }
}

Stream::image_header_t* Stream::getHeader( size_t id )
{
    char* hdr = getHeaderAddress( id );
    if( hdr )
    {
        void* address = static_cast< void* >( hdr );
        return static_cast< image_header_t* >( address );
    }
    else
    {
        return NULL;
    }
}

size_t Stream::getSize( size_t id )
{
    image_header_t* header = getHeader( id );
    if( header )
    {
        return header->size;
    }
    else
    {
        return 0;
    }
}

size_t Stream::getMaxSize()
{
    return getGroupHeader()->max_size;
}

bool Stream::setSize( size_t id, u_int32_t size )
{
    image_header_t* header = getHeader( id );
    if( header )
    {
        if( size <= getGroupHeader()->max_size )
        {
            header->size = size;
            return true;
        }
    }
    return false;
}
}
