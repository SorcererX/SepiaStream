#include <sepia/reader.h>
#include <sepia/databuffer.h>
namespace sepia
{
Reader::Reader( std::string name ) : Stream( name )
{
    m_buffer->getReadAccess( m_element );
}

void Reader::update()
{
    m_buffer->releaseReadAccess( m_element );
    m_buffer->getReadAccess( m_element );
}
}
