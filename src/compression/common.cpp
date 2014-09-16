#include <sepia/compression/compress.h>

#define CHUNK 262144

namespace sepia
{
namespace compression
{
Common::Common()
{
    m_stream.zalloc = Z_NULL;
    m_stream.zfree = Z_NULL;
    m_stream.opaque = Z_NULL;
}

Common::~Common()
{

}

void Common::setInput( unsigned char *a_input, size_t a_size )
{
    m_stream.avail_in = a_size;
    m_stream.next_in = a_input;
}

void Common::setOutput( unsigned char *a_input, size_t a_maxSize )
{
    m_stream.avail_out = a_maxSize;
    m_stream.next_out = a_input;
}

}
}
