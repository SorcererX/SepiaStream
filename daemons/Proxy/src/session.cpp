#include "session.h"
#include "msgsocket.h"
#include <sepia/comm/dispatcher.h>
#include <sepia/comm/observer.h>

using sepia::network::TcpSocket;

Session::Session( int a_socket )
{
    m_socket = new MsgSocket( a_socket );
}

Session::~Session()
{
    //m_socket->close();
    delete m_socket;
}

void Session::start()
{
    m_thread = new std::thread( std::bind( &Session::own_thread, this ) );
}

void Session::receive( const sepia::comm::internal::ForwardSubscribe* a_message )
{
    m_messages.insert( a_message->message_name() );

    sepia::comm::internal::RemoteSubscription msg;

    msg.set_message_name( a_message->message_name() );

    m_header->set_message_name( msg.GetTypeName() );

    m_socket->sendMsg( m_header );
    m_socket->sendMsg( a_message );
}


void Session::receive(const sepia::comm::internal::ForwardUnsubscribe* a_message )
{
    MessageSet::iterator list_it = m_messages.find( a_message->message_name() );
    m_messages.erase( list_it );

    sepia::comm::internal::RemoteUnsubscription msg;

    msg.set_message_name( a_message->message_name() );

    m_header->set_message_name( msg.GetTypeName() );

    m_socket->sendMsg( m_header );
    m_socket->sendMsg( &msg );
}

void Session::receive( const sepia::comm::internal::RemoteSubscription* a_message )
{
    ObserverRaw::initRawReceiver( a_message->message_name() );
}

void Session::receive( const sepia::comm::internal::RemoteUnsubscription* a_message )
{
    ObserverRaw::destroyRawReceiver( a_message->message_name() );
}


void Session::receiveRaw( const sepia::comm::Header* a_header, const char* a_buffer, size_t a_size )
{
    m_socket->sendMsg( a_header );
    m_socket->sendMsg( a_buffer, a_size );
}

void Session::own_thread()
{
    sepia::comm::Observer< sepia::comm::internal::ForwardSubscribe >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::ForwardUnsubscribe >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::RemoteSubscription >::initReceiver();
    sepia::comm::Observer< sepia::comm::internal::RemoteUnsubscription >::initReceiver();

    while( !m_terminate )
    {
        sepia::comm::ObserverBase::threadReceiver();
    }
    sepia::comm::Observer< sepia::comm::internal::ForwardSubscribe >::destroyReceiver();
    sepia::comm::Observer< sepia::comm::internal::ForwardUnsubscribe >::destroyReceiver();
    sepia::comm::Observer< sepia::comm::internal::RemoteSubscription >::destroyReceiver();
    sepia::comm::Observer< sepia::comm::internal::RemoteUnsubscription >::destroyReceiver();
}
