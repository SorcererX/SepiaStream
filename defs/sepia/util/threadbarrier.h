#ifndef SEPIA_UTIL_THREAD_BARRIER_H
#define SEPIA_UTIL_THREAD_BARRIER_H
#include <mutex>
#include <condition_variable>

namespace sepia
{
namespace util
{
class ThreadBarrier
{
public:
    explicit ThreadBarrier( size_t a_count ) : m_count( a_count ) {}
    void wait()
    {
        std::unique_lock< std::mutex > lock( m_mutex );
        if( --m_count == 0 )
        {
            m_condvar.notify_all();
        }
        else
        {
            m_condvar.wait( lock, [this] );
        }
    }
private:
    std::mutex m_mutex;
    std::condition_variable m_condvar;
    size_t m_count;
};

}
}

#endif // SEPIA_UTIL_THREAD_BARRIER_H
