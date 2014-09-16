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

    void perform();
};

}
}

#endif // SEPIA_COMPRESSION_COMPRESS_H
