/*
Copyright (c) 2012-2015, Kai Hugo Hustoft Endresen <kai.endresen@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <processmonitor.h>
#include <sepia/comm/dispatcher.h>
#include "internal.pb.h"
#include <functional>

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
                //m_mutex.unlock();
                sepia::comm::Dispatcher< sepia::comm::internal::ProcessDied >::localSend( &msg );
                break;
            }
        }
        m_mutex.unlock();
        usleep( 500000 ); // 500 ms
    }
}
