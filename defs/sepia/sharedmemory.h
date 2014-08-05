#ifndef SEPIA_SHAREDMEMORY_H
#define SEPIA_SHAREDMEMORY_H
#include <cstdlib>
#include <string>

namespace sepia {

class SharedMemory
{
public:
    static bool init( std::string& name, size_t bytes, int* fd );
    static bool open( std::string& name, int* fd );
    static void mapWrite( int fd, char **address );
    static void mapRead(  int fd, char **address );
    static void unmap( int fd, void* address );
    static void close( std::string& name );
private:
    SharedMemory() = delete;
};

}
#endif // SEPIA_SHAREDMEMORY_H
