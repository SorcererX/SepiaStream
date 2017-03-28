#include "receivetester.hpp"

ReceiveTester::ReceiveTester() : m_messageCount( 0 )
{
}

ReceiveTester::~ReceiveTester()
{

}

ReceiveTester::ReceiveTester( const ReceiveTester& a_object )
{
    m_messageCount = a_object.m_messageCount;
}

void ReceiveTester::start()
{
    m_thread = new std::thread( std::bind( &ReceiveTester::own_thread, this ) );

}

void ReceiveTester::own_thread()
{
    sepia::comm2::Observer< comm2tester_msgs::TestT >::initReceiver();
    while( !m_terminate )
    {
	sepia::comm2::ObserverBase::threadReceiver();
    }
    destroyReceiver();
}

void ReceiveTester::stop()
{
    if( m_thread )
    {
        //sepia::comm::ObserverBase::stopThreadReceiver( m_thread->get_id() );
    }
    sepia::util::ThreadObject::stop();
}

void ReceiveTester::receive( const std::unique_ptr< comm2tester_msgs::TestT >& a_msg )
{
    m_messageCount++;
    if( m_messageCount % 10000 == 0 )
    {
        std::cout << "Got " << m_messageCount << " message(s)." << " ( id: " << a_msg->seq_no << " )" << std::endl;
    }
}

int ReceiveTester::getMessageCount()
{
    return m_messageCount;
}
