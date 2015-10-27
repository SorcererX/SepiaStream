#include <proxyclient.h>
#include <proxyserver.h>
#include <iostream>

int main( int argc, char** argv )
{
    if( argc == 3 )
    {
        // got ./Proxy <hostname> <port>
        std::string hostname = argv[1];
        int port = std::stoi( argv[2] );
        std::cout << "Connecting client to: " << hostname << " using port: " << port << std::endl;
        ProxyClient client( std::string( argv[1] ), std::stoi( argv[2] ) );
    }
    else if( argc == 2 )
    {
        int port = std::stoi( argv[1] );
        std::cout << "Starting server listening on port: " << port << std::endl;
        ProxyServer server( port );
        server.start();
        server.join();
    }
    else if( argc >= 1 )
    {
        std::cout << "Server usage: " << argv[0] << " <port>" << std::endl;
        std::cout << "Client usage: " << argv[0] << " <hostname> <port>" << std::endl;
    }
    else
    {
        std::cerr << "USAGE_ERROR" << std::endl;
    }
    return 0;
}
