/*
Copyright (c) 2012-2016, Kai Hugo Hustoft Endresen <kai.endresen@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

void ProgArgs::printHelp()
{
    for( const auto& item : sm_defaults )
    {
        std::cerr << "\t --" << item.first << ": " << item.second.second << " (default: " << item.second.first << ")" << std::endl;
    }
}

void ProgArgs::addDefault( const std::string& a_option, const std::string& a_value, const std::string& a_description )
{
    sm_defaults[ a_option ] = std::make_pair( a_value, a_description );
}

void ProgArgs::addOptionDefaults( const std::string& a_option, const std::string& a_description )
{
    addDefault( a_option, std::string(), a_description );
}
}
}
