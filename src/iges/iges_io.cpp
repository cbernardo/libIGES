/*
 * file: iges_io.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: helper routines and structures for IGES file I/O
 *
 * This file is part of libIGES.
 *
 * libIGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <string>

#include <error_macros.h>
#include <iges_io.h>


using namespace std;


bool DEItemToInt( const std::string& input, int field, int& var, int* defaulted )
{
    if( field < 0 || field > 9 )
    {
        ERRMSG << "\n + [BUG]: invalid field (" << field << "); valid values are 0..9 only\n";
        return false;
    }

    if( input.length() < (unsigned int)(8 * (field + 1)) )
    {
        ERRMSG << "\n + [BUG]: input string too short; required length: ";
        cerr << (8 * (field + 1)) << ", actual length: " << input.length() << "\n";
        return false;
    }

    char tmp[9];
    int  i;
    int  j = 8 * field;
    int  k = 8;
    size_t np;

    // is the space all blank; if so, is there a default value?
    np = input.substr(j, 8).find_first_not_of(' ');

    if( np == string::npos )
    {
        if( !defaulted )
        {
            ERRMSG << "\n + [BUG/BAD DATA]: no data for non-default parameter\n";
            return false;
        }

        var = *defaulted;
        return true;
    }

    j += np;
    k -= np;

    for( i = 0; i < k; ++i, ++j )
        tmp[i] = input[j];

    tmp[i] = 0;
    char *cp = NULL;

    errno = 0;
    i = strtol(tmp, &cp, 10);

    if( errno || cp == tmp )
    {
        ERRMSG << "\n + [BAD DATA]: not an integer: '" << tmp << "'\n";
        return false;
    }

    if( cp != &tmp[8] )
    {
        ERRMSG << "\n + [BAD DATA]: integer not right justified: '" << tmp << "'\n";
        return false;
    }

    var = i;
    return true;
}


bool DEItemToStr( const std::string& input, int field, std::string& var )
{
    var.clear();

    if( field < 0 || field > 9 )
    {
        ERRMSG << "\n + [BUG]: invalid field (" << field << "); valid values are 0..9 only\n";
        return false;
    }

    if( input.length() < (unsigned int)(8 * (field + 1)) )
    {
        ERRMSG << "\n + [BUG]: input string too short; required length: ";
        cerr << (8 * (field + 1)) << ", actual length: " << input.length() << "\n";
        return false;
    }

    int i = 8;
    int j = field * 8;

    size_t idx = input.substr(j, 8).find_first_not_of(' ');

    if( idx != string::npos )
    {
        j += idx;
        i -= idx;

        var = input.substr(j , i);
    }

    return true;
}


bool ReadIGESRecord( IGES_RECORD* aRecord, std::ifstream& aFile, std::streampos* aRefPos )
{
    string iline;

    if( !aFile.good() )
    {
        ERRMSG << "\n + I/O problems\n";
        return false;
    }

    if( aRefPos )
        *aRefPos = aFile.tellg();

    std::getline( aFile, iline );

    if( !aFile.good() && !aFile.eof() )
    {
        ERRMSG << "\n + I/O problems\n";
        return false;
    }

    if( iline.length() != 80 )
    {
        ERRMSG << "\n + invalid line length (" << iline.length() << "); must be 80\n";
        cerr << " + line: '" << iline << "'\n";
        return false;
    }

    aRecord->data = iline.substr(0, 72);
    aRecord->section_type = iline[72];

    switch( aRecord->section_type )
    {
        case 'F':
        case 'S':
        case 'G':
        case 'D':
        case 'P':
        case 'T':
            break;

        default:
            ERRMSG << "\n + invalid Section Flag ('" << iline[72] << "')\n";
            cerr << " + line: '" << iline << "'\n";
            return false;
            return false;
            break;
    }

    iline[72] = ' ';
    int tmpInt;

    if( !DEItemToInt(aRecord->data, 8, tmpInt, NULL))
    {
        iline[72] = aRecord->section_type;
        ERRMSG << "\n + no sequence number\n";
        cerr << " + line: '" << iline << "'\n";
        return false;
    }

    if(tmpInt <= 0)
    {
        iline[72] = aRecord->section_type;
        ERRMSG << "\n + invalid sequence number\n";
        cerr << " + line: '" << iline << "'\n";
        return false;
    }

    aRecord->index = tmpInt;

    return true;
}
