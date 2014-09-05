#include <sepia/network/server.h>
#include <csignal>
#include <iostream>
#include <string>

sig_atomic_t terminate = 0;
sepia::network::Server* server = NULL;


void catch_int( int ){
    if( server != NULL )
    {
        server->stop();
    }
}


int main( int argc, char** argv )
{
    int port = 31337;
    if( argc > 1 )
    {
        port = std::stoi( std::string( argv[ 1 ] ) );
    }
    std::cout << "Listening on: " << "0.0.0.0" << ":" << port << std::endl;
    server = new sepia::network::Server( "0.0.0.0", port );
    server->start();
    std::signal(SIGINT, catch_int );
    server->join();
}
