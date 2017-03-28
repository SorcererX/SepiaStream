#ifndef SEPIA_COMM2_GLOBALRECEIVER_H
#define SEPIA_COMM2_GLOBALRECEIVER_H
#include <sepia/util/threadobject.h>
#include <zmq.hpp>

namespace sepia
{
namespace comm2
{

class GlobalReceiver : sepia::util::ThreadObject
{
public:
    GlobalReceiver();
    ~GlobalReceiver();
    void start();
    void stop();

protected:
    void own_thread();
    void internal_thread();

private:
    zmq::context_t m_context;
    zmq::socket_t  m_externalSocket;
    zmq::socket_t  m_internalSocket;
};


}
}

#endif // SEPIA_COMM2_GLOBALRECEIVER_H
