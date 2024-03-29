#ifndef SEPIA_COMM2_OBSERVERBASE_H
#define SEPIA_COMM2_OBSERVERBASE_H
#include <map>
#include <string>
#include <unordered_set>
#include <atomic>
#include "global.h"

namespace sepia
{
    namespace comm2
    {

        class ObserverBase
        {
            public:
                virtual ~ObserverBase() {}
                static bool initReceiver( int a_timeoutUs = 0 );
                static bool threadReceiver();
                static bool destroyReceiver();

            protected:
                ObserverBase() : m_subscriberId( 0 ) {}
                void addObserver(const std::string &a_name, ObserverBase* a_observer, unsigned int a_id = 0 );
                void removeObserver(const std::string& a_name, ObserverBase* a_observer );
                static void distribute( const std::string& a_name, unsigned int a_id, const char* a_buffer, size_t a_size );
                virtual void process( const char* a_buffer, size_t a_size, unsigned int a_id ) = 0;

            private:
                typedef std::map< const std::string, std::unordered_set< ObserverBase* > > ObserverMap;
                static thread_local ObserverMap stm_observers;
                static std::shared_ptr< Global > sm_global;
                static thread_local zmq::socket_t stm_socket;
                static thread_local bool stm_initialized;
                std::atomic_uint m_subscriberId;
                static thread_local bool stm_blocking;
        };

    }
}
#endif // SEPIA_COMM2_OBSERVERBASE_H

