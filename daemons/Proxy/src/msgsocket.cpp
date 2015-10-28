#include <msgsocket.h>
#include <sepia/network/tcpsocket.h>
#include <sepia/comm/header.pb.h>

const size_t BUFFER_SIZE = 16384; // 16 KiB

MsgSocket::MsgSocket( int a_socket )
{
    init();
    m_socket = new sepia::network::TcpSocket( a_socket );
}

MsgSocket::MsgSocket( sepia::network::TcpSocket* a_socket )
{
    init();
    m_socket = a_socket;
}


void MsgSocket::init()
{
    m_sendBuffer.reserve( BUFFER_SIZE );
    m_sendBuffer.resize(  BUFFER_SIZE );

    m_receiveBuffer.reserve( BUFFER_SIZE );
    m_receiveBuffer.resize(  BUFFER_SIZE );
}

MsgSocket::~MsgSocket()
{
    delete m_socket;
}

void MsgSocket::sendMsg( const google::protobuf::MessageLite* a_msg )
{
    size_t send_bytes = a_msg->ByteSize();
    a_msg->SerializeToArray( m_sendBuffer.data(), send_bytes );
    sendMsg( m_sendBuffer.data(), send_bytes );
}

void MsgSocket::sendMsg( const char* a_buffer, size_t a_size )
{
    m_socket->send( a_size );
    m_socket->send( a_buffer, a_size );
}

void MsgSocket::recvMsg( google::protobuf::MessageLite* a_msg )
{
    size_t elem_size;
    recvMsg( m_receiveBuffer.data(), elem_size );
    a_msg->ParseFromArray( m_receiveBuffer.data(), elem_size );
}

void MsgSocket::recvMsg( char* a_buffer, size_t& a_size )
{
    m_socket->receive( a_size );
    m_socket->receive( a_buffer, a_size );
}

