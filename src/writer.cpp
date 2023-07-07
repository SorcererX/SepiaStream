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

#include <sepia/writer.h>
#include <iostream>
#include <cstring>
#include <string>

namespace sepia
{
Writer::Writer( const std::string& name, uint32_t images, uint32_t width, uint32_t height, uint32_t bpp )
    : Stream( name, images, width, height, bpp )
{
    m_previousElement.data = NULL;
    m_previousElement.info = NULL;
    m_buffer->getWriteAccess( m_element );

    // Fill element with valid data.
    getGroupHeader()->max_size = width*height*bpp/8;
    getGroupHeader()->count = images;

    for( std::size_t i = 0; i < getGroupHeader()->count; i++ )
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

Writer::~Writer()
{
    // release acquired semaphores.
    m_buffer->releaseWriteAccess( m_previousElement );
    m_buffer->releaseWriteAccess( m_element );
}

bool Writer::setSize( std::size_t id, uint32_t width, uint32_t height, uint32_t bpp )
{
    if( bpp % 8 != 0 )
    {
        std::cerr << "Image::setSize() - BPP not divisible by 8!" << std::endl;
        return false;
    }

    if( Stream::setSize( id, width*height*(bpp/8) ) )
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

void Writer::copyWrite( std::size_t id, char* address )
{
    if( id < getGroupHeader()->count )
    {
        //memcpy( getHeader( id ), &m_headers.at( id ), sizeof( image_header_t ) );
        memcpy( getAddress( id ), address, getSize( id ) );
    }
}

void Writer::update()
{
    m_previousElement = m_element;
    m_buffer->getWriteAccess( m_element );

    // Ensure that headers are unchanged after update
    memcpy( m_element.data, m_previousElement.data, sizeof( group_header_t ) );

    for( std::size_t i = 0; i < getGroupHeader()->count; i++ )
    {
        memcpy( getHeaderAddress( i), getHeaderAddressFromElement( m_previousElement.data, i ), sizeof( image_header_t ) );
    }

    m_buffer->releaseWriteAccess( m_previousElement );
}
}
