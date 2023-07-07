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

#include <sepia/compression/compress.h>

namespace sepia
{
namespace compression
{

Compress::Compress( Common::Method a_method, int a_level )
{
    m_method = a_method;
    m_level = a_level;
    if( deflateInit2( &m_stream, m_level, Z_DEFLATED, 15, 9, Z_HUFFMAN_ONLY ) != Z_OK )
    {
        throw ;
    }
}

Compress::~Compress()
{
    deflateEnd( &m_stream );
}

size_t Compress::perform()
{
    unsigned char* start = m_stream.next_out;
    std::size_t prev_size = m_stream.avail_out;
    deflate( &m_stream, Z_NO_FLUSH );
    std::size_t wrote = m_stream.next_out - start;
    m_stream.next_out = start;
    m_stream.avail_out = prev_size;
    return wrote;
}

}
}
