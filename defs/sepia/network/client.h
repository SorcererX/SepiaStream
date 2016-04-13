#ifndef SEPIA_NETWORK_TCPCLIENT_H
#define SEPIA_NETWORK_TCPCLIENT_H
#include <atomic>
#include <string>
#include <thread>
#include <libde265/de265.h>

namespace sepia
{
namespace network
{
class Client
{
public:
    Client( const std::string& a_host, int a_port, const std::string& a_name, bool a_useCompression = false );
    ~Client();
    void start();
    void stop();
    void join();
protected:
    void own_thread();
private:
    std::string m_host;
    int m_port;
    std::string m_name;
    bool m_useCompression;
    std::atomic_bool m_terminate;
    std::thread m_thread;
    de265_decoder_context* m_h265Decoder;
};
}
}
#endif // SEPIA_PROXY_CLIENT_H
