#include <sepia/network/server.h>
#include <csignal>

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
    server = new sepia::network::Server( "0.0.0.0", 31337 );
    server->start();
    std::signal(SIGINT, catch_int );
    server->join();
}
