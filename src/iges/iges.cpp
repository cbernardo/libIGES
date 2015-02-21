/*
 * file: iges.cpp
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

#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <all_entities.h>

using namespace std;

static std::string UNIT_NAMES[UNIT_END] =
{
    "IN",
    "MM",
    "CUST",
    "FT",
    "MI",
    "M",
    "KM",
    "MIL",
    "UM",
    "CM",
    "UIN"
};

IGES::IGES()
{
    init();
    return;
}   // IGES()


IGES::~IGES()
{
    // XXX - TO BE IMPLEMENTED
    return;
}

bool IGES::init(void)
{
    if( !entities.empty() )
    {
        ERRMSG << "\n + [BUG] init() invoked while entity list was not empty\n";
        return false;
    }

    globalData.pdelim = ',';
    globalData.rdelim = ';';
    globalData.nIntegerBits = 0;
    globalData.floatMaxExp = 0;
    globalData.floatMaxSig = 0;
    globalData.doubleMaxExp = 0;
    globalData.doubleMaxSig = 0;
    globalData.modelScale = 1.0;
    globalData.unitsFlag = UNIT_INCH;
    globalData.maxLinewidthGrad = 1;
    globalData.maxLinewidth = 0;
    globalData.minResolution = 0;
    globalData.maxCoordinateValue = 0.0;
    globalData.igesVersion = 3;
    globalData.draftStandard = DRAFT_NONE;

    globalData.productIDSS.clear();
    globalData.fileName.clear();
    globalData.nativeSystemID.clear();
    globalData.preprocessorVersion.clear();
    globalData.productIDRS.clear();
    globalData.unitsName.clear();
    globalData.creationDate.clear();
    globalData.author.clear();
    globalData.organization.clear();
    globalData.modificationDate.clear();
    globalData.applicationNote.clear();

    startSection.clear();
    nGlobSecLines = 0;
    nDESecLines = 0;
    nPDSecLines = 0;

    return true;
}


// delete all entities and reinitialize global data
bool IGES::Clear( void )
{
    if( !entities.empty() )
    {
        size_t maxe = entities.size();

        for( size_t i = 0; i < maxe; ++ i )
            delete entities[i];

        entities.clear();
    }

    init();
    return false;
}


// open and read the file with the given name
bool IGES::Read( const char* aFileName )
{
    // XXX - Set the LOCALE so that we get correct translation of floats;
    // use the "C" locale - see KiCad VRML export for clues.
    if( !aFileName )
    {
        ERRMSG << "\n + [BUG] null pointer passed for filename\n";
        return false;
    }

    if( !entities.empty() )
    {
        ERRMSG << "\n + [BUG] function invoked while entities were instantiated\n";
        cerr << " + invoke Clear() function before reading a new file\n";
        return false;
    }

    ifstream file;

    file.open( aFileName, ios::in );

    if( !file.is_open() )
    {
        ERRMSG << "\n + [INFO] could not open file\n";
        cerr << " + filename: '" << aFileName << "'\n";
        return false;
    }

    // read the FLAG/START section
    IGES_RECORD rec;

    if( !ReadIGESRecord( &rec, file ) )
    {
        ERRMSG << "\n + [INFO] could read file\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    if( rec.section_type == 'F' )
    {
        ERRMSG << "\n + [INFO] files with a FLAG section (compressed or binary format) are not supported.\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    if( rec.section_type != 'S' )
    {
        ERRMSG << "\n + [CORRUPT FILE] file does not contain a START section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    bool fOK = true;

    while( rec.section_type == 'S' && fOK )
    {
        if( rec.index != (int)(startSection.size() + 1) )
        {
            ERRMSG << "\n + [CORRUPT FILE] sequence number (" << rec.index;
            cerr << ") does not match expected (" << (startSection.size() + 1) << ")\n";
            cerr << " + filename: '" << aFileName << "'\n";
            file.close();
            Clear();
            return false;
        }

        startSection.push_back( rec.data );
        fOK = ReadIGESRecord( &rec, file );
    }

    if( !fOK )
    {
        ERRMSG << "\n + [INFO] problems reading file\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    if( rec.section_type != 'G' )
    {
        ERRMSG << "\n + [CORRUPT FILE] file does not contain a GLOBAL section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    // read the global section
    if( ! readGlobals( rec, file ) )
    {
        ERRMSG << "\n + [INFO] problems reading file GLOBAL section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    // XXX - Ideally we compare the filename with the name stored
    // in the IGES file (case insensitive to appease defective
    // file storage protocols). If the names are not the same.
    // print out a warning message. Keep in mind that the
    // name discrepancies may result in the failure of some
    // IGES implementations to correctly load externally
    // referenced files.

    // read the DE section
    if( rec.section_type != 'D' )
    {
        ERRMSG << "\n + [CORRUPT FILE] file does not contain a DIRECTORY section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    if( ! readDE( rec, file ) )
    {
        ERRMSG << "\n + [INFO] problems reading file DIRECTORY section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    // read the PE section
    if( rec.section_type != 'P' )
    {
        ERRMSG << "\n + [CORRUPT FILE] file does not contain a PARAMETER section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    if( ! readPD( rec, file ) )
    {
        ERRMSG << "\n + [INFO] problems reading file PARAMETER section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    // read the T section
    if( ! readTS( rec, file ) )
    {
        ERRMSG << "\n + [CORRUPT FILE]\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    // XXX - establish correct entity associations
    // XXX - cull unsupported entities

    // XXX - TO BE IMPLEMENTED
    return false;
}


// open a file with the given name and write out all data
bool IGES::Write( const char* aFileName, bool fOverwrite )
{
    // XXX - Set the LOCALE so that we get correct translation of floats;
    // use the "C" locale - see KiCad VRML export for clues.
    // XXX - TO BE IMPLEMENTED
    return false;
}


// create an entity of the given type
bool IGES::NewEntity( int aEntityType )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// add an entity from another IGES object or an entity created without NewEntity()
bool IGES::AddEntity( IGES_ENTITY* aEntity )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// delete an entity and any dependent children
bool IGES::DelEntity( IGES_ENTITY* aEntity )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES::readGlobals( IGES_RECORD& rec, std::ifstream& file )
{
    // on entry the record contains the first GLOBAL record entry
    std::string globs;
    bool fOK = true;

    while( rec.section_type == 'G' && fOK )
    {
        ++nGlobSecLines;

        if( rec.index !=  nGlobSecLines )
        {
            ERRMSG << "\n + [CORRUPT FILE] sequence number (" << rec.index;
            cerr << ") does not match expected (" << nGlobSecLines << ")\n";
            return false;
        }

        globs += rec.data;
        fOK = ReadIGESRecord( &rec, file );
    }

    if( !fOK )
    {
        ERRMSG << "\n + [INFO] could not read GLOBAL section\n";
        return false;
    }


    // G1: parameter delimeter: REQUIRED DEFAULT ','
    int idx = 0;
    char delim = globalData.pdelim;

    if( globs[idx] != ',' )
    {
        if( globs.compare(0, 2, "1H") )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid parameter delimeter\n";
            return false;
        }

        delim = globs[2];

        if( delim <= 0x20 || delim >= 0x7f || delim == ';'
            || delim == 0x2b || delim == 0x2d || delim == 0x2e
            || delim == 'D' || delim == 'E' || delim == 'H'
            || ( delim >= 0x30 && delim <= 0x39 ) )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid parameter delimeter\n";
            return false;
        }

        globalData.pdelim = delim;
        idx += 3;
    }

    if( globs[idx] != delim )
    {
        ERRMSG << "\n + [CORRUPT FILE] expecting delimeter '" << delim;
        cerr << "' but received '" << globs[idx] << "'\n";
        return false;
    }

    ++idx;
    char rdelim = globalData.rdelim;

    // G2: record delimeter: REQUIRED DEFAULT ';'
    if( globs[idx] != delim )
    {
        if( globs.compare(0, 2, "1H") )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid record delimeter\n";
            return false;
        }

        rdelim = globs[idx + 2];

        if( delim <= 0x20 || delim >= 0x7f || delim == ';'
            || delim == 0x2b || delim == 0x2d || delim == 0x2e
            || delim == 'D' || delim == 'E' || delim == 'H'
        || ( delim >= 0x30 && delim <= 0x39 ) )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid parameter delimeter\n";
            return false;
        }

        globalData.rdelim = rdelim;
        idx += 3;
    }

    if( globs[idx] != delim )
    {
        ERRMSG << "\n + [CORRUPT FILE] expecting delimeter '" << delim;
        cerr << "' but received '" << globs[idx] << "'\n";
        return false;
    }

    ++idx;

    // G3: Product ID, Sending System, REQUIRED NO DEFAULT
    bool eor = false;

    if( !ParseHString( globs, idx, globalData.productIDSS, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve ProductID of Sending System\n";
        return false;
    }
    else
    {
        if( globalData.productIDSS.empty() )
        {
            ERRMSG << "\n + [CORRUPT FILE] no ProductID of Sending System (defaults not permitted)\n";
            return false;
        }
    }

    // G4: Filename, REQUIRED NO DEFAULT
    if( !ParseHString( globs, idx, globalData.fileName, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve FileName\n";
        return false;
    }
    else
    {
        if( globalData.fileName.empty() )
        {
            ERRMSG << "\n + [CORRUPT FILE] no FileName (defaults not permitted)\n";
            return false;
        }
    }

    // G5: Native System ID, REQUIRED NO DEFAULT
    if( !ParseHString( globs, idx, globalData.nativeSystemID, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Native System ID\n";
        return false;
    }
    else
    {
        if( globalData.nativeSystemID.empty() )
        {
            ERRMSG << "\n + [CORRUPT FILE] no Native System ID (defaults not permitted)\n";
            return false;
        }
    }

    // G6: Preprocessor Version, REQUIRED NO DEFAULT
    if( !ParseHString( globs, idx, globalData.preprocessorVersion, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Preprocessor Version\n";
        return false;
    }
    else
    {
        if( globalData.nativeSystemID.empty() )
        {
            ERRMSG << "\n + [CORRUPT FILE] no Preprocessor Version (defaults not permitted)\n";
            return false;
        }
    }

    // G7: NBits for Integer representation, REQUIRED NO DEFAULT
    if( !ParseInt( globs, idx, globalData.nIntegerBits, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve # of bit for integer representation\n";
        return false;
    }
    else
    {
        if( globalData.nIntegerBits < 8 || globalData.nIntegerBits > 32 )
        {
            ERRMSG << "\n + [INFO] Sending System integers not supported by this library (#bits: ";
            cerr << globalData.nIntegerBits << ")\n";
            return false;
        }
    }

    // G8: Single Precision Magnitude, REQUIRED NO DEFAULT
    if( !ParseInt( globs, idx, globalData.floatMaxExp, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Single Precision Magnitude\n";
        return false;
    }
    else
    {
        // since we *only* use doubles for internal representation,
        // check that this number is <= MAX on a 64-bit IEEE float
        if( globalData.floatMaxExp < 4 || globalData.floatMaxExp > 307 )
        {
            ERRMSG << "\n + [INFO] Sending System floats not supported by this library (Max Mag: ";
            cerr << globalData.floatMaxExp << ")\n";
            return false;
        }
    }

    // G9: Single Precision Significance, REQUIRED NO DEFAULT
    if( !ParseInt( globs, idx, globalData.floatMaxSig, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Single Precision Significance\n";
        return false;
    }
    else
    {
        // since we *only* use doubles for internal representation,
        // check that this number is <= MAX on a 64-bit IEEE float
        if( globalData.floatMaxSig < 3 || globalData.floatMaxSig > 16 )
        {
            ERRMSG << "\n + [INFO] Sending System floats not supported by this library (Max Mag: ";
            cerr << globalData.floatMaxSig << ")\n";
            return false;
        }
    }

    // G10: Double Precision Magnitude, REQUIRED NO DEFAULT
    if( !ParseInt( globs, idx, globalData.doubleMaxExp, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Double Precision Magnitude\n";
        return false;
    }
    else
    {
        // since we *only* use doubles for internal representation,
        // check that this number is <= MAX on a 64-bit IEEE float
        if( globalData.doubleMaxExp < 4 || globalData.doubleMaxExp > 307 )
        {
            ERRMSG << "\n + [INFO] Sending System doubles not supported by this library (Max Mag: ";
            cerr << globalData.doubleMaxExp << ")\n";
            return false;
        }
    }

    // G11: Double Precision Significance, REQUIRED NO DEFAULT
    if( !ParseInt( globs, idx, globalData.doubleMaxSig, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Double Precision Significance\n";
        return false;
    }
    else
    {
        // since we *only* use doubles for internal representation,
        // check that this number is <= MAX on a 64-bit IEEE float
        if( globalData.doubleMaxSig < 3 || globalData.doubleMaxSig > 16 )
        {
            ERRMSG << "\n + [INFO] Sending System doubles not supported by this library (Max Mag: ";
            cerr << globalData.doubleMaxSig << ")\n";
            return false;
        }
    }

    // G12: Product ID, Receiving System, REQUIRED, DEFAULT = ProductID,SS
    if( !ParseHString( globs, idx, globalData.productIDRS, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve ProductID of Receiving System\n";
        return false;
    }
    else if( globalData.productIDSS.empty() )
    {
        globalData.productIDRS = globalData.productIDSS;
    }

    // G13: Model Space Scale, REQUIRED, DEFAULT = 1.0;
    double rdefault = 1.0;

    if( !ParseReal( globs, idx, globalData.modelScale, eor, delim, rdelim, &rdefault ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Model Space Scale\n";
        return false;
    }

    // G14: Model Units
    int tint;
    int idefault = 1;

    if( !ParseInt( globs, idx, tint, eor, delim, rdelim, &idefault ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Model Units\n";
        return false;
    }

    if( tint < UNIT_INCH || tint > UNIT_MICROINCH )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Model Units (" << tint << ")\n";
        return false;
    }

    globalData.unitsFlag = (IGES_UNIT)tint;

    // G15: Units Name
    if( !ParseHString( globs, idx, globalData.unitsName, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Units Name\n";
        return false;
    }
    else if( globalData.unitsName.empty() )
    {
        if( globalData.unitsFlag == UNIT_EXTERN )
        {
            ERRMSG << "\n + [CORRUPT FILE] no provided Unit Name for UNIT_EXTERN\n";
            return false;
        }
    }
    else if( globalData.unitsFlag != UNIT_EXTERN )
    {
        if( globalData.unitsName.compare( UNIT_NAMES[globalData.unitsFlag] ) )
            globalData.unitsName = UNIT_NAMES[globalData.unitsFlag];
    }

    // G16: qwerty

    // XXX - TO BE IMPLEMENTED: Process the globals

    return false;
}


bool IGES::readDE( IGES_RECORD& rec, std::ifstream& file )
{
    // on entry the record contains the first DIRECTORY ENTRY record
    std::streampos pos;

    // XXX - TO BE IMPLEMENTED

    // on exit the file must be rewound to the start of the first PD line
    return false;
}


bool IGES::readPD( IGES_RECORD& rec, std::ifstream& file )
{
    // on entry the record contains the first PARAMETER DATA record
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES::readTS( IGES_RECORD& rec, std::ifstream& file )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}
