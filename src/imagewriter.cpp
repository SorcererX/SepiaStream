#include "imagewriter.h"
#include <iostream>
#include <cstring>

ImageWriter::ImageWriter( std::string name, u_int32_t images, u_int32_t width, u_int32_t height, u_int32_t bpp )
    : ImageStream( name, images, width, height, bpp )
{
    m_previousElement.data = NULL;
    m_previousElement.info = NULL;
    m_buffer->getWriteAccess( m_element );

    // Fill element with valid data.
    getGroupHeader()->max_size = width*height*bpp/8;
    getGroupHeader()->count = images;

    for( size_t i = 0; i < getGroupHeader()->count; i++ )
    {
        image_header_t* hdr = getHeader( i );
        hdr->width = width;
        hdr->height = height;
        hdr->size = getGroupHeader()->max_size;
        hdr->frame_number = 0;
        hdr->gpi_level = 0;
        hdr->tv_sec = 0;
        hdr->tv_usec = 0;
        hdr->fourcc = 0;
        hdr->bpp = bpp;
        hdr->black_level = 0;
    }
}

ImageWriter::~ImageWriter()
{
    // release acquired semaphores.
    m_buffer->releaseWriteAccess( m_previousElement );
    m_buffer->releaseWriteAccess( m_element );
}

bool ImageWriter::setSize( size_t id, u_int32_t width, u_int32_t height, u_int32_t bpp )
{
    if( bpp % 8 != 0 )
    {
        std::cerr << "Image::setSize() - BPP not divisible by 8!" << std::endl;
        return false;
    }

    if( ImageStream::setSize( id, width*height*(bpp/8) ) )
    {
        image_header_t* header = getHeader( id );
        header->height = height;
        header->width = width;
        header->bpp = bpp;
        return true;
    }
    else
    {
        std::cerr << "Image::setSize() - image size exceeds allocated buffer size." << std::endl;
        return false;
    }
    return false;
}

void ImageWriter::copyWrite( size_t id, char* address )
{
    if( id < getGroupHeader()->count )
    {
        //memcpy( getHeader( id ), &m_headers.at( id ), sizeof( image_header_t ) );
        memcpy( getAddress( id ), address, getSize( id ) );
    }
}

void ImageWriter::update()
{
    m_previousElement = m_element;
    m_buffer->getWriteAccess( m_element );

    // Ensure that headers are unchanged after update
    memcpy( m_element.data, m_previousElement.data, sizeof( group_header_t ) );

    for( size_t i = 0; i < getGroupHeader()->count; i++ )
    {
        memcpy( getHeaderAddress( i), getHeaderAddressFromElement( m_previousElement.data, i ), sizeof( image_header_t ) );
    }

    m_buffer->releaseWriteAccess( m_previousElement );
}
