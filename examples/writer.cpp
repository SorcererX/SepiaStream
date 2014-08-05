#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <imagewriter.h>

int main( int argc, char** argv )
{
    std::vector< char > image;
    image.resize( 1280 * 1024 );
    image.reserve( 1280 * 1024 );
    for( size_t i = 0; i < image.size(); i++ )
    {
        image[i] = i % 255;
    }
    ImageWriter writer( "/image", 2, 1280, 1024, 8 );
    while( true )
    {
        std::cout << "." << std::flush;
        writer.copyWrite( 0, image.data() );
        writer.copyWrite( 1, image.data() );
        writer.update();
    }
}
