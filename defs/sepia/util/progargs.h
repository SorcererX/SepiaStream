#ifndef SEPIA_UTIL_PROGARGS_H
#define SEPIA_UTIL_PROGARGS_H
#include <map>
#include <string>
#include <utility>
#include <typeinfo>

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
    template <typename T> static T value( const std::string& a_string, bool* a_valid = NULL );

    template <typename T> static void addOptionDefaults( const std::string& a_option, T* a_value, const std::string& a_description );
    static void printHelp();

private:
    ProgArgs() = delete;
    static std::map< std::string, std::string > sm_LongArgs;
    static std::map< std::string, std::pair< std::string, std::string> > sm_defaults;
};


template <typename T> T ProgArgs::value( const std::string& a_string, bool* a_valid )
{
    const std::string val = value( a_string, a_valid );

    if( typeid(T) == typeid(int) )
    {
        return std::stoi( val );
    }
    else if( typeid(T) == typeid(float) )
    {
        return std::stof( val );
    }
    else if( typeid(T) == typeid(double) )
    {
        return std::stod( val );
    }
    else if( typeid(T) == typeid(long) )
    {
        return std::stol( val );
    }
    else if( typeid(T) == typeid(long long) )
    {
        return std::stoll( val );
    }
    else if( typeid(T) == typeid(long double) )
    {
        return std::stold( val );
    }
}

template<typename T> inline void ProgArgs::addOptionDefaults( const std::string& a_option,
                                                        T* a_value,
                                                        const std::string& a_description )
{
    sm_defaults[ a_option ] = std::make_pair( std::to_string( *a_value ), a_description );

    if( contains( a_option ) )
    {
        bool valid = false;
        T tmpval = value<T>( a_option, &valid );
        if( valid )
        {
            *a_value = tmpval;
        }
    }
}

template<> inline void ProgArgs::addOptionDefaults< std::string >( const std::string& a_option,
                                              std::string* a_value,
                                              const std::string& a_description )
{
    sm_defaults[ a_option ] = std::make_pair( *a_value, a_description );

    if( contains( a_option ) )
    {
        bool valid = false;
        std::string tmpval = value( a_option, &valid );

        if( valid )
        {
            *a_value = tmpval;
        }
    }
}

}
}

#endif // SEPIA_UTIL_PROGARGS_H
