#include <receivetester.hpp>
#include <functional>

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

void ReceiveTester::own_thread()
{
    sepia::comm::Observer< commtester_msgs::Test >::initReceiver();
    while( !m_terminate )
    {
        sepia::comm::ObserverBase::threadReceiver();
    }
    destroyReceiver();
}

void ReceiveTester::stop()
{
    if( m_thread )
    {
        sepia::comm::ObserverBase::stopThreadReceiver( m_thread->get_id() );
    }
    sepia::util::ThreadObject::stop();
}

void ReceiveTester::receive( const commtester_msgs::Test* msg )
{
    m_messageCount++;
    //std::cout << "Got " << m_messageCount << " message(s)." << std::endl;
}

int ReceiveTester::getMessageCount()
{
    return m_messageCount;
}
