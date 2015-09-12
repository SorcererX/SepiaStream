#include <sepia/comm/scom.h>
#include <sepia/comm/globalreceiver.h>
#include <sepia/comm/dispatcher.h>
#include "../defs/receivetester.hpp"
#include "commtester_messages.pb.h"
#include <iostream>
#include <chrono>


int main()
{
    // initialize Sepia Client
    sepia::comm::init( "CommTester" );

    // start global receive thread (required to receive messages)
    // this will dispatch messages to all thread receivers.
    sepia::comm::GlobalReceiver receiver;
    receiver.start();

    std::vector< ReceiveTester > m_receivers;
    m_receivers.resize( 20 );

    for( int i = 0; i < m_receivers.size(); i++ )
    {
        m_receivers[i].start();
    }

    commtester_msgs::Test message;

    auto now = std::chrono::system_clock::now();
    struct timeval time_value;

    std::cout << "Waiting 2 seconds after starting receivers before starting send..." << std::endl;
    usleep( 200000 );
    std::cout << "Starting send." << std::endl;
    for( int i = 0; i < 100000; i++ )
    {
        message.set_seq_no( i );
        now = std::chrono::system_clock::now();
        message.set_time( now.time_since_epoch().count() );
        sepia::comm::Dispatcher< commtester_msgs::Test >::localSend( &message );
    }
    std::cout << "Send completed." << std::endl;
    std::cout << "Waiting 1 second after sending last message before closing receivers.." << std::endl;
    usleep( 1000000 );
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
