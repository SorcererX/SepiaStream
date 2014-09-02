#include <sepia/network/client.h>
#include <csignal>

sig_atomic_t terminate = 0;
sepia::network::Client* client = NULL;


void catch_int( int ){
    if( client != NULL )
    {
        client->stop();
    }
}


int main( int argc, char** argv )
{
    client = new sepia::network::Client( "84.212.193.194", 31337, "TEST" );
    client->start();
    std::signal(SIGINT, catch_int );
    client->join();
}
