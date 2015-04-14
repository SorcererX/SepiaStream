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

#include <sepia/semaphore.h>
#include <sepia/semaphore_interface.h>

namespace sepia {

Semaphore::Semaphore( key_t key, int count )
{
    m_count = count;
    semaphore_interface::init( key, m_count, &m_semid );
}

Semaphore::Semaphore( int semid )
{
    m_semid = semid;
}

void Semaphore::post( int id )
{
    if( id < m_count )
    {
        semaphore_interface::post( m_semid, id, 1 );
    }
}

void Semaphore::postAll( int id )
{
    if( id < m_count )
    {
        semaphore_interface::post( m_semid, id, m_count );
    }
}

void Semaphore::wait( int id )
{
    if( id < m_count )
    {
        semaphore_interface::wait( m_semid, id, 1 );
    }
}

void Semaphore::waitAll( int id )
{
    if( id < m_count )
    {
        semaphore_interface::wait( m_semid, id, m_count );
    }
}

void Semaphore::setVal( int id, int value )
{
    if( id < m_count )
    {
        semaphore_interface::setVal( m_semid, id, value );
    }
}

int Semaphore::getVal( int number )
{
    return semaphore_interface::getVal( m_semid, number );
}

} // namespace sepia
