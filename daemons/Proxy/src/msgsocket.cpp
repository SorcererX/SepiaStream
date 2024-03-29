#include <msgsocket.h>
#include <sepia/network/tcpsocket.h>
#include "header.pb.h"

const std::size_t BUFFER_SIZE = 16384; // 16 KiB

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
    std::size_t send_bytes = a_msg->ByteSizeLong();
    a_msg->SerializeToArray( m_sendBuffer.data(), send_bytes );
    sendMsg( m_sendBuffer.data(), send_bytes );
}

void MsgSocket::sendMsg( const char* a_buffer, std::size_t a_size )
{
    std::cout << "MsgSocket::sendMsg() - " << a_size << std::flush;
    m_socket->send( a_size );
    m_socket->send( a_buffer, a_size );
    std::cout << " -- COMPLETE" << std::endl;
}

void MsgSocket::recvMsg( google::protobuf::MessageLite* a_msg )
{
    std::size_t elem_size;
    recvMsg( m_receiveBuffer.data(), elem_size );
    a_msg->ParseFromArray( m_receiveBuffer.data(), elem_size );
}

void MsgSocket::recvMsg( char* a_buffer, std::size_t& a_size )
{
    std::cout << "MsgSocket::recvMsg() - get size: " << std::flush;
    m_socket->receive( a_size );
    std::cout << a_size << std::flush;
    m_socket->receive( a_buffer, a_size );
    std::cout << " -- COMPLETE" << std::endl;
}

