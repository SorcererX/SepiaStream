#ifndef SEPIA_WRITER_H
#define SEPIA_WRITER_H
#include <string>
#include <sepia/stream.h>

namespace sepia
{
class Writer : public Stream
{
public:
    Writer( const std::string& name, uint32_t images, uint32_t width, uint32_t height, uint32_t bpp );
    ~Writer();
    bool setSize( std::size_t id, uint32_t width, uint32_t height, uint32_t bpp );
    void copyWrite( std::size_t id, char* address );
    void update();

private:
    std::vector< image_header_t > m_headers;
    elem_t m_previousElement;
};
}

#endif // SEPIA_WRITER_H
