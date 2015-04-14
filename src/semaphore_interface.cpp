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

#include <sepia/semaphore_interface.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <iostream>
#include <unistd.h>

namespace sepia
{

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

namespace semaphore_interface {

void init( key_t key, int nsems, int* semid ) // key from ftok.
{
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;

    *semid = semget( key, nsems, IPC_CREAT | IPC_EXCL | 0666 );

    if( *semid >= 0 )
    {
        std::cout << "SEM: CREATE" << std::endl;
        sb.sem_op = nsems;
        sb.sem_flg = 0;

        for( sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++ )
        {
            // do a semop() to "free" the semaphores.
            // this sets the sem_otime field as needed below.
            if( semop( *semid, &sb, 1 ) == -1 )
            {
                std::cerr << "ERROR." << std::endl;
                int e = errno;
                semctl( *semid, 0, IPC_RMID ); // clean up
                errno = e;
                return ; // check errno.
            }
            std::cout << sb.sem_num << ": " << getVal( *semid, sb.sem_num ) << std::endl;
        }
    }
    else if( errno == EEXIST ) // someone already got semaphore.
    {
        std::cout << "SEM: EXISTS" << std::endl;
        int ready = 0;

        *semid = semget( key, nsems, 0 ); // get id

        // wait for other process to initialize semaphore.
        arg.buf = &buf;

        for( i = 0; i < 10 && !ready; i++ )
        {
            semctl( *semid, nsems-1, IPC_STAT, arg );

            if( arg.buf->sem_otime != 0 )
            {
                ready = 1;
            }
            else
            {
                sleep( 1 ); // wait 1 second.
            }
        }

        if( !ready )
        {
            errno = ETIME;
            *semid = -1;
            return ;
        }
    }
}

void wait( int semid, int num, int count )
{
    struct sembuf sb;
    sb.sem_num = num;
    sb.sem_op = -count;  // set to allocate resource
    sb.sem_flg = SEM_UNDO;

    if( semop( semid, &sb, 1 ) == -1 )
    {
        std::cerr << "Semaphore::waitSem - " << num << " FAIL." << std::endl;
    }
}

void post( int semid, int num, int count )
{
    struct sembuf sb;
    sb.sem_num = num;
    sb.sem_op = count;  // free resource
    sb.sem_flg = SEM_UNDO;

    if( semop( semid, &sb, 1 ) == -1 )
    {
        std::cerr << "Semaphore::postSem - " << num << " FAIL." << std::endl;
    }
}

void setVal( int semid, int num, int val )
{
    union semun arg;
    arg.val = val;

    semctl( semid, num, SETVAL, arg );
}

int getVal( int semid, int num )
{
    return semctl( semid, num, GETVAL );
}

}

}
