#ifndef SEMAPHORE_INTERFACE_H
#define SEMAPHORE_INTERFACE_H
#include <cstdlib>

namespace sepia
{
    namespace semaphore_interface
    {
        void init( key_t key, int count, int* semid );
        void post( int semid, int num, int count = 0 );
        void wait( int semid, int num, int count = 0 );
        void setVal( int semid, int num, int val );
        int getVal( int semid, int num );
    }
}

#endif // SEMAPHORE_INTERFACE_H
