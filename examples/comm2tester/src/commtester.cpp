#include <sepia/comm2/globalreceiver.h>
#include <sepia/comm2/dispatcher.h>
#include "../defs/receivetester.hpp"
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <messagesender.h>

int main( int argc, char** argv )
{
    int message_count = 0;
    int receive_count = 0;
    double message_delay = 0;
    int mode = 0;
    if( argc == 5 )
    {
        message_count = std::stoi( argv[1] );
        receive_count = std::stoi( argv[2] );
        message_delay = std::stod( argv[3] );
        mode = std::stoi( argv[4] );
    }
    else
    {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << "<message count> <receive count> <message delay> <mode>" << std::endl;
        std::cout << "where mode is: " << std::endl;
        std::cout << "0 - local send" << std::endl;
        std::cout << "1 - global send" << std::endl;
        std::cout << "2 - only receive" << std::endl;
        return 1;
    }

    // start global receive thread (required to receive messages)
    // this will dispatch messages to all thread receivers.
    sepia::comm2::GlobalReceiver receiver;
    receiver.start();

    sepia::comm2::MessageSender::init();
    std::vector< ReceiveTester > m_receivers;
    m_receivers.resize( receive_count );

    for( int i = 0; i < m_receivers.size(); i++ )
    {
        m_receivers[i].start();
    }

    if( mode == 0 || mode == 1 )
    {
        comm2tester_msgs::TestT message;

        auto now = std::chrono::system_clock::now();
        struct timeval time_value;

        std::cout << "Waiting 200 ms after starting receivers before starting send..." << std::endl;
        usleep( 200000 );
        std::cout << "Starting send." << std::endl;
        for( int i = 0; i < message_count; i++ )
        {
            message.seq_no = i;
            now = std::chrono::system_clock::now();
            message.time = now.time_since_epoch().count();
            if( mode == 0 )
            {
                sepia::comm2::Dispatcher< comm2tester_msgs::TestT >::localSend( &message );
            }
            else
            {
                sepia::comm2::Dispatcher< comm2tester_msgs::TestT >::send( &message );
            }
            if( message_delay > 0 )
            {
                usleep( 1000 * message_delay );
            }
        }
        std::cout << "Send completed." << std::endl;
    }
    std::cout << "Waiting to receive messages..." << std::endl;

    bool got_messages = false;
    while( !got_messages )
    {
        usleep( 10000 );
        got_messages = true;
        for( int i = 0; i < m_receivers.size(); i++ )
        {
            if(  m_receivers[ i ].getMessageCount() != message_count  )
            {
                got_messages = false;
            }
        }
    }

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
        std::cout << i << ": " <<  m_receivers[ i ].getMessageCount() << std::endl;
    }


//    receiver.stop();
//    receiver.join();
    std::cout << "all done." << std::endl;
    return 0;
}
