#ifndef SEPIA_UTIL_THREADOBJECT_H
#define SEPIA_UTIL_THREADOBJECT_H
#include <atomic>
#include <thread>

namespace sepia
{
namespace util
{
    class ThreadObject
    {
    public:
        ThreadObject();
        void join();
        void stop();
        virtual void start() = 0;

    protected:
        std::atomic_bool m_terminate;
        std::thread* m_thread;
    };
}
}
#endif // SEPIA_UTIL_THREADOBJECT_H
