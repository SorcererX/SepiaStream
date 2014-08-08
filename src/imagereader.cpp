#include "imagereader.h"
#include <sepia/databuffer.h>

ImageReader::ImageReader( std::string name ) : ImageStream( name )
{
    m_buffer->getReadAccess( m_element );
}

void ImageReader::update()
{
    m_buffer->releaseReadAccess( m_element );
    m_buffer->getReadAccess( m_element );
}
