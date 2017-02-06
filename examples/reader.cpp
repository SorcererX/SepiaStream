#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <sepia/reader.h>

int main( int argc, char** argv )
{
    sepia::Reader reader( "/image" );

    while( true )
    {
        for( int i = 0; i < reader.getGroupHeader()->count; i++ )
        {
            std::cout << "image: " << i << std::flush;
            int64_t sum = 0;
            const int width = reader.getHeader(i)->width;
            const int height = reader.getHeader(i)->height;

            unsigned char* array = reinterpret_cast< unsigned char* >( reader.getAddress( i ) );

            for( int y = 0; y < height; y++ )
            {
                for( int x = 0; x < width; x++ )
                {
                    sum += array[ y * width + x ];
                }
            }
            std::cout << " sum: " << sum << std::endl;
        }
        reader.update();
    }
}
