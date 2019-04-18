#ifndef SEPIA_COMM2_INTERNALS_H
#define SEPIA_COMM2_INTERNALS_H
#include <zmq.hpp>
#include <string>

namespace sepia
{
namespace comm2
{
class Internals
{
public:
    static zmq::context_t sm_context;
    static std::string sm_externalPub;
    static std::string sm_externalSub;
    static std::string sm_internalPub;
    static std::string sm_internalSub;
    static std::string sm_externalRequest;
};
}
}

#endif // SEPIA_COMM2_INTERNALS_H
