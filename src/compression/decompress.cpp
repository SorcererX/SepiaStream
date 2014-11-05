#include <sepia/compression/decompress.h>

namespace sepia
{
namespace compression
{

Decompress::Decompress( Common::Method a_method )
{
    m_method = a_method;
    if( inflateInit( &m_stream) != Z_OK )
    {
        throw ;
    }
}

Decompress::~Decompress()
{
    inflateEnd( &m_stream );
}

size_t Decompress::perform()
{
    inflate( &m_stream, Z_FINISH );
    return 0; // TODO: FIX
}

}
}
