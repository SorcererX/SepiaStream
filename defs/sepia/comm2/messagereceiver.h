#ifndef SEPIA_COMM2_MESSAGERECEIVER_H
#define SEPIA_COMM2_MESSAGERECEIVER_H

#include <string>

namespace sepia
{
namespace comm2
{

class MessageReceiver
{
public:
    static MessageReceiver* instance();

protected:
    MessageReceiver();
    ~MessageReceiver();

private:
    static MessageReceiver* sm_instance;
};

}
}

#endif // SEPIA_COMM2_MESSAGERECEIVER_H
