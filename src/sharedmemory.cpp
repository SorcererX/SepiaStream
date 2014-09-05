#include <sepia/sharedmemory.h>
#include <cstdio>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <iostream>
#include <unistd.h>
#include <cstring>


namespace sepia {

bool SharedMemory::init( std::string& name, size_t bytes, int* fd )
{
    bool retval;

    *fd = shm_open( name.c_str(), O_CREAT | O_RDWR | O_EXCL, 0777 );

    if( *fd >= 0 )
    {
        retval = true;

        if (ftruncate( *fd, bytes) == -1)
        {
            std::cerr << "Unable to allocate " << bytes << std::endl;
            shm_unlink( name.c_str() );
            exit(1);
        }

    }
    else if( errno == EEXIST )
    {
        *fd = shm_open( name.c_str(), O_RDWR, 0777 );
        retval = false;
    }
    else
    {
       std::cerr << "ERROR: " << strerror( errno ) << std::endl;
       exit(1);
    }

    return retval;
}

bool SharedMemory::open( std::string& name, int* fd )
{
    *fd = shm_open( name.c_str(), O_RDWR, 0777 );
    if( *fd >= 0 )
    {
        return true;
    }
    return false;
}


void SharedMemory::mapWrite( int fd, char **address )
{
    /* Map memory object */

    struct stat desc;

    fstat( fd, &desc );

    *address = (char*) mmap(0, desc.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ( *address == (void *) -1) {
        std::cerr << "mmap failed : " << strerror(errno) << std::endl;
        exit(1);
    }
}

void SharedMemory::mapRead( int fd, char **address )
{
    /* Map memory object */

    struct stat desc;

    fstat( fd, &desc );


    *address = (char*) mmap(0, desc.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if ( *address == (void *) -1) {
        std::cerr << "mmap failed : " << strerror(errno) << std::endl;
        exit(1);
    }
}

void SharedMemory::unmap( int fd, void* address )
{
    struct stat desc;

    fstat( fd, &desc );

    munmap( address, desc.st_size );
}

void SharedMemory::close( std::string& name )
{
    shm_unlink( name.c_str() );
}


}