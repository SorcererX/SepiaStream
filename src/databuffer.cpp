#include <sepia/databuffer.h>
#include <sepia/sharedmemory.h>
#include <sepia/semaphore.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>


namespace sepia
{
DataBuffer::DataBuffer( std::string a_name,
                        unsigned int count,
                        size_t elem_size ) : m_memoryPool( NULL )
                                           , m_name( a_name )
                                           , m_semaphore( NULL )
{
    m_created = sepia::SharedMemory::init( m_name,
                                              sizeof( DataBuffer::header_t ) + count*( elem_size + sizeof( info_t ) ),
                                              &m_fd );

    sepia::SharedMemory::mapWrite( m_fd, &m_memoryPool );

    if( m_created )
    {
        std::cout << "SHM: Created" << std::endl;
        m_header = new (m_memoryPool) DataBuffer::header_t();

        // determine random key.
        std::string path = std::string( "/dev/shm/" ) + a_name;

        m_header->sem_key = ftok( path.c_str(), 'S' );

        // storing properties
        m_header->current_index = 0;
        m_header->elements = count;
        m_header->elem_size = elem_size;

        elem_t element;
        for( unsigned int i = 0; i < m_header->elements; i++ )
        {
            get( i, element );
            element.info->complete = false;
            element.info->index = i;
            element.info->size = m_header->elem_size;
            std::cout << "element.info->index: " << element.info->index << std::endl;
        }

    }
    else
    {
        void* addr = static_cast<void*>( m_memoryPool );
        m_header = static_cast<header_t*>( addr );
        std::cout << "header elements: " << m_header->elements << std::endl;
    }
    m_semaphore = new sepia::Semaphore( m_header->sem_key, m_header->elements );
}

DataBuffer::DataBuffer( std::string a_name ) : m_memoryPool( NULL)
                                             , m_name( a_name )
                                             , m_semaphore( NULL )
                                             , m_created( false )
{
    while( !sepia::SharedMemory::open( m_name, &m_fd ) )
    {
        // open not successful, retrying in 100 ms.
        usleep( 100000 );
    }
    sepia::SharedMemory::mapWrite( m_fd, &m_memoryPool );

    void* addr = static_cast<void*>( m_memoryPool );
    m_header = static_cast<header_t*>( addr );
    m_semaphore = new sepia::Semaphore( m_header->sem_key, m_header->elements );
}

void DataBuffer::get( unsigned int index, elem_t& elem )
{
    if( index < m_header->elements )
    {
        void* addr = static_cast<void*>( m_memoryPool + sizeof( DataBuffer::header_t ) + index * ( m_header->elem_size + sizeof( info_t ) ) );
        elem.info = static_cast<info_t*>( addr );
        addr = static_cast<void*>( static_cast<char*>( addr ) + sizeof( info_t) );
        elem.data = static_cast<char*>( addr );
    }
    else
    {
        elem.info = NULL;
        elem.data = NULL;
    }
}


void DataBuffer::getReadAccess( elem_t& element )
{
    while( element.data == NULL )
    {

        unsigned int old_index = m_header->current_index;

        m_semaphore->wait( m_header->current_index );

        get( old_index, element );

        if( m_header->current_index == old_index )
        {
            //
            if( !element.info->complete )
            {
                std::cerr << "WARNING: The last writer released all semaphores but did not set complete flag." << std::endl;
                std::cerr << "         This is probably due to an application fault, waiting and retrying." << std::endl;
                releaseReadAccess( element );
                usleep( 1000000 );
            }
        }
    }
}

void DataBuffer::getWriteAccess( elem_t& element )
{
    // locate free buffer.

    for( unsigned int index = m_header->current_index + 1; index < m_header->elements + m_header->current_index; index++ )
    {
        unsigned int real_idx = index % m_header->elements;

        if( m_semaphore->getVal( real_idx ) == m_header->elements )
        {
            m_semaphore->waitAll( real_idx );
            m_header->current_index = real_idx;
            get( real_idx, element );
            element.info->complete = false;
            return ;
        }
        else
        {
            std::cout << "DEBUG: " << index << ", " << real_idx << ", " << m_semaphore->getVal( real_idx ) << std::endl;
        }
    }
    std::cout << "no free index found." << std::endl;

    // no free index found, picking next index and waiting (potentially).
    unsigned int index = ( m_header->current_index + 1 ) % m_header->elements;

    m_semaphore->waitAll( index );
    m_header->current_index = index;
    get( index, element );
    element.info->complete = false;

}

void DataBuffer::releaseWriteAccess( elem_t& element )
{
    if( element.info )
    {
        //std::cerr << "releaseWriteAccess() " << element.info->index << std::endl;
        m_semaphore->postAll( element.info->index );
    }
    element.data = NULL;
    element.info = NULL;
}

void DataBuffer::releaseReadAccess( elem_t& element )
{
    if( element.info )
    {
        m_semaphore->post( element.info->index );
    }
    element.data = NULL;
    element.info = NULL;
}

unsigned int DataBuffer::getElementSize()
{
    if( m_header )
    {
        return m_header->elem_size;
    }
    else
    {
        return 0;
    }
}

unsigned int DataBuffer::getElementCount()
{
    if( m_header )
    {
        return m_header->elements;
    }
    else
    {
        return 0;
    }
}

}
