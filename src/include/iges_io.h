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
bool ReadIGESRecord(IGES_RECORD* aRecord, std::ifstream& aFile);

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
