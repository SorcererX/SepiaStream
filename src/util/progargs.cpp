#include <sepia/util/progargs.h>
#include <iostream>

namespace sepia
{
namespace util
{

std::map< std::string, std::string > ProgArgs::sm_LongArgs;
std::map< std::string, std::pair< std::string, std::string> > ProgArgs::sm_defaults;

void ProgArgs::init( int argc, char** argv )
{
    std::string* last_value = NULL;
    bool last_was_argument = false;
    for( int i = 1; i < argc; i++ ) // i = 0 is the executable.
    {
        std::string str( argv[i] );

        if( ( str.length() > 2 )
         && ( str.compare( 0, 2, "--" ) == 0 ) )
        {
            str.erase( str.begin(), str.begin() + 2 );
            last_value = &sm_LongArgs[ str ];
            last_was_argument = true;
        }
        else if( last_value != NULL && last_was_argument )
        {
            *last_value = str;
            last_was_argument = false;
        }
        else
        {
            last_was_argument = false;
        }
    }
}

bool ProgArgs::contains( const std::string& a_string )
{
    std::map< std::string, std::string >::const_iterator it = sm_LongArgs.find( a_string );

    return it != sm_LongArgs.end();
}

const std::string ProgArgs::value( const std::string& a_string, bool* a_valid )
{
    std::map< std::string, std::string >::const_iterator it = sm_LongArgs.find( a_string );

    if( it != sm_LongArgs.end() )
    {
        if( a_valid )
        {
            *a_valid = true;
        }
        return it->second;
    }
    else
    {
        if( a_valid )
        {
            *a_valid = false;
        }
        return std::string();
    }
}

void ProgArgs::addOptionDefaults( const std::string& a_option, const std::string& a_value, const std::string& a_description )
{
    sm_defaults[ a_option ] = std::make_pair( a_value, a_description );
}

void ProgArgs::printHelp()
{
    for( const auto& item : sm_defaults )
    {
        std::cerr << item.first << " :" << item.second.second << std::endl;
    }
}

}
}
