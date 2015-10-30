/*
 * file: test_read.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: This program reads an IGES file and writes it
 * back out as 'test_out_read.igs'. This tests the current
 * implementation's ability to recognize entities within the
 * given input file; unhandled entities will be culled and
 * incorrectly implemented entities may result in a corrupt
 * IGES file. The informational output messages usually
 * provide some hints about the issues reading the given
 * input file.
 *
 * This file is part of libIGES.
 *
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <iostream>
#include <iomanip>
#include <dll_iges.h>
#include <all_api_entities.h>

#define ONAME "test_out_read.igs"

using namespace std;

void print_vec( const MCAD_POINT p )
{
    cout << setprecision( 3 );
    cout << "V: " << p.x << ", " << p.y << ", " << p.z << "\n";
    return;
}

int main( int argc, char **argv )
{
    if( argc != 2 )
    {
        cout << "*** Usage: readtest modelname\n";
        return -1;
    }

    DLL_IGES model;

    if( !model.Read( argv[1] ) )
    {
        cerr << "Oops - too bad, better luck next time\n";
    }
    else
    {
        cout << "[OK]: things are looking good\n";
    }

    model.Write( ONAME, true );

    return 0;
}
