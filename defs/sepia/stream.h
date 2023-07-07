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
        uint32_t count;
        uint32_t max_size;
    };

    struct image_header_t
    {
        uint32_t size;
        uint32_t fourcc;
        uint32_t width;
        uint32_t height;
        uint32_t frame_number;
        uint32_t frame_counter;
        uint32_t tv_sec;
        uint32_t tv_usec;
        uint32_t gpi_level;
        uint32_t black_level;
        uint32_t bpp;
        uint32_t channels;
    };

    char* getAddress( std::size_t id );
    std::size_t getSize( std::size_t id );
    std::size_t getMaxSize();
    image_header_t* getHeader( std::size_t id );
    group_header_t* getGroupHeader();

protected:
    Stream( std::string name );
    Stream( std::string name, uint32_t images, uint32_t width, uint32_t height, uint32_t bpp );
    bool setSize( std::size_t id, uint32_t size );
    char* getHeaderAddressFromElement( char* element, std::size_t id );
    char* getHeaderAddress( std::size_t id );
    elem_t m_element;
    DataBuffer* m_buffer;
private:
    group_header_t* getGroupHeaderFromElement( char* element );
};
}
#endif // SEPIA_STREAM_H
