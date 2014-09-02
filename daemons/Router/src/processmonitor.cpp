#include "processmonitor.h"
#include <sepia/comm/dispatcher.h>
#include "internal.pb.h"


// for kill
#include <sys/types.h>
#include <signal.h>
#include <mutex>


ProcessMonitor::ProcessMonitor()
{
}

void ProcessMonitor::add( unsigned int pid )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_pids.insert( pid );
}

void ProcessMonitor::remove( unsigned int pid )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    std::unordered_set< unsigned int >::iterator it = m_pids.find( pid );
    m_pids.erase( it );
}

void ProcessMonitor::start()
{
    m_thread = new std::thread( std::bind( &ProcessMonitor::own_thread, this ) );
}

void ProcessMonitor::own_thread()
{
    sepia::comm::internal::ProcessDied msg;

    while( !m_terminate )
    {
        std::unordered_set< unsigned int >::iterator it;
        m_mutex.lock();
        for( it = m_pids.begin(); it != m_pids.end(); ++it )
        {
            int retval = kill( *it, 0 );
            if( retval != 0 )
            {
                msg.set_pid( *it );
                m_mutex.unlock();
                sepia::comm::Dispatcher< sepia::comm::internal::ProcessDied >::localSend( &msg );
                break;
            }
        }
        m_mutex.unlock();
        usleep( 500000 ); // 500 ms
    }
}
