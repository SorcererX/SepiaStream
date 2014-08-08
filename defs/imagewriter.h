#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H
#include <imagestream.h>

class ImageWriter : public ImageStream
{
public:
    ImageWriter( std::string name, u_int32_t images, u_int32_t width, u_int32_t height, u_int32_t bpp );
    ~ImageWriter();
    bool setSize( size_t id, u_int32_t width, u_int32_t height, u_int32_t bpp );
    void copyWrite( size_t id, char* address );
    void update();

private:
    std::vector< image_header_t > m_headers;
    sepia::elem_t m_previousElement;
};

#endif // IMAGEWRITER_H
