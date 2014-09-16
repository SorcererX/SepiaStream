#include <sepia/compression/compress.h>

namespace sepia
{
namespace compression
{

Compress::Compress( Common::Method a_method, int a_level )
{
    m_method = a_method;
    m_level = a_level;
    if( deflateInit( &m_stream, m_level ) != Z_OK )
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
    size_t prev_size = m_stream.avail_out;
    deflate( &m_stream, Z_NO_FLUSH );
    size_t wrote = m_stream.next_out - start;
    m_stream.next_out = start;
    m_stream.avail_out = prev_size;
    return wrote;
}

}
}
