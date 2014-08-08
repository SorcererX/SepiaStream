#ifndef SEPIA_DATABUFFER_H
#define SEPIA_DATABUFFER_H
#include <vector>
#include <cstdlib>
#include <string>

namespace sepia
{
typedef struct {
    unsigned int index;
    unsigned int size;
    bool complete;
} info_t;

class Semaphore;
typedef struct {
    info_t* info;
    char* data;
} elem_t;

class DataBuffer
{
public:

    typedef struct {
        key_t sem_key;
        unsigned int current_index;
        unsigned int elements;
        size_t elem_size;
    } header_t;

    DataBuffer( std::string a_name );
    DataBuffer( std::string a_name, unsigned int count, size_t elem_size );
    void getReadAccess( elem_t& element );
    void getWriteAccess( elem_t& element );
    void releaseWriteAccess( elem_t& element );
    void releaseReadAccess( elem_t& element );

    unsigned int getElementSize();
    unsigned int getElementCount();

protected:
    void get( unsigned int index, elem_t& elem );

private:
    char* m_memoryPool;
    std::string m_name;
    DataBuffer::header_t* m_header;
    sepia::Semaphore* m_semaphore;
    int m_fd;
    bool m_created;
};

}
#endif // SEPIA_DATABUFFER_H
