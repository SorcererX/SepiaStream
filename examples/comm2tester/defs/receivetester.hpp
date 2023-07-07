#include <sepia/comm2/observer.h>
#include <sepia/util/threadobject.h>
#include "comm2tester_msgs_generated.h"
#include "comm2tester_msgs.pb.h"

class ReceiveTester : public sepia::comm2::Observer< comm2tester_msgs::TestT >
                    , public sepia::comm2::Observer< comm2tester_msgs::ProtoTest >
                    , public sepia::util::ThreadObject
{
public:
    ReceiveTester();
    ~ReceiveTester();
    ReceiveTester( const ReceiveTester& a_object );
    void stop();
    int64_t getMessageCount();

protected:
    void receive( const std::unique_ptr< comm2tester_msgs::TestT >& a_msg );
    void receive( const std::unique_ptr< comm2tester_msgs::ProtoTest >& a_msg );
    void own_thread();
    int64_t m_flatbufferMessageCount;
    int64_t m_protobufMessageCount;
    int64_t m_lastFlatbufferSeqNo;
    int64_t m_lastProtobufSeqNo;
};
