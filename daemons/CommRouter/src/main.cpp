#include <iostream>
#include <zmq.hpp>

int main()
{
    zmq::context_t context( 2 );
    zmq::socket_t publisher( context, ZMQ_XPUB );
    zmq::socket_t subscriber( context, ZMQ_XSUB );

    publisher.bind( "tcp://*:31339" );
    subscriber.bind( "tcp://*:31340" );

    zmq::proxy( publisher, subscriber );

    return 0;
}
