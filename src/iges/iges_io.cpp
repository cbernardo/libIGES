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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#include <error_macros.h>
#include <iges_io.h>
#include <mcad_elements.h>


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
            cerr << " + string: '" << input.substr(j, 8) << "'\n";
            cerr << " + position: " << j << "\n";
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
    i = strtol( tmp, &cp, 10 );

    if( errno || cp == tmp )
    {
        ERRMSG << "\n + [BAD DATA]: not an integer: '" << tmp << "'\n";
        return false;
    }

    if( 0 != *cp )
    {
        ERRMSG << "\n + [BAD DATA]: integer not right justified: '" << tmp << "'\n";
        cerr << " + string: '" << tmp << "'\n";
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

    if( iline.length() )
    {
        char estr = iline[iline.length() -1];

        while( (estr == '\n' || estr == '\r' || estr == '\f') && iline.size() > 1 )
        {
            iline.erase( --iline.end() );
            estr = iline[iline.length() -1];
        }
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
            break;
    }

    iline[72] = ' ';
    int tmpInt;

    if( !DEItemToInt(iline, 9, tmpInt, NULL))
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


bool ParseHString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd )
{
    param.clear();

    if( idx >= (int)data.length() )
    {
        ERRMSG << "\n + [BUG] out of bounds\n";
        return false;
    }

    if( data[idx] == pd || data[idx] == rd )
    {
        if( data[idx] == rd )
            eor = true;

        ++idx;
        return true;
    }

    const char* cp = &(data.c_str()[idx]);
    char* rp;

    errno = 0;
    int i = strtol( cp, &rp, 10 );

    if( errno || cp == rp )
    {
        ERRMSG << "\n + [BAD DATA]: invalid Hollerith string\n";
        cerr << "Data: " << cp << "\n";
        return false;
    }

    idx += (rp - cp);

    if( data[idx] != 'H' )
    {
        ERRMSG << "\n + [BAD DATA]: invalid Hollerith string (no 'H' following length)\n";
        cerr << "Data: " << cp << "\n";
        return false;
    }

    ++idx;

    if( i <= 0 )
    {
        ERRMSG << "\n + [BAD DATA]: invalid Hollerith string length (" << i << ")\n";
        cerr << "Data: " << cp << "\n";
        return false;
    }

    if( idx + i >= (int)data.length() )
    {
        ERRMSG << "\n + [BAD DATA]: invalid Hollerith string length (" << i << ")\n";
        cerr << " + requested string length exceeds record length\n";
        cerr << "Data: " << cp << "\n";
        return false;
    }

    param = data.substr(idx, i);

    idx += i;

    if( data[idx] == rd )
    {
        ++idx;
        eor = true;
        return true;
    }

    if( data[idx] == pd )
    {
        ++idx;
        return true;
    }

    ERRMSG << "\n + [BAD DATA]: invalid record; no Parameter or Record delimeter after Hollerith string\n";
    cerr << "Data: " << cp << "\n";
    cerr << "String: '" << param << "'\n";
    cerr << "Character found in place of delimeter: '" << data[idx] << "'\n";
    return false;
}


bool ParseLString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd )
{
    param.clear();
    int tidx = idx;

    if( idx >= (int)data.length() )
    {
        ERRMSG << "\n + [BUG] out of bounds\n";
        return false;
    }

    if( data[idx] == pd || data[idx] == rd )
    {
        if( data[idx] == rd )
            eor = true;

        ++idx;
        return true;
    }

    size_t strEnd = data.find_first_of( pd, idx );
    size_t strEnd2 = data.find_first_of( rd, idx );

    if( strEnd == string::npos || (strEnd2 != string::npos && strEnd > strEnd2) )
    {
        if( strEnd2 == string::npos )
        {
            ERRMSG << "\n + [BAD DATA] no Parameter or Record delimeter found in data\n";
            cerr << "Data: " << data.substr( idx ) << "\n";
            return false;
        }

        strEnd = strEnd2;
    }

    param = data.substr( idx, strEnd - idx );
    idx += (int)param.length();

    if( data[idx] == rd )
    {
        ++idx;
        eor = true;
        return true;
    }

    if( data[idx] == pd )
    {
        ++idx;
        return true;
    }

    ERRMSG << "\n + [BAD DATA]: invalid record; no Parameter or Record delimeter after string\n";
    cerr << "Data: " << data.substr(tidx) << "\n";
    return false;
}


bool ParseInt( const std::string& data, int& idx, int& param, bool& eor, char pd, char rd, int* idefault )
{
    std::string tmp;
    int tidx = idx;

    if( !ParseLString( data, idx, tmp, eor, pd, rd ) )
    {
        ERRMSG << "[BAD DATA]\n";
        return false;
    }

    if( tmp.empty() )
    {
        if( idefault )
        {
            param = *idefault;
            return true;
        }

        ERRMSG << "\n + [BAD DATA]: empty field for non-default parameter\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    const char* cp = tmp.c_str();
    char* rp;

    errno = 0;
    int i = strtol( cp, &rp, 10 );

    if( errno || cp == rp )
    {
        ERRMSG << "\n + [BAD DATA]: invalid integer\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    if( rp - cp != (int)tmp.length() )
    {
        ERRMSG << "\n + [WARNING]: extra characters at end of integer\n";
        cerr << "Integer value: " << i << "\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
    }

    param = i;
    return true;
}


bool ParseReal( const std::string& data, int& idx, double& param, bool& eor, char pd, char rd, double* ddefault )
{
    std::string tmp;
    int tidx = idx;

    if( !ParseLString( data, idx, tmp, eor, pd, rd ) )
    {
        ERRMSG << "[BAD DATA]\n";
        return false;
    }

    if( tmp.empty() )
    {
        if( ddefault )
        {
            param = *ddefault;
            return true;
        }

        ERRMSG << "\n + [BAD DATA]: empty field for non-default parameter\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    size_t dex = tmp.find_first_of( 'D' );

    if( dex != string::npos )
        tmp[dex] = 'E';

    const char* cp = tmp.c_str();
    char* rp;

    errno = 0;
    double d = strtod( cp, &rp );

    if( errno || cp == rp )
    {
        ERRMSG << "\n + [BAD DATA]: invalid floating point number\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    if( rp - cp != (int)tmp.length() )
    {
        ERRMSG << "\n + [WARNING]: extra characters at end of floating point number\n";
        cerr << "Float value: " << setprecision(12) << d << setprecision(0) << "\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
    }

    param = d;
    return true;
}


bool FormatDEInt( std::string& out, const int num )
{
    if( num > 9999999 || num < -9999999 )
    {
        ERRMSG << "\n + [BUG] integer to format (" << num;
        cerr << ") exceeds 7-digit limitation of IGES format\n";
        return false;
    }

    ostringstream ostr;
    ostr << num;

    out.clear();
    size_t len = ostr.str().length();

    if( len < 8 )
        out.append( 8 - len, ' ' );

    out.append( ostr.str() );

    return true;
}


// format a real number as a float or double and tack on a delimeter (may be PD or RD)
bool FormatPDREal( std::string &tStr, double var, char delim, double minRes )
{
    if( 0 >= minRes )
    {
        ERRMSG << "\n + [BUG] minRes <= 0\n";
        return false;
    }

    double vlim = var / minRes;

    if( vlim < 0.0 )
        vlim = -vlim;

    if( vlim < 10.0 )
        vlim = 10.0;

    // estimate the exponent size
    double ne = var;

    if( ne < 0.0 )
        ne = -ne;

    if( ne < 1e-8 )
    {
        ne = 1.0;
        var = 0.0;
    }
    else
    {
        ne = log(ne) / 2.3025850929940457;
    }

    // estimate the number of digits required to represent a number
    // to the stated minimum; throw in 4 extra digits to ensure
    // rounding errors do not result in input errors when reading a file
    // with an extent from ~2000 to 1e-8 units.
    int nc = (int)(log(vlim)/ 2.3025850929940457 + 1.00000000000001) + 4;
    ostringstream ostr;

    if( nc > 16 )
        nc = 16;

    // if magnitudes are big enough then switch to scientific notation
    if( var > 999.9 || var < -999.9 )
        ostr << scientific;

    // if magnitudes are small enough then switch to scientific notation
    if( var > -0.00001 && var < -0.00001 )
        ostr << scientific;

    ostr.precision( nc );
    ostr << var;

    // trim off any excess to ensure the most compact notation
    tStr = ostr.str();

    size_t pdot = tStr.find_first_of( '.' );
    size_t pexp = tStr.find_first_of( "eE" );
    size_t pidx;

    if( pdot != string::npos )
    {
        // strip any zeroes if we can
        if( pexp != string::npos )
            pidx = pexp - 1;
        else
            pidx = tStr.length() - 1;

        while( tStr[pidx] == '0' )
            --pidx;

        // don't eat up the first zero to the right of the dot
        if( tStr[pidx] == '.' )
            ++pidx;

        if( pexp != string::npos )
        {
            if( 0 )
            {
                if( nc > 7 || ne < -38.0 || ne > 38.0 )
                    tStr[pexp] = 'D'; // change exponent to 'D' (double)
                else
                    tStr[pexp] = 'E'; // change exponent to 'E' (float)
            }

            // note: according to the specification 'D' shall be used
            // for doubles and 'E' for single floats; however many
            // MCAD packages do not work correctly with 'D' so we
            // shall only output 'E'. The input parser however is
            // tolerant of the 'D' notation.
            tStr[pexp] = 'E'; // change exponent to 'E' (float)
            tStr = tStr.substr( 0, pidx + 1 ) + tStr.substr( pexp );
        }
        else
        {
            tStr = tStr.substr( 0, pidx + 1 );
        }

    }
    else
    {
        // Note: as per specification, the 'E' or the '.' may
        // be missing, but not both
        if( pexp == string::npos )
        {
            tStr += ".0";
        }
        else
        {
            if( nc > 7 )
                tStr[pexp] = 'D'; // change exponent to 'D' (double)
            else
                tStr[pexp] = 'E'; // change exponent to 'E' (float)
        }
    }

    tStr += delim;

    return true;
}

// tack the delimited section item tStr onto fStr and when appropriate update fOut and index;
// if the delimeter of tStr == rd then the PD entry is finalized
bool AddPDItem( std::string& tStr, std::string& fStr, std::string& fOut,
                int& pdIndex, int deIndex, char pd, char rd )
{
    if( tStr.length() > 64 )
    {
        ERRMSG << "\n + [BUG] parameter length exceeds max. permissible by IGES specification\n";
        return false;
    }

    if( fStr.length() > 64 )
    {
        ERRMSG << "\n + [BUG] PD entry exceeds max. permissible by IGES specification\n";
        return false;
    }

    if( fStr.length() + tStr.length() > 64 )
    {
        int len = 64 - fStr.length();

        if( len > 0 )
            fStr.append( len, ' ' );

        // add sequence number
        std::string seq;

        if( !FormatDEInt( seq, deIndex ) )
        {
            ERRMSG << "\n + [BUG] cannot tack on Sequence Number\n";
            return false;
        }

        fStr += seq;

        // add PD Sequence Number
        if( !FormatDEInt( seq, pdIndex ) )
        {
            ERRMSG << "\n + [BUG] cannot tack on PD Sequence Number\n";
            return false;
        }

        seq[0] = 'P';
        fStr += seq;
        fOut += fStr;
        fOut += "\n";
        fStr.clear();
        ++pdIndex;
    }

    // tack tStr onto fStr
    fStr += tStr;

    if( tStr[tStr.length() -1] == rd )
    {
        // this is the final entry
        int len = 64 - fStr.length();

        if( len > 0 )
            fStr.append( len, ' ' );

        // add sequence number
        std::string seq;

        if( !FormatDEInt( seq, deIndex ) )
        {
            ERRMSG << "\n + [BUG] cannot tack on Sequence Number\n";
            return false;
        }

        fStr += seq;

        // add PD Sequence Number
        if( !FormatDEInt( seq, pdIndex ) )
        {
            ERRMSG << "\n + [BUG] cannot tack on PD Sequence Number\n";
            return false;
        }

        seq[0] = 'P';
        fStr += seq;
        fOut += fStr;
        fOut += "\n";
        fStr.clear();
        ++pdIndex;
    }

    tStr.clear();
    return true;
}


// Add an item to the Global Section entry
bool AddSecItem( std::string& tStr, std::string& fStr, std::string& fOut,
                 int& index, char pd, char rd )
{
    if( tStr.empty() )
    {
        ERRMSG << "\n + [BUG] item (tStr) is a null string\n";
        return false;
    }

    char tc = tStr[tStr.length() -1];

    if( tc != pd && tc != rd )
    {
        ERRMSG << "\n + [BUG] delimeter at end of item (" << tc;
        cerr << ") is neither pd (" << pd << ") nor rd (" << rd << ")\n";
        return false;
    }

    if( tStr.length() > 72 )
    {
        ERRMSG << "\n + [BUG] item length exceeds max. permissible by IGES specification\n";
        return false;
    }

    if( fStr.length() > 72 )
    {
        ERRMSG << "\n + [BUG] global entry exceeds max. permissible by IGES specification\n";
        return false;
    }

    if( fStr.length() + tStr.length() > 72 )
    {
        int len = 72 - fStr.length();

        if( len > 0 )
            fStr.append( len, ' ' );

        // add global section sequence number
        std::string seq;

        if( !FormatDEInt( seq, index ) )
        {
            ERRMSG << "\n + [BUG] cannot tack on Global Sequence Number\n";
            return false;
        }

        seq[0] = 'G';
        fStr += seq;
        fOut += fStr;
        fOut += "\n";
        fStr.clear();
        ++index;
    }

    // tack tStr onto fStr
    fStr += tStr;

    if( tStr[tStr.length() -1] == rd )
    {
        // this is the final entry
        int len = 72 - fStr.length();

        if( len > 0 )
            fStr.append( len, ' ' );

        // add sequence number
        std::string seq;

        if( !FormatDEInt( seq, index ) )
        {
            ERRMSG << "\n + [BUG] cannot tack on Global Sequence Number\n";
            return false;
        }

        seq[0] = 'G';
        fStr += seq;
        fOut += fStr;
        fOut += "\n";
        fStr.clear();
        ++index;
    }

    tStr.clear();
    return true;
}


// get the Hollerith constant of a given string
bool GetHConst( const std::string& tStr, std::string& hConst )
{
    if( tStr.empty() )
        return false;

    ostringstream ostr;
    ostr << tStr.length() << "H";
    hConst = ostr.str();

    return true;
}

// convert the string in tStr to a Hollerith string and append to the Global Section fOut
bool AddSecHStr( const std::string& tStr, std::string& fStr, std::string& fOut,
                 int& index, char pd, char rd, char delim )
{
    std::string hc;
    std::string tmp;

    if( tStr.empty() )
    {
        tmp = delim;

        if( !AddSecItem( tmp, fStr, fOut, index, pd, rd ) )
        {
            ERRMSG << "\n + [BUG] could not add defaulted Hollerith string\n";
            return false;
        }

        return true;
    }

    if( delim != pd && delim != rd )
    {
        ERRMSG << "\n + [BUG] 'delim' (" << delim << ") is neither a parameter (";
        cerr << pd << ") nor record (" << rd << ") delimeter\n";
        return false;
    }

    if( !GetHConst( tStr, hc ) )
    {
        ERRMSG << "\n + [BUG] could not retrieve Hollerith constant for '";
        cerr << tStr << "'\n";
        return false;
    }

    // note: as per spec, if the Hollerith constant cannot fit on
    // the current line then it must be placed on the next line.
    if( fStr.length() + hc.length() > 72 )
    {
        fStr.append( 72 - fStr.length(), ' ' );

        if( !FormatDEInt( tmp, index++ ) )
        {
            ERRMSG << "\n + [BUG] could not format global section sequence number\n";
            return false;
        }

        tmp[0] = 'G';
        fStr += tmp + "\n";
        fOut += fStr;
        fStr.clear();
    }

    // concatenate the Hollerith string
    hc += tStr + delim;

    size_t sz = hc.length();
    size_t sidx;

    for( sidx = 0; sidx < sz; )
    {
        tmp = hc.substr( sidx, 72 - fStr.length() );
        sidx += tmp.length();

        fStr += tmp;

        if( fStr.length() == 72 )
        {
            if( !FormatDEInt( tmp, index++ ) )
            {
                ERRMSG << "\n + [BUG] could not format global section sequence number\n";
                return false;
            }

            tmp[0] = 'G';
            fStr += tmp + "\n";
            fOut += fStr;
            fStr.clear();
        }
    }

    // check if we should finalize the entry
    if( fStr.length() && delim == rd )
    {
        if( fStr.length() != 72 )
            fStr.append( 72 - fStr.length(), ' ' );

        if( !FormatDEInt( tmp, index++ ) )
        {
            ERRMSG << "\n + [BUG] could not format global section sequence number\n";
            return false;
        }

        tmp[0] = 'G';
        fStr += tmp + "\n";
        fOut += fStr;
        fStr.clear();
    }

    return true;
}
