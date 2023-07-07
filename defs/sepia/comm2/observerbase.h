#ifndef SEPIA_COMM2_OBSERVERBASE_H
#define SEPIA_COMM2_OBSERVERBASE_H
#include <map>
#include <string>
#include <unordered_set>
#include <zmq.hpp>

namespace sepia
{
namespace comm2
{

class ObserverBase
{
public:
    ~ObserverBase() {}
    static bool threadReceiver();

protected:
    ObserverBase() {}
    void addObserver(const std::string &a_name, ObserverBase* a_observer );
    void removeObserver(const std::string& a_name, ObserverBase* a_observer );
    static void distribute( const std::string& a_name, const char* a_buffer, std::size_t a_size );
    virtual void process( const char* a_buffer ) = 0;
private:
    typedef std::map< const std::string, std::unordered_set< ObserverBase* > > ObserverMap;
    static thread_local ObserverMap stm_observers;
    static thread_local zmq::socket_t  stm_socket;
    static thread_local bool stm_initialized;
};

}
}
#endif // SEPIA_COMM2_OBSERVERBASE_H
