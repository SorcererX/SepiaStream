#ifndef SEPIA_UTIL_PROGARGS_H
#define SEPIA_UTIL_PROGARGS_H
#include <map>
#include <string>
#include <utility>

namespace sepia
{
namespace util
{

class ProgArgs
{
public:
    static void init( int argc, char** argv );
    static bool contains( const std::string& a_string );
    static const std::string value( const std::string& a_string, bool* a_valid = NULL );
    static void addOptionDefaults( const std::string& a_option, const std::string& a_value, const std::string& a_description );
    static void printHelp();

private:
    ProgArgs() = delete;
    static std::map< std::string, std::string > sm_LongArgs;
    static std::map< std::string, std::pair< std::string, std::string> > sm_defaults;
};


}
}

#endif // SEPIA_UTIL_PROGARGS_H
