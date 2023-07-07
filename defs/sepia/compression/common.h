#ifndef SEPIA_COMPRESSION_COMMON_H
#define SEPIA_COMPRESSION_COMMON_H
#include <cstdint>
#include <zlib.h> // see http://www.zlib.net/

namespace sepia
{
namespace compression
{

class Common
{
public:
    enum class Method {
        ZLIB,
        ZLIB_RLE_ONLY,
        ZLIB_HUFFMAN_ONLY,
        FFV1,
        HUFFYUV
    };

    void setInput( unsigned char* a_input, std::size_t a_size );
    void setOutput( unsigned char* a_input, std::size_t a_maxSize );
protected:
    Common();
    ~Common();
    z_stream m_stream;
    Method m_method;
    int m_level;
};

}
}

#endif // SEPIA_COMPRESSION_COMMON_H
