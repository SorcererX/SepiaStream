#ifndef SEPIA_COMM_GLOBALRECEIVER_H
#define SEPIA_COMM_GLOBALRECEIVER_H
#include <vector>
#include <unordered_set>
#include <thread>
#include <sepia/util/threadobject.h>
#include <sepia/comm/observer.h>

namespace sepia
{
namespace comm
{
class MessageHandler;


class GlobalReceiver : public sepia::util::ThreadObject
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
    void stop();

protected:
    void own_thread();
    void heartbeat_thread();
    static void receiveData( char** a_buffer, size_t& remaining_bytes );
    static const Header* getLastHeader();


private:
    static MessageHandler* sm_messageHandler;
    static Header sm_header;
    static std::vector< char > sm_buffer;
    std::thread* m_heartBeatThread;
    static bool sm_isRouter;
    static size_t sm_lastBufferSize;
    static ObserverBase* sm_router;
    static std::string sm_name;
};
}
}
#endif // SEPIA_COMM_GLOBALRECEIVER_H
