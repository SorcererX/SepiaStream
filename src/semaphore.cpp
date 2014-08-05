#include <sepia/semaphore.h>
#include <sepia/semaphore_interface.h>

namespace sepia {

Semaphore::Semaphore( key_t key, int count )
{
    m_count = count;
    semaphore_interface::init( key, m_count, &m_semid );
}

Semaphore::Semaphore( int semid )
{
    m_semid = semid;
}

void Semaphore::post( int id )
{
    if( id < m_count )
    {
        semaphore_interface::post( m_semid, id, 1 );
    }
}

void Semaphore::postAll( int id )
{
    if( id < m_count )
    {
        semaphore_interface::post( m_semid, id, m_count );
    }
}

void Semaphore::wait( int id )
{
    if( id < m_count )
    {
        semaphore_interface::wait( m_semid, id, 1 );
    }
}

void Semaphore::waitAll( int id )
{
    if( id < m_count )
    {
        semaphore_interface::wait( m_semid, id, m_count );
    }
}

void Semaphore::setVal( int id, int value )
{
    if( id < m_count )
    {
        semaphore_interface::setVal( m_semid, id, value );
    }
}

int Semaphore::getVal( int number )
{
    return semaphore_interface::getVal( m_semid, number );
}

} // namespace sepia
