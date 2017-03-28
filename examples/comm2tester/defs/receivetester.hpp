#include <sepia/comm2/observer.h>
#include <sepia/util/threadobject.h>
#include "comm2tester_msgs_generated.h"

class ReceiveTester : public sepia::comm2::Observer< comm2tester_msgs::TestT >
                    , public sepia::util::ThreadObject
{
public:
    ReceiveTester();
    ~ReceiveTester();
    ReceiveTester( const ReceiveTester& a_object );
    void start();
    void stop();
    int getMessageCount();

protected:
    void receive( const std::unique_ptr< comm2tester_msgs::TestT >& a_msg );
    void own_thread();
    int m_messageCount;
};
