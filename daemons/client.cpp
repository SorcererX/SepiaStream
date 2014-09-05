#include <sepia/network/client.h>
#include <csignal>
#include <iostream>

sig_atomic_t terminate = 0;
sepia::network::Client* client = NULL;


void catch_int( int ){
    std::signal( SIGINT, catch_int );
    if( client != NULL )
    {
        client->stop();
    }
}


int main( int argc, char** argv )
{
    if( argc < 3 )
    {
        std::cerr << "Arguments: ./client <HOST> <PORT> <NAME>" << std::endl;
        return -1;
    }
    else
    {
        std::string host = argv[ 1 ];
        int port = std::stoi( argv[ 2 ] );
        std::string name = argv[ 3 ];

        client = new sepia::network::Client( host, port, name );
        client->start();
        std::signal(SIGINT, catch_int );
        client->join();
    }
}
