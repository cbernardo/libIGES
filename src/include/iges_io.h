/*
 * file: iges_io.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: internal structures to aid in IGES file I/O.
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

#ifndef IGES_IO_H
#define IGES_IO_H

#include <string>
#include <iges_base.h>

// Extract an item from a Directory Entry record and convert to an integer.
// Note: these functions assume an input string which is a multiple of 8 chars;
// fields must be right-aligned.
// input: the Directory Entry Record (or any generic string)
// field: the Field Number within the record (0 .. 9)
// var: the variable to store the result
// defaulted: pointer to a variable with a default value if the variable may be defaulted
bool DEItemToInt( const std::string& input, int field, int& var, int* defaulted = NULL );

// extract an item from a Directory Entry record and convert to a normal string
// Note: the IGES specification does not preclude trailing spaces within strings
// in the DE.
bool DEItemToStr( const std::string& input, int field, std::string& var );

struct IGES_RECORD;
// read a single line of the IGES file and parse into the record fields
bool ReadIGESRecord( IGES_RECORD* aRecord, std::ifstream& aFile, std::streampos* aRefPos = NULL );

// parse a free-form Hollerith string and return true on success
// idx: contains an index to the starting position in the data stream; returns index just past
//      the parameter or record delimeter
// param: will contain the parsed string, if any
// eor: set to true if the record delimeter has been encountered
bool ParseHString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd );

// parse a free-form LanguageString (a generic string up to the first encountered delimeter )
bool ParseLString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd );

// parse a free-form string to retrieve an integer
bool ParseInt( const std::string& data, int& idx, int& param, bool& eor, char pd, char rd, int* idefault = NULL );

// parse a free-form string to retrieve a floating point number
bool ParseReal( const std::string& data, int& idx, double& param, bool& eor, char pd, char rd, double* ddefault = NULL );

// format and right-justify an integer; pad to 8 characters using spaces
bool FormatDEInt( std::string& out, const int num );

// format a real number as a float or double and tack on a delimeter (may be PD or RD)
bool FormatPDREal( std::string &tStr, double var, char delim, double minRes );

// tack the delimited PD Item tStr onto fStr and when appropriate update fOut and index;
// if the delimeter of tStr == rd then the PD entry is finalized
bool AddPDItem( std::string& tStr, std::string& fStr, std::string& fOut,
                int& index, int sequenceNumber, char pd, char rd );


// a single-line data record
struct IGES_RECORD
{
    std::string data;           // data section (columns 1..72)
    char        section_type;   // column  73
    int         index;          // columns 74..80
};


// XXX - move this elsewhere; the end user never has to see it
// Flags: determines whether the item is Required or Optional,
// Defaulted or non-defaulted type, and whether the item read
// was defaulted.
class IGES_DATUM
{
    IGES_TYPE type;
    int       flags;

public:
    IGES_DATUM();
    virtual ~IGES_DATUM();

    union
    {
        int i;          // Integer
        float f;        // Real (E)
        double d;       // Real (D)
        int p;          // IGES pointer
        bool b;         // Logical
        std::string* s; // Language String or Hollerith String
    } data;

    IGES_TYPE GetType( void );
    bool      SetType( IGES_TYPE aType );

    int       GetFlags( void );
    void      SetFlags( int aFlag );

    // XXX - routines to read and write?
    // bool Read(const std::string&, IGES_TYPE, FLAGS&, {DEFAULT})
};

#endif  // IGES_IO_H
