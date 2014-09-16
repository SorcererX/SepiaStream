#ifndef SEPIA_COMPRESSION_COMPRESS_H
#define SEPIA_COMPRESSION_COMPRESS_H
#include <sepia/compression/common.h>

namespace sepia
{
namespace compression
{

class Decompress : public Common
{
public:
    Decompress( Common::Method a_method );
    ~Decompress();

    size_t perform();
};

}
}

#endif // SEPIA_COMPRESSION_COMPRESS_H
