#ifndef SEPIA_COMM_GLOBALRECEIVER_H
#define SEPIA_COMM_GLOBALRECEIVER_H
#include <vector>
#include <unordered_set>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <messages.pb.h>
#include <sepia/comm/observer.h>

namespace sepia
{
namespace comm
{
class MessageHandler;


class GlobalReceiver
{
public:
    GlobalReceiver();
    static void initClient( const std::string& a_name );
    static void initRouter();
    static bool isRouter();
    static void setRouter( ObserverBase* a_observer );
    static const char* getLastMessageBuffer(); // should probably be hidden somehow.
    static size_t getLastMessageBufferSize();
    void start();

protected:
    void own_thread();
    static void receiveData( char** a_buffer, size_t& remaining_bytes );
    static const cuttlefish_msgs::Header* getLastHeader();


private:
    static MessageHandler* sm_messageHandler;
    static cuttlefish_msgs::Header sm_header;
    static std::vector< char > sm_buffer;
    static bool sm_isRouter;
    static size_t sm_lastBufferSize;
    static ObserverBase* sm_router;
    boost::thread* m_thread;
};
}
}
#endif // SEPIA_COMM_GLOBALRECEIVER_H
