#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H
#include <sepia/util/threadobject.h>
#include <unordered_set>
#include <mutex>

class ProcessMonitor : public sepia::util::ThreadObject
{
public:
    ProcessMonitor();
    void add( unsigned int pid );
    void remove( unsigned int pid );

protected:
    void own_thread();

private:
    std::unordered_set< unsigned int > m_pids;
    std::mutex m_mutex;
};

#endif // PROCESSMONITOR_H
