#include "threadobject.h"


namespace sepia
{
namespace util
{
ThreadObject::ThreadObject()
{
    m_terminate = false;
}

void ThreadObject::join()
{
    if( m_thread )
    {
        m_thread->join();
    }
}

void ThreadObject::stop()
{
    m_terminate = true;
}

}
}
