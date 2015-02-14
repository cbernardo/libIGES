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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef IGES_IO_H
#define IGES_IO_H

// a single-line data record
struct IGES_RECORD
{
    std::string data;           // data section (columns 1..72)
    char        section_type;   // column  73
    int         index;          // columns 74..80
};


// XXX - move this elsewhere; the end user never has to see it
class IGES_DATUM
{
    IGES_TYPE type;
    int       flags;

public:
    IGES_DATUM();
    IGES_DATUM(IGES_TYPE aType, int aFlag, );
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

    IGES_TYPE GetType(void);
    bool      SetType(IGES_TYPE aType);

    int       GetFlags(void);
    void      SetFlags(int aFlag);

    // XXX - routines to read and write?
};

#endif  // IGES_IO_H
