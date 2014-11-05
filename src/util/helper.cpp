#include <helper.h>
#include <iostream>

namespace sepia
{
namespace util
{
void print_time( std::chrono::steady_clock::time_point& a_start )
{
    auto end = std::chrono::steady_clock::now();
    auto elapsed = end-a_start;
    std::cerr << "Processing time: " << elapsed.count() * 1000.0 / std::chrono::steady_clock::period().den << " ms" << std::endl;
}
}
}
