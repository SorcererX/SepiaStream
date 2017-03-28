#include <sepia/comm2/observerbase.h>
#include <iostream>
#include <unistd.h>

namespace sepia
{
namespace comm2
{

thread_local ObserverBase::ObserverMap ObserverBase::stm_observers;
thread_local zmq::context_t ObserverBase::stm_context( 1 );
thread_local zmq::socket_t  ObserverBase::stm_socket( stm_context, ZMQ_SUB );
thread_local bool ObserverBase::stm_initialized( false );

void ObserverBase::addObserver( const std::string& a_name, ObserverBase* a_observer )
{
    ObserverMap::iterator it = stm_observers.find( a_name );

    if( it != stm_observers.end() )
    {
        it->second.insert( a_observer );
    }
    else
    {
        std::unordered_set< ObserverBase* > temp;
        temp.insert( a_observer );
        stm_observers[ a_name ] = temp;
        std::cout << "Subscribing to: " << a_name << std::endl;
        stm_socket.setsockopt( ZMQ_SUBSCRIBE, a_name.data(), a_name.size() );
        //sm_socket.setsockopt( ZMQ_SUBSCRIBE, NULL, 0 );
    }
}

void ObserverBase::removeObserver(const std::string& a_name, ObserverBase* a_observer )
{
    ObserverMap::iterator it = stm_observers.find( a_name );

    if( it != stm_observers.end() )
    {
        it->second.erase( a_observer );

        if( it->second.size() == 0 )
        {
            stm_observers.erase( it );
            std::cout << "Unsubscribing to: " << a_name << std::endl;
            stm_socket.setsockopt( ZMQ_UNSUBSCRIBE, a_name );
        }
    }
}

void ObserverBase::distribute( const std::string& a_name, const char* a_buffer, size_t a_size )
{
    ObserverMap::iterator it = stm_observers.find( a_name );

    if( it != stm_observers.end() )
    {
        std::unordered_set< ObserverBase* >::const_iterator it2;

        for( it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
        {
            (*it2)->process( a_buffer );
        }
    }
    else
    {
        std::cout << __PRETTY_FUNCTION__ << " - name: " << a_name << " -- NO SUBSCRIBER" << std::endl;
    }
}

void ObserverBase::threadReceiver()
{
    if( !stm_initialized )
    {
        stm_socket.connect( "tcp://127.0.0.1:31350" ); // 31350 for internal.
        stm_initialized = true;
    }

    bool ok = false;

    //std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
    zmq::message_t name_msg;
    ok = stm_socket.recv( &name_msg, ZMQ_NOBLOCK );
    if( !ok )
    {
        return ;
    }
    std::string name( static_cast< const char* >( name_msg.data() ), name_msg.size() );
    //std::cout << "-- length: " << name_msg.size() << " name: " << name << std::endl;

    zmq::message_t data;
    stm_socket.recv( &data, 0 );

    //std::cout << "-- data_length: " << data.size() << std::endl;
    distribute( name, static_cast< char* >( data.data() ), data.size()  );
}

}
}
