#ifndef __MSGSOCKET_H
#define __MSGSOCKET_H
#include <vector>
#include <cstddef>

namespace google
{
    namespace protobuf
    {
        class MessageLite;
    }
}

namespace sepia
{
    namespace network
    {
        class TcpSocket;
    }
    namespace comm
    {
        class Header;
    }
}

class MsgSocket
{
public:
    MsgSocket( int a_socket );
    MsgSocket( sepia::network::TcpSocket* a_socket );
    ~MsgSocket();
    void sendMsg( const google::protobuf::MessageLite* a_msg );
    void sendMsg( const char* a_buffer, size_t a_size );
    void recvMsg( char* a_buffer, size_t& a_size );
    void recvMsg( google::protobuf::MessageLite* a_msg );

protected:
    void init();
private:
    sepia::network::TcpSocket* m_socket;
    std::vector< char > m_sendBuffer;
    std::vector< char > m_receiveBuffer;
};


#endif // __MSGSOCKET_H
