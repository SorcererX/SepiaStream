#include <sepia/comm/scom.h>
#include <sepia/comm/globalreceiver.h>
#include <sepia/comm/dispatcher.h>
#include "../defs/receivetester.hpp"
#include "commtester_messages.pb.h"
#include <iostream>
#include <unistd.h>


int main()
{
    // initialize Sepia Client
    sepia::comm::init( "CommTester" );

    // start global receive thread (required to receive messages)
    // this will dispatch messages to all thread receivers.
    sepia::comm::GlobalReceiver receiver;
    receiver.start();

    std::vector< ReceiveTester > m_receivers;
    m_receivers.resize( 10 );

    for( int i = 0; i < m_receivers.size(); i++ )
    {
        m_receivers[i].start();
    }

    commtester_msgs::Test message;

    struct timeval time_value;
    usleep( 500000 );
    for( int i = 0; i < 1000; i++ )
    {
        message.set_seq_no( i );
        gettimeofday( &time_value, 0 );
        message.set_time( time_value.tv_sec );
        sepia::comm::Dispatcher< commtester_msgs::Test >::localSend( &message );
    }
    //usleep( 1000 );

    for( int i = 0; i < m_receivers.size(); i++ )
    {
        m_receivers[ i ].stop();
    }
    for( int i = 0; i < m_receivers.size(); i++ )
    {
        m_receivers[ i ].join();
    }

    for( int i = 0; i < m_receivers.size(); i++ )
    {
        std::cout << "i: " << m_receivers[ i ].getMessageCount() << std::endl;
    }
    receiver.stop();
    receiver.join();
    return 0;
}
