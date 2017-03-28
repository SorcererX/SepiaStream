#include <sepia/comm2/messagereceiver.h>

namespace sepia
{
namespace comm2
{

MessageReceiver* MessageReceiver::sm_instance = NULL;

MessageReceiver* MessageReceiver::instance()
{
    if( !sm_instance )
    {
        sm_instance = new MessageReceiver();
    }
    return sm_instance;
}

MessageReceiver::MessageReceiver()
{
}

MessageReceiver::~MessageReceiver()
{
}

}
}
