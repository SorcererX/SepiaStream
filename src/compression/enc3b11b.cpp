/*
Copyright (c) 2012-2015, Kai Hugo Hustoft Endresen <kai.endresen@gmail.com>
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

#include <enc3b11b.h>
#include <sepia/util/helper.h>
#include <cstdint>
#include <bitset>
#include <iostream>
#include <cstring>

using sepia::util::print_time;

namespace
{
inline void set_bits( uint64_t* a_destination, const char* a_input, std::size_t a_steps )
{
    std::bitset<64> array;

    for( std::size_t i = 0; i < a_steps; i++ )
    {
        switch( *(a_input + i) )
        {
        case -3: // -3, encoded as 101
            array.set( 3*i+2, true );
            array.set( 3*i, true );
            break;
        case -2: // -2, encoded as  110
            array.set( 3*i+2, true );
            array.set( 3*i+1, true );
            break;
        case -1: // -1, encoded as 111
            array.set( 3*i+2, true );
            array.set( 3*i+1, true );
            array.set( 3*i,   true );
            break;
        case 0: // 0, encoded as 000
            break;
        case 1: // 1, encoded as 001
            array.set( 3*i,   true );
            break;
        case 2: // 2, encoded as 010
            array.set( 3*i+1, true );
            break;
        case 3: // 3, encoded as 011
            array.set( 3*i+1, true );
            array.set( 3*i,   true );
            break;
        default: // all others, encoded as 100
            array.set( 3*i+2, true );
            break;
        }
        //std::cout << (int) *(a_input + i) << " ";
    }
    *a_destination = array.to_ullong();
    // last bit (index: 63 is unused (avoids having to read/write bits across word boundaries)
}
}

namespace sepia
{
namespace compression
{

Enc3b11b::Enc3b11b() : m_stepSize( sizeof( u_int64_t ) * 8 / 3 )
{
}

Enc3b11b::~Enc3b11b()
{

}

void Enc3b11b::decode( unsigned char* a_destination, unsigned char* a_source, std::size_t a_size )
{
    const std::size_t pixel_offset = ( a_size * sizeof( u_int64_t ) / m_stepSize + sizeof( u_int64_t )  ) ;
    std::size_t read_bytes = 0;
    for( int i = 0; i < a_size; i+= sizeof( u_int64_t ) ) // decode entire 64-bit integers.
    {
        u_int64_t* temp = reinterpret_cast< u_int64_t* >( a_source+i );
        char diff[ m_stepSize ];
//        std::cout << "chunk_dst: " << std::bitset<64>( *temp ) << " " << i << std::endl;
        //std::cout << std::dec << std::endl;
        for( int j = 0; j < m_stepSize; j++ )
        {
            bool special = false;
            char value = ( *temp >> (j*3) ) & 0x7LL;
            switch( value )
            {
            case 5: // 101
                value = -3;
                break;
            case 6: // 110
                value = -2;
                break;
            case 7: // 111
                value = -1;
                break;
            case 0: // 000
                value = 0;
                break;
            case 1: // 001
                value = 1;
                break;
            case 2: // 010
                value = 2;
                break;
            case 3: // 011
                value = 3;
                break;
            case 4: // 100
                special = true;
                value = *(a_source+pixel_offset+read_bytes);
                read_bytes++;
                break;
            default:
                std::cerr << "Decode error: " << (int) value << " is not a valid 3-bit code." << std::endl;
                break;
            }
            //std::cout << "index: " << i*step_size+j << std::endl;
            std::size_t dest_index = i / sizeof( u_int64_t );

            unsigned char orig_value = *(a_destination+dest_index*m_stepSize+j);

            *(a_destination+dest_index*m_stepSize+j) = ( *(a_destination+dest_index*m_stepSize+j) + value ) % 256;
        }
    }
}

void Enc3b11b::encode( unsigned char* a_destination, unsigned char* a_source, std::size_t a_size )
{
    const std::size_t pixel_offset = ( a_size * sizeof( u_int64_t ) / m_stepSize + sizeof( u_int64_t )  ) ;
    auto begin = std::chrono::steady_clock::now();
    if( m_lastInput.size() == 0 )
    {
        m_lastInput.reserve( a_size );
        m_lastInput.assign( a_size, 0 );
        // this is the first image.
    }

    if( m_diff.size() == 0 )
    {
        m_diff.reserve( a_size );
        m_diff.assign( a_size, 0 );
    }

    // calculate diff
    for( std::size_t i = 0; i < a_size; i++ )
    {
        short value = static_cast<unsigned char>( *(a_source+i) ) - m_lastInput.at( i );

        if( value > 127 )
        {
            value -= 255;
        }
        else if( value < -127 )
        {
            value += 255;
        }
        m_diff[i] = static_cast< char >( value );
    }

    memcpy( m_lastInput.data(), a_source, a_size );

    print_time( begin );

    // set bit matrix
    for( std::size_t i = 0; i < a_size; i+= m_stepSize )
    {
        u_int64_t* curr = reinterpret_cast< u_int64_t* >( a_destination+(i*sizeof( u_int64_t) /m_stepSize ) );
        set_bits( curr, reinterpret_cast< char* >( m_diff.data()+i ), std::min( m_stepSize, a_size - i ) );
    }
    print_time( begin );

    // write non-0 pixels:
    std::size_t written_pixels = 0;
    for( std::size_t i = 0; i < m_diff.size(); i++ )
    {
        if( abs( m_diff[i] ) > 3  )
        {
            *(a_destination+ pixel_offset + written_pixels ) = m_diff[i];
            written_pixels++;
        }
    }
    print_time( begin );

    std::size_t frame_size = written_pixels + a_size * sizeof( u_int64_t ) / m_stepSize + sizeof( u_int64_t );

    std::cout << "3-bit pixels : " << a_size - written_pixels
              << " ( " << ( a_size - written_pixels ) * 100.0 / a_size << "% )"
              << "- compressed size: " << frame_size
              << " ( " << frame_size*100.0/a_size << "% )" << std::endl;
}
}
}
