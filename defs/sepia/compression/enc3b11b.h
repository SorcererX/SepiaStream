#ifndef _3B11B_H
#define _3B11B_H
#include <vector>
#include <unistd.h>

namespace sepia
{
namespace compression
{

class Enc3b11b
{
public:
    Enc3b11b();
    ~Enc3b11b();
    void decode( unsigned char* a_destination, unsigned char* a_source, size_t a_size );
    void encode( unsigned char* a_destination, unsigned char* a_source, size_t a_size );
private:
    size_t m_stepSize;
    std::vector< unsigned char > m_lastInput;
    std::vector< char > m_diff;
};

}
}

#endif // 3b11b
