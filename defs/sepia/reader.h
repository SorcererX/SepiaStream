#ifndef SEPIA_READER_H
#define SEPIA_READER_H
#include <sepia/stream.h>

namespace sepia
{
class Reader : public Stream
{
public:
    Reader( const std::string& a_name );
    void update();
};
}

#endif // SEPIA_READER_H
