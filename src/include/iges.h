/*
 * file: iges.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES top level object for input, output and manipulation
 * of IGES entity data.
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

#ifndef IGES_H
#define IGES_H

// NOTE: This file should describe the IGES superclass which can be used to
// read/write data and instantiate an IGES model.

#include <list>
#include <string>
#include <vector>
#include "iges_base.h"
#include "iges_entity.h"

struct IGES_GLOBAL
{
    char        pdelim;                     // parameter delimeter; RD ','
    char        rdelim;                     // record delimeter; RD ';'
    std::string productIDSS;                // Product Identification used by Sending System (RN)
    std::string fileName;                   // Name of this file (must match in order to support external refs.) (RN)
    std::string nativeSystemID;             // Name of the software which created the IGES model (RN)
    std::string preprocessorVersion;        // Version of the Preprocessor which created the IGES model (RN)
    int         nIntegerBits;               // number of significant bits in an integer on the Sending System (RN)
    int         floatMaxExp;                // max. exponent of a Float type (RN)
    int         floatMaxSig;                // max. significant digits of a Float type (RN)
    int         doubleMaxExp;               // max. exponent of a Double type (RN)
    int         doubleMaxSig;               // max. significant digits of a Double type (RN)
    std::string productIDRS;                // Product Identification used by Receiving System (RD: product IDSS)
    double      modelScale;                 // ModelUnits/RealWorldUnits : Ex. a 1:8 model must multiply units by 8 to get RWU (RD: 1.0)
    IGES_UNIT   unitsFlag;                  // Internal unit representation of model on disk (RD: 1 (inch))
    std::string unitsName;                  // String symbolizing units described by unitsFlag (RD: must match unitsFlag)
    int         maxLinewidthGrad;           // Max. linewidth gradations (RD: 1; min. = 1)
    double      maxLinewidth;               // Max. linewidth in terms of internal units (RN)
    std::string creationDate;               // [YY]YYMMDD.HHNNSS date of file creation (enforce UTC) (RN)
    double      minResolution;              // Min. user intended resolution (RN)
    double      maxCoordinateValue;         // Max. used coordinate value (normally set to '0.0' = not determined) (RD: 0.0)
    std::string author;                     // name of author (RD: "")
    std::string organization;               // name of author's organization (RD: "")
    int         igesVersion;                // flag indicating IGES version of the file; 3..11 (RD: 3)
    IGES_DRAFTING_STANDARD draftStandard;   // flag indicating drafting standard (if any) (RD: 0)
    std::string modificationDate;           // [YY]YYMMDD.HHNNSS date of file creation/modification (RD: creationDate)
    std::string applicationNote;            // Application Protocol, Application Subset, MIL-STD-SPEC, User Protocol, etc (RD: "")

    // XXX - TO BE IMPLEMENTED: routines to read/write
};


class IGES
{
private:
    std::list<std::string> startSection;    // text from the Start section
    int                    nStartSecLines;  // number of lines in the Start section

    int                    nGlobSecLines;   // number of lines in the Global section
    int                    nDESecLines;     // number of lines in the Directory Entry section
    int                    nPDSecLines;     // number of lines in the Parameter Data section

    std::vector<IGES_ENTITY*> entities;     // all existing IGES entities and their data

    // XXX - TO BE IMPLEMENTED
    // associate: associate pointers with other entities after reading all data; retrictions on types
    //            must be enforced to ensure data integrity and software stability
    // prepare: prepare data for writing; Parameter Data is formatted using the given index;
    //          each Entity must have been previously assigned a correct Sequence Number

public:
    struct IGES_GLOBAL     globalData;      // Global Section data

    // bool Read/Write: read and write entity data
    // bool export(IGES*): Export all entities to the given IGES* (to be used for creating Assemblies)
    // GetGlobals(): return * to globs for purposes of merging other files
    // NewEntity(Type)
    // AddEntity(Entity *)  -- Add an entity from another IGES object or an entity created without NewEntity()
    // DelEntity(Entity *)  -- Delete an entity and any dependent children

};


#endif  // IGES_H
