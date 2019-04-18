#include <sepia/comm2/internals.h>

namespace sepia
{
namespace comm2
{
zmq::context_t Internals::sm_context( 4 );
std::string Internals::sm_externalPub( "tcp://localhost:31340" );
std::string Internals::sm_externalSub( "tcp://localhost:31339" );
std::string Internals::sm_internalPub( "inproc://internal_pubsub" );
std::string Internals::sm_internalSub( "inproc://internal_pubsub" );
std::string Internals::sm_externalRequest( "tcp://localhost:31341" );
}
}
