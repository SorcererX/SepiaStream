#include <receivetester.hpp>

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
    sepia::comm::Observer< commtester_msgs::Test >::initReceiver();
    while( !m_terminate )
    {
        sepia::comm::ObserverBase::threadReceiver();
    }
}

void ReceiveTester::receive( const commtester_msgs::Test* msg )
{
    m_messageCount++;
    std::cout << "Got " << m_messageCount << " message(s)." << std::endl;
}

int ReceiveTester::getMessageCount()
{
    return m_messageCount;
}
