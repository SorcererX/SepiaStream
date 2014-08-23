#ifndef SEPIA_COMM_H
#define SEPIA_COMM_H
#include <sepia/comm/globalreceiver.h>
#include <sepia/comm/messagesender.h>

namespace sepia
{
namespace comm
{
   void init( const std::string& a_name )
   {
      MessageSender::initClient();
      GlobalReceiver::initClient( a_name );
   }

   void initRouter()
   {
      GlobalReceiver::initRouter();
   }

}
}
#endif // SEPIA_COMM_H
