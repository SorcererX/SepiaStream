#ifndef SEPIA_UTIL_THREAD_BARRIER_H
#define SEPIA_UTIL_THREAD_BARRIER_H
#include <mutex>
#include <condition_variable>
#include <cstdint>

namespace sepia::util
{
class ThreadBarrier
{
public:
    explicit ThreadBarrier( std::size_t a_count ) : m_count( a_count ) {}
    void wait()
    {
        std::unique_lock< std::mutex > lock( m_mutex );
        auto generation = m_generation;
        if( !--m_count )
        {
            m_count = m_threshold;
            m_condvar.notify_all();
        }
        else
        {
            m_condvar.wait( lock, [this, generation]
            {
                return generation != m_generation;
            } );
        }
    }
private:
    std::mutex m_mutex;
    std::condition_variable m_condvar;
    std::size_t m_count;
    std::size_t m_generation;
    std::size_t m_threshold;
};
}

#endif // SEPIA_UTIL_THREAD_BARRIER_H
