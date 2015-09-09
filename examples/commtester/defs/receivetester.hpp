#include <sepia/comm/observer.h>
#include <sepia/util/threadobject.h>
#include "commtester_messages.pb.h"

class ReceiveTester : public sepia::comm::Observer< commtester_msgs::Test >
        , public sepia::util::ThreadObject
{
public:
    ReceiveTester();
    ~ReceiveTester();
    ReceiveTester( const ReceiveTester& a_object );
    void start();
    int getMessageCount();

protected:
    void receive( const commtester_msgs::Test *msg );
    void own_thread();
    int m_messageCount;
};
