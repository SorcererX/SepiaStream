#include <sepia/comm2/observerbase.h>
#include <iostream>
#include <zmq.h>

namespace sepia
{
    namespace comm2
    {

        thread_local ObserverBase::ObserverMap ObserverBase::stm_observers;
        std::shared_ptr< Global > ObserverBase::sm_global( Global::instance() );
        thread_local zmq::socket_t ObserverBase::stm_socket( sm_global->getContext(), ZMQ_SUB );
        thread_local bool ObserverBase::stm_initialized( false );
        thread_local bool ObserverBase::stm_blocking( false );

        void ObserverBase::addObserver( const std::string& a_name, ObserverBase* a_observer, unsigned int a_id )
        {
            a_observer->m_subscriberId = a_id;
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
                if( a_id == 0 )
                {
                    stm_socket.setsockopt( ZMQ_SUBSCRIBE, a_name.data(), a_name.size() );
                }
                else
                {
                    std::string filtered = a_name;
                    filtered.append( "," );
                    filtered.append( std::to_string( a_id ) );
                    stm_socket.setsockopt( ZMQ_SUBSCRIBE, filtered.data(), filtered.size() );
                }
            }
        }

        void ObserverBase::removeObserver( const std::string& a_name, ObserverBase* a_observer )
        {
            ObserverMap::iterator it = stm_observers.find( a_name );

            if( it != stm_observers.end() )
            {
                unsigned int subscriberId = a_observer->m_subscriberId;
                it->second.erase( a_observer );

                if( it->second.size() == 0 )
                {
                    stm_observers.erase( it );
                    std::cout << "Unsubscribing to: " << a_name << std::endl;
                    try
                    {
                        if( subscriberId == 0 )
                        {
                            stm_socket.setsockopt( ZMQ_UNSUBSCRIBE, a_name );
                        }
                        else
                        {
                            std::string filtered = a_name;
                            filtered.append( "," );
                            filtered.append( std::to_string( subscriberId ) );
                            stm_socket.setsockopt( ZMQ_UNSUBSCRIBE, filtered );
                        }
                    }
                    catch( const zmq::error_t& ex )
                    {
                        // std::cout << __PRETTY_FUNCTION__ << " " << ex.what() << std::endl;
                    }
                }
            }
        }

        void ObserverBase::distribute( const std::string& a_name, unsigned int a_id, const char* a_buffer, size_t a_size )
        {
            ObserverMap::iterator it = stm_observers.find( a_name );

            if( it != stm_observers.end() )
            {
                std::unordered_set< ObserverBase* >::const_iterator it2;

                for( it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
                {
                    if( a_id == 0 )
                    {
                        ( *it2 )->process( a_buffer, a_size, a_id );
                    }
                    else
                    {
                        if( ( *it2 )->m_subscriberId == 0 || ( *it2 )->m_subscriberId == a_id )
                        {
                            ( *it2 )->process( a_buffer, a_size, a_id );
                        }
                    }
                }
            }
            else
            {
                std::cout << __PRETTY_FUNCTION__ << " - name: " << a_name << " -- NO SUBSCRIBER" << std::endl;
            }
        }

        bool ObserverBase::initReceiver( int a_timeoutMs )
        {
            if( !stm_initialized )
            {
                if( a_timeoutMs == 0 )
                {
                    stm_blocking = false;
                    a_timeoutMs = -1;
                }
                else
                {
                    stm_blocking = true;
                }

                stm_socket.setsockopt( ZMQ_RCVTIMEO, &a_timeoutMs, sizeof( int ) );
                stm_socket.connect( sm_global->getPublisherUrl() );
                stm_initialized = true;
            }
            return true;
        }

        bool ObserverBase::destroyReceiver()
        {
            if( stm_initialized )
            {
                stm_socket.close();
                stm_initialized = false;
            }
            return true;
        }

        bool ObserverBase::threadReceiver()
        {
            zmq::message_t name_msg;
            try
            {
                if( stm_blocking )
                {
                    if( !stm_socket.recv( &name_msg ) )
                    {
                        // timeout
                        return true;
                    }
                }
                else
                {
                    stm_socket.recv( &name_msg, ZMQ_NOBLOCK );
                }
            }
            catch( const zmq::error_t& ex )
            {

                if( ex.num() != ETERM )
                {
                    throw;
                }
                stm_socket.close();
                return false;
            }

            std::string name( static_cast< const char* >( name_msg.data() ), name_msg.size() );

            zmq::message_t data;
            try
            {
                stm_socket.recv( &data, 0 );
            }
            catch( const zmq::error_t& ex )
            {
                if( ex.num() != ETERM )
                {
                    throw;
                }
                stm_socket.close();
                return false;
            }

            size_t pos = name.find_last_of( ',' );

            if( pos == std::string::npos )
            {
                distribute( name, 0, static_cast< char* >( data.data() ), data.size() );
            }
            else
            {
                distribute( name.substr( 0, pos ), std::stoi( name.substr( pos + 1 ) ), static_cast< char* >( data.data() ), data.size() );
            }

            return true;
        }

    }
}
