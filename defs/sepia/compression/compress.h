#ifndef SEPIA_COMPRESSION_COMPRESS_H
#define SEPIA_COMPRESSION_COMPRESS_H
#include <sepia/compression/common.h>

namespace sepia
{
namespace compression
{

class Compress : public Common
{
public:
    Compress( Common::Method a_method, int a_level );
    ~Compress();

    size_t perform();
};

}
}

#endif // SEPIA_COMPRESSION_COMPRESS_H
