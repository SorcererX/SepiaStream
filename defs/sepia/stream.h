#ifndef SEPIA_STREAM_H
#define SEPIA_STREAM_H
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sepia/databuffer.h>

namespace sepia
{
class Stream
{
public:
    struct group_header_t
    {
        u_int32_t count;
        u_int32_t max_size;
    };

    struct image_header_t
    {
        u_int32_t size;
        u_int32_t fourcc;
        u_int32_t width;
        u_int32_t height;
        u_int32_t frame_number;
        u_int32_t tv_sec;
        u_int32_t tv_usec;
        u_int32_t gpi_level;
        u_int32_t black_level;
        u_int32_t bpp;
        u_int32_t channels;
    };

    char* getAddress( size_t id );
    size_t getSize( size_t id );
    size_t getMaxSize();
    image_header_t* getHeader( size_t id );
    group_header_t* getGroupHeader();

protected:
    Stream( std::string name );
    Stream( std::string name, u_int32_t images, u_int32_t width, u_int32_t height, u_int32_t bpp );
    bool setSize( size_t id, u_int32_t size );
    char* getHeaderAddressFromElement( char* element, size_t id );
    char* getHeaderAddress( size_t id );
    elem_t m_element;
    DataBuffer* m_buffer;
private:
    group_header_t* getGroupHeaderFromElement( char* element );
};
}
#endif // SEPIA_STREAM_H
