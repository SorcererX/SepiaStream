#ifndef SEPIA_WRITER_H
#define SEPIA_WRITER_H
#include <sepia/stream.h>

namespace sepia
{
class Writer : public Stream
{
public:
    Writer( std::string name, u_int32_t images, u_int32_t width, u_int32_t height, u_int32_t bpp );
    ~Writer();
    bool setSize( size_t id, u_int32_t width, u_int32_t height, u_int32_t bpp );
    void copyWrite( size_t id, char* address );
    void update();

private:
    std::vector< image_header_t > m_headers;
    elem_t m_previousElement;
};
}

#endif // SEPIA_WRITER_H
