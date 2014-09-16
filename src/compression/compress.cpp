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

void Compress::perform()
{
    deflate( &m_stream, Z_FINISH );
}

}
}
