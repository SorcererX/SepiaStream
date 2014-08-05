#ifndef IMAGEREADER_H
#define IMAGEREADER_H
#include <imagestream.h>

class ImageReader : public ImageStream
{
public:
    ImageReader( std::string name );
    void update();
};

#endif // IMAGEREADER_H
