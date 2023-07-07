#include "session.h"
#include "msgsocket.h"
#include <sepia/comm/dispatcher.h>
#include <sepia/comm/observer.h>
#include <functional>

using sepia::network::TcpSocket;

Session::Session( int a_socket )
{
    m_socket = new MsgSocket( a_socket );
    init();
}

Session::Session( sepia::network::TcpSocket* a_socket )
{
    m_socket = new MsgSocket( a_socket );
    init();
}

void Session::init()
{
    m_recvBuffer.reserve( 1024 );
    m_recvBuffer.resize( 1024 );
    m_tcpReceiverThread = NULL;
    m_header = new sepia::comm::Header();
    m_header->set_source_node( getpid() );
    m_header->set_source_router( 0 );
}

Session::~Session()
{
    //m_socket->close();
    delete m_socket;
}

void Session::start()
{
    sepia::util::ThreadObject::start();
    m_tcpReceiverThread = new std::thread( std::bind( &Session::tcpreceiver_thread, this ) );
}

void Session::receive( const sepia::comm::internal::ForwardSubscribe* a_message )
{
    m_messages.insert( a_message->message_name() );

    sepia::comm::internal::RemoteSubscription msg;

    msg.set_message_name( a_message->message_name() );

    m_header->set_message_name( msg.GetTypeName() );

    std::cout << msg.GetTypeName() << " TCPsend: " << msg.ShortDebugString() << std::endl;
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

    std::cout << msg.GetTypeName() << " TCPsend: " << msg.ShortDebugString() << std::endl;
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


void Session::receiveRaw( const sepia::comm::Header* a_header, const char* a_buffer, std::size_t a_size )
{
    std::cout << a_header->message_name() << " RawSend: <NOT DECODED>" << std::endl;
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

void Session::tcpreceiver_thread()
{
    sepia::comm::Header header;
    while( !m_terminate )
    {
        std::size_t header_size = 0;
        m_socket->recvMsg( m_recvBuffer.data(), header_size );
        header.ParseFromArray( m_recvBuffer.data(), header_size );

        std::size_t msg_size = 0;
        m_socket->recvMsg( m_recvBuffer.data() + header_size , msg_size );

        if( ( header.message_name() == "sepia.comm.internal.RemoteSubscription" )
         || ( header.message_name() == "sepia.comm.internal.RemoteUnsubscription" ) )
        {
            std::cout << "Got: " << header.message_name() << " routing local.." << std::endl;
            ObserverBase::routeMessageToThreads( &header, m_recvBuffer.data() + header_size, msg_size );
            std::cout << "Routing complete." << std::endl;
        }
        else
        {
            header.set_source_node( getpid() );
            std::cout << "Got: " << header.message_name() << "rawSending" << std::endl;
            sepia::comm::MessageSender::rawSend( &header, m_recvBuffer.data() + header_size, msg_size );
            std::cout << "Rawsend complete." << std::endl;
        }
    }
}
