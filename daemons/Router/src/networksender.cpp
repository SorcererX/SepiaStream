#include "networksender.h"

/*
NetworkSender::NetworkSender( const NetworkSender& a_object )
{

}
*/

NetworkSender::NetworkSender()
{
}

NetworkSender::~NetworkSender()
{

}

void NetworkSender::start()
{
    m_thread = new std::thread( std::bind( &NetworkSender::own_thread, this ) );
}

void NetworkSender::receive( const sepia::comm::internal::ForwardSubscribe* a_message )
{
    m_messages.insert( a_message->message_name() );
    // send subscription message across to other unit!
}


void NetworkSender::receive(const sepia::comm::internal::ForwardUnsubscribe* a_message )
{
     MessageSet::iterator list_it = m_messages.find( a_message->message_name() );
     m_messages.erase( list_it );
     // send unsubscription message across to other unit!
}

void NetworkSender::receive( const sepia::comm::internal::RemoteSubscription* a_message )
{
    // add to routing.
    m_remoteSubscriptions.insert( a_message->message_name() );

}

void NetworkSender::receive( const sepia::comm::internal::RemoteUnsubscription* a_message )
{
    // remove from routing.
    MessageSet::iterator list_it = m_remoteSubscriptions.find( a_message->message_name() );
    m_remoteSubscriptions.erase( list_it );
}


void NetworkSender::receiveRaw( const sepia::comm::Header* a_header, const char* a_buffer, size_t a_size )
{
    // check current incoming subscriptions.
    // send message across to other unit!
}

void NetworkSender::own_thread()
{
    sepia::comm::Observer< sepia::comm::internal::ForwardSubscribe >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::ForwardUnsubscribe >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::RemoteSubscription >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::RemoteUnsubscription >::initReceiver();
}
