#ifndef SEPIA_SEMAPHORE_H
#define SEPIA_SEMAPHORE_H
#include <cstdlib>

namespace sepia {

class Semaphore
{
public:
    Semaphore( key_t key, int count );
    Semaphore( int shmid );
    void postAll( int number );
    void waitAll( int number );
    void post( int number );
    void wait( int number );
    int getVal( int number );
    void setVal( int number, int value );
private:
    int m_count;
    int m_semid;
};

} // namespace sepia

#endif // SEPIA_SEMAPHORE_H
