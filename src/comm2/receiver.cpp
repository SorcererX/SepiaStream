#include "global.h"
#include <sepia/comm2/receiver.h>
#include <iostream>
#include <zmq.h>
#include <zmq.hpp>
#include <sepia/comm2/observerbase.h>
#include <sepia/comm2/observerall.h>

namespace sepia::comm2
{
    Receiver::Receiver()
    {
    }

    Receiver::Receiver( ObserverAll* a_observer, int a_timeoutMs )
    {
        addObserver( a_observer );
        init( a_timeoutMs );
    }

    Receiver::~Receiver()
    {
        destroy();
    }

    bool Receiver::init( int a_timeoutMs )
    {
        if( a_timeoutMs == 0 )
        {
            m_blocking = false;
            a_timeoutMs = -1;
        }
        else
        {
            m_blocking = true;
        }

        if( m_initialized )
        {
            return false;
        }
        m_socket = std::make_unique< zmq::socket_t >( Global::instance()->getContext(), ZMQ_SUB );

        try
        {
            m_socket->setsockopt( ZMQ_RCVTIMEO, &a_timeoutMs, sizeof( int ) );
            m_socket->connect( Global::instance()->getPublisherUrl() );
        }
        catch( const zmq::error_t& ex )
        {
            std::cout << __PRETTY_FUNCTION__ << " exception: " << ex.what() << std::endl;
        }

        m_initialized = true;

        // Add subscriptions here
        for( auto& [ name, observer ] : m_observers )
        {
            std::cout << "Subscribing to: " << name << std::endl;
            m_socket->setsockopt( ZMQ_SUBSCRIBE, name.data(), name.size() );
        }
        return true;
    }

    bool Receiver::destroy()
    {
        if( m_initialized )
        {
            for( auto& [ name, observer ] : m_observers )
            {
                std::cout << "Unsubscribing to: " << name << std::endl;
                m_socket->setsockopt( ZMQ_UNSUBSCRIBE, name.data(), name.size() );
            }
            m_socket->close();
            m_socket.reset();
            m_initialized = false;
        }
        return true;
    }

    bool Receiver::exec()
    {
        zmq::message_t name_msg;

        try
        {
            if( m_blocking )
            {
                if( !m_socket->recv( &name_msg ) )
                {
                    // timeout
                    return true;
                }
            }
            else
            {
                m_socket->recv( &name_msg, ZMQ_NOBLOCK );
            }
        }
        catch( const zmq::error_t& ex )
        {
            if( ex.num() != ETERM )
            {
                throw;
            }
            m_socket->close();
            return false;
        }

        std::string name( static_cast< const char* >( name_msg.data() ), name_msg.size() );

        zmq::message_t data;

        try
        {
            m_socket->recv( &data, 0 );
        }
        catch( const zmq::error_t& ex )
        {
            if( ex.num() != ETERM )
            {
                throw;
            }
            m_socket->close();
            return false;
        }

        distribute( name, static_cast< char* >( data.data() ), data.size() );

        return true;
    }

    bool Receiver::addObserver( ObserverAll* a_observer )
    {
        for( const auto& [ name, observer ] : a_observer->getObservers() )
        {
            addObserver( name, observer );
        }
        return true;
    }

    bool Receiver::removeObserver( ObserverAll* a_observer )
    {
        for( const auto& [ name, observer ] : a_observer->getObservers() )
        {
            removeObserver( name, observer );
        }
        return true;
    }

    bool Receiver::addObserver( const std::string& a_name, ObserverBase* a_observer )
    {
        ObserverMap::iterator it = m_observers.find( a_name );

        if( it != m_observers.end() )
        {
            it->second.insert( a_observer );
        }
        else
        {
            std::unordered_set< ObserverBase* > temp;
            temp.insert( a_observer );
            m_observers[ a_name ] = temp;

            if( m_initialized )
            {
                std::cout << "Subscribing to: " << a_name << std::endl;
                m_socket->setsockopt( ZMQ_SUBSCRIBE, a_name.data(), a_name.size() );
            }
        }
        return true;
    }

    bool Receiver::removeObserver( const std::string& a_name, ObserverBase* a_observer )
    {
        ObserverMap::iterator it = m_observers.find( a_name );

        if( it != m_observers.end() )
        {
            it->second.erase( a_observer );

            if( it->second.empty() && m_initialized )
            {
                std::cout << "Unsubscribing to: " << a_name << std::endl;
                try
                {
                    m_socket->setsockopt( ZMQ_UNSUBSCRIBE, a_name );
                }
                catch( const zmq::error_t& ex )
                {
                }
            }
        }
        return false;
    }

    bool Receiver::distribute( const std::string& a_name, const char* a_data, size_t a_size )
    {
        auto observerlist_it = m_observers.find( a_name );

        if( observerlist_it != m_observers.end() )
        {
            for( auto& observer : observerlist_it->second )
            {
                observer->process( a_data, a_size );
            }
        }
        return true;
    }
}