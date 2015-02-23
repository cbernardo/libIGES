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

#include <locale.h>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <all_entities.h>

using namespace std;

// Note: a default of 11 = IGES5.3
#define DEFAULT_IGES_VERSION (11)


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


// This class magically manages switching between the C locale and
// the user's locale
class IGES_LOCALE
{
public:
    IGES_LOCALE()
    {
        setlocale( LC_NUMERIC, "C" );   // switch the numerics locale to "C"
    }

    ~IGES_LOCALE()
    {
        setlocale( LC_NUMERIC, "" );    // revert to the current numerics default locale
    }
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
    IGES_LOCALE igloc;

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
        ERRMSG << "\n + [INFO] could not read file\n";
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

    // read the PD section
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
        ERRMSG << "\n + [CORRUPT FILE] could not read Terminate Section\n";
        cerr << " + filename: '" << aFileName << "'\n";
        file.close();
        Clear();
        return false;
    }

    // Associate entities
    size_t nEnt = entities.size();
    size_t iEnt;

    for( iEnt = 0; iEnt < nEnt; ++iEnt )
    {
        if( !entities[iEnt]->associate( &entities ) )
        {
            ERRMSG << "\n + [INFO] could not establish file associations\n";
            return false;
        }
    }

    // cull unsupported entities
    int nCulled = 0;
    std::vector<IGES_ENTITY*> tmpEnts;

    for( iEnt = 0; iEnt < nEnt; ++iEnt )
    {
        if( entities[iEnt]->IsOrphaned() )
        {
            ++nCulled;
            delete entities[iEnt];
        }
        else
        {
            tmpEnts.push_back( entities[iEnt] );
        }
    }

    entities = tmpEnts;

#ifdef DEBUG
    cout << " + [INFO] Entities culled: " << nCulled << "\n";
    cout << " + [INFO] Entities remaining: " << entities.size() << "\n";
#endif

    return true;
}


// open a file with the given name and write out all data
bool IGES::Write( const char* aFileName, bool fOverwrite )
{
    IGES_LOCALE igloc;

    // XXX - TO BE IMPLEMENTED
    return false;
}


// create an entity of the given type
bool IGES::NewEntity( int aEntityType, IGES_ENTITY** aEntityPointer )
{
    IGES_ENTITY* ep = NULL;
    *aEntityPointer = NULL;

    switch( aEntityType )
    {
        case ENT_CIRCULAR_ARC:
            ep = new IGES_ENTITY_100( this );
            break;

        default:
            ep = new IGES_ENTITY_NULL( this );
            ((IGES_ENTITY_NULL*)ep)->setEntityType( aEntityType );
            break;
    }

    if( !ep )
    {
        ERRMSG << "\n + [INFO] could not create a new IGES_ENTITY with ID ";
        cerr << aEntityType << "\n";
        return false;
    }

    *aEntityPointer = ep;
    entities.push_back( ep );
    return true;
}


// add an entity from another IGES object or an entity created without NewEntity()
bool IGES::AddEntity( IGES_ENTITY* aEntity )
{
    if( !aEntity )
    {
        ERRMSG << "\n + [BUG] AddEntity() invoked with NULL argument\n";
        return false;
    }

    std::vector<IGES_ENTITY*>::iterator sEnt = entities.begin();
    std::vector<IGES_ENTITY*>::iterator eEnt = entities.end();

    while( sEnt != eEnt )
    {
        if( *sEnt == aEntity )
            return true;

        ++sEnt;
    }

    entities.push_back( aEntity );

    return true;
}


// delete an entity
bool IGES::DelEntity( IGES_ENTITY* aEntity )
{
    if( !aEntity )
    {
        ERRMSG << "\n + [BUG] DelEntity() invoked with NULL argument\n";
        return false;
    }

    std::vector<IGES_ENTITY*>::iterator sEnt = entities.begin();
    std::vector<IGES_ENTITY*>::iterator eEnt = entities.end();

    while( sEnt != eEnt )
    {
        if( *sEnt == aEntity )
        {
            delete *sEnt;
            entities.erase( sEnt );
            return true;
        }

        ++sEnt;
    }

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
        if( globalData.floatMaxExp < 4 || globalData.floatMaxExp > 308 )
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
        if( globalData.doubleMaxExp < 4 || globalData.doubleMaxExp > 308 )
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

    // G16: Max. Number of LineWidth Gradations
    if( !ParseInt( globs, idx, globalData.maxLinewidthGrad, eor, delim, rdelim, &idefault ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Max. Linewidth Gradations\n";
        return false;
    }

    if( globalData.maxLinewidthGrad < 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Max. Linewidth Gradations (";
        cerr << globalData.maxLinewidthGrad << ")\n";
        return false;
    }

    // G17: Max. Line Width
    if( !ParseReal( globs, idx, globalData.maxLinewidth, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Max. Linewidth\n";
        return false;
    }

    if( globalData.maxLinewidth < 0.0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Max. Linewidth (";
        cerr << globalData.maxLinewidthGrad << ")\n";
        return false;
    }

    // G18: Creation Date
    if( !ParseHString( globs, idx, globalData.creationDate, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve file creation date\n";
        return false;
    }
    else if( globalData.creationDate.empty() )
    {
        ERRMSG << "\n + [CORRUPT FILE] empty creation date\n";
        return false;
    }

    // XXX - ideally we should parse the string to ensure it's a valid date
    if( globalData.creationDate.length() != 13
        && globalData.creationDate.length() != 15 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid creation date '";
        cerr << globalData.creationDate << "'\n";
        return false;
    }

    // G19: Min. User-intended resolution
    if( !ParseReal( globs, idx, globalData.minResolution, eor, delim, rdelim, NULL ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve min. user-intended resolution\n";
        return false;
    }

    if( globalData.minResolution <= 0.0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid min. user-intended resolution (";
        cerr << globalData.minResolution << ")\n";
        return false;
    }

    // G20: Approx. Max. Coordinate, REQUIRED DEFAULT 0
    if( eor )
    {
        globalData.maxCoordinateValue = 0.0;
    }
    else
    {
        rdefault = 0.0;

        if( !ParseReal( globs, idx, globalData.maxCoordinateValue, eor, delim, rdelim, &rdefault ) )
        {
            ERRMSG << "\n + [CORRUPT FILE] could not retrieve approx. max coordinate\n";
            return false;
        }

        if( globalData.maxCoordinateValue < 0.0 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid max. coordinate value (";
            cerr << globalData.maxCoordinateValue << ")\n";
            return false;
        }
    }

    // G21: Author, REQUIRED, DEFAULT NULL
    if( eor )
    {
        globalData.author.clear();
    }
    else if( !ParseHString( globs, idx, globalData.author, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Author\n";
        return false;
    }

    // G22: Organization, REQUIRED, DEFAULT NULL
    if( eor )
    {
        globalData.organization.clear();
    }
    else if( !ParseHString( globs, idx, globalData.organization, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve Organization\n";
        return false;
    }

    // G23: Version Flag, REQUIRED DEFAULT 3
    if( eor )
    {
        globalData.igesVersion = 3;
    }
    else
    {
        idefault = DEFAULT_IGES_VERSION;

        if( !ParseInt( globs, idx, globalData.igesVersion, eor, delim, rdelim, &idefault ) )
        {
            ERRMSG << "\n + [CORRUPT FILE] could not retrieve IGES version\n";
            return false;
        }

        if( globalData.igesVersion < 3 )
            globalData.igesVersion = 3;
        else if( globalData.igesVersion > 11 )    /* this should never happen since IGES is an unmaintained standard */
            globalData.igesVersion = 11;

    }

    // G24: Drafting Standard, REQUIRED DEFAULT 0
    if( eor )
    {
        globalData.draftStandard = DRAFT_NONE;
    }
    else
    {
        idefault = 0;

        if( !ParseInt( globs, idx, tint, eor, delim, rdelim, &idefault ) )
        {
            ERRMSG << "\n + [CORRUPT FILE] could not retrieve Drafting Standard\n";
            return false;
        }

        if( tint < DRAFT_NONE || tint > DRAFT_JIS )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid Drafting Standard (";
            cerr << tint << ")\n";
        }

        globalData.draftStandard = (IGES_DRAFTING_STANDARD)tint;
    }

    // G25: Modification Date, REQUIRED DEFAULT NULL
    if( eor )
    {
        globalData.modificationDate.clear();
    }
    else
    {
        if( !ParseHString( globs, idx, globalData.modificationDate, eor, delim, rdelim ) )
        {
            ERRMSG << "\n + [CORRUPT FILE] could not retrieve file creation date\n";
            return false;
        }

        // XXX - ideally we should parse the string to ensure it's a valid date
        if( globalData.modificationDate.length() > 0 && globalData.modificationDate.length() != 13
            && globalData.modificationDate.length() != 15 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid modification date '";
            cerr << globalData.modificationDate << "'\n";
            return false;
        }
    }

    // G26: Application Protocol / Subset Identifier, REQUIRED DEFAULT NULL
    if( eor )
    {
        globalData.applicationNote.clear();
        return true;
    }

    if( !ParseHString( globs, idx, globalData.applicationNote, eor, delim, rdelim ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not retrieve AP / Subset Identifier string\n";
        return false;
    }

    if( !eor )
    {
        ERRMSG << "\n + [CORRUPT FILE] no end-of-record marker found in Global Section\n";
        return false;
    }

    return true;
}


bool IGES::readDE( IGES_RECORD& rec, std::ifstream& file )
{
    // on entry the record contains the first DIRECTORY ENTRY record
    std::streampos pos;

    if( rec.index != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] first DE sequence is not 1 (received: ";
        cerr << rec.index << ")\n";
        return false;
    }

    int tmpInt;
    IGES_ENTITY* ep;

    while( rec.section_type == 'D' )
    {
        if( !DEItemToInt( rec.data, 0, tmpInt, NULL) )
        {
            ERRMSG << "\n + could not extract Entity Type number\n";
            return false;
        }

        if( !NewEntity( tmpInt, &ep ) )
        {
            ERRMSG << "\n + could not create Entity #" << tmpInt << "\n";
            return false;
        }

        if( !ep->ReadDE( &rec, file, nDESecLines ) )
        {
            ERRMSG << "\n + [INFO] could not read Directory Entry\n";
            return false;
        }

        // read the first line of the next DE
        if( !ReadIGESRecord( &rec, file, &pos ) )
        {
            ERRMSG << "\n + [INFO] could not read subsequent IGES record\n";
            return false;
        }
    }

    // on exit the file must be rewound to the start of the first PD line
    // reset the file pointer to the previous line
    if( file.bad() )
        file.clear();

    file.seekg( pos );

    if( file.bad() )
    {
        ERRMSG << "\n + [INFO] could not rewind the file stream\n";
        return false;
    }

    return true;
}


bool IGES::readPD( IGES_RECORD& rec, std::ifstream& file )
{
    // on entry the record contains the first PARAMETER DATA record
    // but the stream should have been rewound to the start of that
    // line

    std::vector<IGES_ENTITY*>::iterator sEnt = entities.begin();
    std::vector<IGES_ENTITY*>::iterator eEnt = entities.end();
    size_t i = 0;

    while( sEnt != eEnt )
    {
        if( !(*sEnt)->ReadPD( file, nPDSecLines ) )
        {
            ERRMSG << "\n + [INFO] could not read parameter data for Entity[";
            cerr << i << "]\n";
            return false;
        }

        ++i;
        ++sEnt;
    }

    return true;
}


bool IGES::readTS( IGES_RECORD& rec, std::ifstream& file )
{
    if( !ReadIGESRecord( &rec, file ) )
    {
        ERRMSG << "\n + [INFO] could not read Terminate Section from file\n";
        return false;
    }

    if( rec.section_type != 'T' )
    {
        ERRMSG << "\n + [CORRUPT FILE] expecting Terminate Section 'T', received '";
        cerr << rec.section_type << "'\n";
        return false;
    }

    if( rec.index != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected Sequence Number '1', got '";
        cerr << rec.index << "'\n";
        return false;
    }

    if( rec.data[0] != 'S' )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected 'S' in column 1, got '";
        cerr << rec.data[0] << "'\n";
        return false;
    }

    if( rec.data[8] != 'G' )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected 'G' in column 9, got '";
        cerr << rec.data[8] << "'\n";
        return false;
    }

    if( rec.data[16] != 'D' )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected 'D' in column 17, got '";
        cerr << rec.data[16] << "'\n";
        return false;
    }

    if( rec.data[24] != 'P' )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected 'P' in column 25, got '";
        cerr << rec.data[24] << "'\n";
        return false;
    }

    rec.data[0] = 32;
    rec.data[8] = 32;
    rec.data[16] = 32;
    rec.data[24] = 32;

    // bool DEItemToInt( const std::string& input, int field, int& var, int* defaulted )
    int tmpInt;

    if( !DEItemToInt( rec.data, 0, tmpInt, NULL ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] no Start Sequence Count in Terminate Section\n";
        return false;
    }

    if( tmpInt != (int)startSection.size() )
    {
        ERRMSG << "\n + [INCONSISTENT FILE] file has " << startSection.size() << "lines ";
        cerr << "in the Start Section; Terminate Section reports " << tmpInt << "\n";
    }

    if( !DEItemToInt( rec.data, 1, tmpInt, NULL ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] no Global Sequence Count in Terminate Section\n";
        return false;
    }

    if( tmpInt != nGlobSecLines )
    {
        ERRMSG << "\n + [INCONSISTENT FILE] file has " << nGlobSecLines << "lines ";
        cerr << "in the Global Section; Terminate Section reports " << tmpInt << "\n";
    }

    if( !DEItemToInt( rec.data, 2, tmpInt, NULL ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] no Directory Sequence Count in Terminate Section\n";
        return false;
    }

    if( tmpInt != nDESecLines )
    {
        ERRMSG << "\n + [INCONSISTENT FILE] file has " << nDESecLines << "lines ";
        cerr << "in the Directory Section; Terminate Section reports " << tmpInt << "\n";
    }

    if( !DEItemToInt( rec.data, 3, tmpInt, NULL ) )
    {
        ERRMSG << "\n + [CORRUPT FILE] no Parameter Sequence Count in Terminate Section\n";
        return false;
    }

    if( tmpInt != nPDSecLines )
    {
        ERRMSG << "\n + [INCONSISTENT FILE] file has " << nPDSecLines << "lines ";
        cerr << "in the Parameter Section; Terminate Section reports " << tmpInt << "\n";
    }

    return true;
}
