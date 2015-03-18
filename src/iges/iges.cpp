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

#include <libigesconf.h>
#include <locale.h>
#include <cstdlib>
#include <cerrno>
#include <sstream>
#include <limits>
#include <iomanip>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <all_entities.h>
#include <boost/filesystem.hpp>

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


static double UNIT_TO_MM[UNIT_END] =
{
    25.4,       // mm/inch
    1.0,        // mm/mm
    1.0,        // UNIT_EXTERN - this is only here as a filler
    304.8,      // mm/foot
    1609344.0,  // mm/mile
    1000.0,     // mm/m
    1000000.0,  // mm/km
    0.0254,     // mm/mil
    0.001,      // mm/micron
    10.0,       // mm/cm
    2.54e-5     // mm/microinch
};


static int mdays[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


static bool checkDate( const std::string aDate )
{
    if( aDate.length() != 13 && aDate.length() != 15 )
        return false;

    std::string dyear;
    std::string dmon;
    std::string dmday;
    std::string dhour;
    std::string dmin;
    std::string dsec;

    int idx = 0;
    bool ok = true;

    if( aDate.length() == 13 )
    {
        dyear = "19" + aDate.substr( 0, 2 );
        idx += 2;
    }
    else
    {
        dyear = aDate.substr( 0, 4 );
        idx += 4;
    }

    dmon = aDate.substr( idx, 2 );
    idx += 2;
    dmday = aDate.substr( idx, 2 );
    idx += 2;

    if( aDate[idx] != '.' )
        ok = false;

    ++idx;
    dhour = aDate.substr( idx, 2 );
    idx += 2;
    dmin = aDate.substr( idx, 2 );
    idx += 2;
    dsec = aDate.substr( idx, 2 );

    int iyear;
    int imon;
    int iday;
    int ihour;
    int imin;
    int isec;

    const char* cp0;
    char* cp1;

    errno = 0;
    cp0 = dyear.c_str();
    iyear = strtol( cp0, &cp1, 10 );

    if( errno || cp1 == cp0 || (cp1 - cp0) != 4 )
        ok = false;

    // note: parts created before the creation of the IGES specification
    // obviously have a bad date; however there is defective software out
    // there such as the SolidWorks IGES exporter which do not comply with
    // the IGES specification and report 2-digit years regardless of the
    // century. Due to such defective software, any 2-digit year will be
    // accepted as valid by this implementation; however a message will be
    // printed to the error stream if a date predates IGES v1.
    if( iyear < 1978 )
    {
        ERRMSG << "\n + [WARNING] the file has a suspicious year in the date tag (";
        cerr << dyear << ")\n";
    }

    errno = 0;
    cp0 = dmon.c_str();
    imon = strtol( cp0, &cp1, 10 );

    if( errno || cp1 == cp0 || (cp1 - cp0) != 2 )
        ok = false;

    if( imon < 1 || imon > 12 )
        ok = false;

    errno = 0;
    cp0 = dmday.c_str();
    iday = strtol( cp0, &cp1, 10 );

    if( errno || cp1 == cp0 || (cp1 - cp0) != 2 )
        ok = false;

    if( iday < 1 || iday > mdays[imon -1] )
        ok = false;

    errno = 0;
    cp0 = dhour.c_str();
    ihour = strtol( cp0, &cp1, 10 );

    if( errno || cp1 == cp0 || (cp1 - cp0) != 2 )
        ok = false;

    if( ihour < 0 || ihour > 23 )
        ok = false;

    errno = 0;
    cp0 = dmin.c_str();
    imin = strtol( cp0, &cp1, 10 );

    if( errno || cp1 == cp0 || (cp1 - cp0) != 2 )
        ok = false;

    if( imin < 0 || imin > 59 )
        ok = false;

    errno = 0;
    cp0 = dsec.c_str();
    isec = strtol( cp0, &cp1, 10 );

    if( errno || cp1 == cp0 || (cp1 - cp0) != 2 )
        ok = false;

    // note: using '60' ensures we allow leap seconds
    if( isec < 0 || imin > 60 )
        ok = false;

    return ok;
}


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
    Clear();
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

    globalData.cf = 1.0;
    globalData.convert = false;

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
    return true;
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

    file.open( aFileName, ios::in | ios::binary );

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

    // Compare the filename with the name stored
    // in the IGES file. If the names are not the same then
    // print out a warning message and set the internal filename
    // to match the name on disk. Keep in mind that the
    // name discrepancies may result in the failure of some
    // IGES implementations to correctly load externally
    // referenced files.
    std::string fName;
    do
    {
        boost::filesystem::path bp( aFileName );
        fName = bp.filename().string();
    } while(0);

    if( fName.compare( globalData.fileName ) )
    {
        ERRMSG << "\n + [INFO] filename mismatch:\n";
        cerr << " + internal filename: '" << globalData.fileName << "'\n";
        cerr << " + filename on disk: '" << fName << "'\n";
        globalData.fileName = fName;
    }

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

    cull();
    return true;
}


// open a file with the given name and write out all data
bool IGES::Write( const char* aFileName, bool fOverwrite )
{
    IGES_LOCALE igloc;

    if( !aFileName )
    {
        ERRMSG << "\n + [BUG] null pointer passed for filename\n";
        return false;
    }

    cull();

    if( entities.empty() )
    {
        ERRMSG << "\n + [INFO ] no entities to save\n";
        return false;
    }

    // Assign Sequence numbers
    size_t nEnt = entities.size();
    size_t iEnt;
    int index = 1;

    for( iEnt = 0; iEnt < nEnt; ++iEnt )
        entities[iEnt]->sequenceNumber = (int)(iEnt << 1) + 1;

    nDESecLines = nEnt << 1;

    // Format PD entries for output and update some DE items
    for( iEnt = 0; iEnt < nEnt; ++iEnt )
    {
        if( !entities[iEnt]->format( index ) )
        {
            ERRMSG << "\n + [INFO] could not format entity for output\n";

            for( nEnt = 0; nEnt < iEnt; ++nEnt )
                entities[iEnt]->unformat();

            return false;
        }
    }

    nPDSecLines = index - 1;

    ofstream file;

    file.open( aFileName, ios::out | ios_base::in | ios::binary );

    if( file.is_open() )
    {
        if( !fOverwrite )
        {
            ERRMSG << "\n + [INFO] file already exists; not overwriting\n";
            cerr << " + filename: '" << aFileName << "'\n";
            file.close();
            return false;
        }

        // reopen the file and truncate it
        file.close();
        file.open( aFileName, ios::out | ios_base::in | ios::binary | ios::trunc );
    }
    else
    {
        file.open( aFileName, ios::out | ios::binary );
    }

    if( !file.is_open() )
    {
        for( iEnt = 0; iEnt < nEnt; ++iEnt )
            entities[iEnt]->unformat();

        ERRMSG << "\n + [INFO] could not open file\n";
        cerr << " + filename: '" << aFileName << "'\n";
        return false;
    }

    do
    {
        boost::filesystem::path bp( aFileName );
        globalData.fileName = bp.filename().string();
    } while(0);

    // START SECTION
    if( !writeStart( file ) )
    {
        ERRMSG << "\n + [INFO] could not write START section\n";
        file.close();
        return false;
    }

    // GLOBAL SECTION
    if( !writeGlobals( file ) )
    {
        ERRMSG << "\n + [INFO] could not write GLOBAL section\n";
        file.close();
        return false;
    }

    // DIRECTORY ENTRY SECTION
    for( iEnt = 0; iEnt < nEnt; ++iEnt )
    {
        if( !entities[iEnt]->WriteDE( file ) )
        {
            ERRMSG << "\n + [INFO] could not write out Directory Entries\n";
            file.close();
            return false;
        }
    }

    // PARAMETER DATA SECTION
    for( iEnt = 0; iEnt < nEnt; ++iEnt )
    {
        if( !entities[iEnt]->WritePD( file ) )
        {
            ERRMSG << "\n + [INFO] could not write out Parameter Data\n";
            file.close();
            return false;
        }
    }

    // TERMINATE SECTION
    std::string oline;
    std::string tmp;

    if( !FormatDEInt( tmp, startSection.size() ) )
    {
        ERRMSG << "\n + [INFO] could not format S* entry in terminal line\n";
        file.close();
        return false;
    }

    tmp[0] = 'S';
    oline = tmp;

    if( !FormatDEInt( tmp, nGlobSecLines ) )
    {
        ERRMSG << "\n + [INFO] could not format G* entry in terminal line\n";
        file.close();
        return false;
    }

    tmp[0] = 'G';
    oline += tmp;

    if( !FormatDEInt( tmp, nDESecLines) )
    {
        ERRMSG << "\n + [INFO] could not format D* entry in terminal line\n";
        file.close();
        return false;
    }

    tmp[0] = 'D';
    oline += tmp;

    if( !FormatDEInt( tmp, nPDSecLines ) )
    {
        ERRMSG << "\n + [INFO] could not format P* entry in terminal line\n";
        file.close();
        return false;
    }

    tmp[0] = 'P';
    oline += tmp;
    oline.append( 40, ' ' );

    if( !FormatDEInt( tmp, 1 ) )
    {
        ERRMSG << "\n + [INFO] could not format T* entry in terminal line\n";
        file.close();
        return false;
    }

    tmp[0] = 'T';
    oline += tmp;
    oline += "\n";
    file << oline;

    if( file.fail() )
    {
        file.close();
        return false;
    }

    file.close();
    return true;
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

        case ENT_COMPOSITE_CURVE:
            ep = new IGES_ENTITY_102( this );
            break;

        case ENT_LINE:
            ep = new IGES_ENTITY_110( this );
            break;

        case ENT_SURFACE_OF_REVOLUTION:
            ep = new IGES_ENTITY_120( this );
            break;

        case ENT_TABULATED_CYLINDER:
            ep = new IGES_ENTITY_122( this );
            break;

        case ENT_TRANSFORMATION_MATRIX:
            ep = new IGES_ENTITY_124( this );
            break;

        case ENT_NURBS_CURVE:
            ep = new IGES_ENTITY_126( this );
            break;

        case ENT_NURBS_SURFACE:
            ep = new IGES_ENTITY_128( this );
            break;

        case ENT_CURVE_ON_PARAMETRIC_SURFACE:
            ep = new IGES_ENTITY_142( this );
            break;

        case ENT_TRIMMED_PARAMETRIC_SURFACE:
            ep = new IGES_ENTITY_144( this );
            break;

        case ENT_RIGHT_CIRCULAR_CYLINDER:
            ep = new IGES_ENTITY_154( this );
            break;

        case ENT_SOLID_OF_LINEAR_EXTRUSION:
            ep = new IGES_ENTITY_164( this );
            break;

        case ENT_BOOLEAN_TREE:
            ep = new IGES_ENTITY_180( this );
            break;

        case ENT_SUBFIGURE_DEFINITION:
            ep = new IGES_ENTITY_308( this );
            break;

        case ENT_COLOR_DEFINITION:
            ep = new IGES_ENTITY_314( this );
            break;

        case ENT_SINGULAR_SUBFIGURE_INSTANCE:
            ep = new IGES_ENTITY_408( this );
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

    if( !checkDate( globalData.creationDate ) )
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

        if( !checkDate( globalData.modificationDate ) )
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

    // apply a scale if the model scale is not 1.0
    if( globalData.modelScale < 0.9999998 || globalData.modelScale > 1.000001 )
    {
        globalData.minResolution /= globalData.modelScale;
        globalData.cf = 1.0 / globalData.modelScale;
        globalData.modelScale = 1.0;
        globalData.convert = true;
    }

    if( globalData.unitsFlag != UNIT_MILLIMETER )
    {
        globalData.minResolution *= UNIT_TO_MM[globalData.unitsFlag];
        globalData.cf *= UNIT_TO_MM[globalData.unitsFlag];
        globalData.unitsFlag = UNIT_MILLIMETER;
        globalData.convert = true;
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


// cull unsupported and orphaned entities
void IGES::cull( void )
{
    size_t nEnt = entities.size();
    size_t iEnt;
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

    return;
}


bool IGES::ConvertUnits( IGES_UNIT newUnit )
{
    if( globalData.unitsFlag == newUnit )
        return true;

    if( globalData.unitsFlag == UNIT_EXTERN )
    {
        ERRMSG << "\n + [INFO] cannot convert units; internal units is UNIT_EXTERN\n";
        return false;
    }

    if( newUnit == UNIT_EXTERN )
    {
        ERRMSG << "\n + [INFO] cannot convert units; user-specified units is UNIT_EXTERN\n";
        return false;
    }

    double cf;

    // + Calculate a scale factor to convert units.
    // + adjust the User Intended Minimum to represent the mm equivalent (if possible)
    cf = UNIT_TO_MM[globalData.unitsFlag] / UNIT_TO_MM[newUnit];

    if( cf > 0.9999998 && cf < 1.000001 )
        return true;

    globalData.minResolution *= cf;

    // scale all existing entities
    size_t nEnt = entities.size();

    for( size_t i = 0; i < nEnt; ++ i )
    {
        if( !entities[i]->rescale(cf) )
        {
            ERRMSG << "\n + [BUG] cannot convert units\n";
            return false;
        }
    }

    globalData.unitsFlag = newUnit;

    return true;
}


bool IGES::ChangeModelScale( double aScale )
{
    if( aScale < 6.0e-8 )
    {
        ERRMSG << "\n + [INFO] rejecting scale (< 6.0e-8)\n";
        return false;
    }

    if( aScale > 17000000.0 )
    {
        ERRMSG << "\n + [INFO] rejecting scale (> 17000000.0)\n";
        return false;
    }

    double cf = aScale / globalData.modelScale;
    globalData.minResolution *= aScale;
    globalData.modelScale = aScale;

    // scale all existing entities
    size_t nEnt = entities.size();

    for( size_t i = 0; i < nEnt; ++ i )
    {
        if( !entities[i]->rescale(cf) )
        {
            ERRMSG << "\n + [BUG] cannot convert units\n";
            return false;
        }
    }

    return true;
}


std::list<std::string>* IGES::GetHeaders(void)
{
    return &startSection;
}


size_t IGES::GetNHeaderLines(void)
{
    return startSection.size();
}


bool IGES::AddToHeader( const std::string& comments )
{
    if( comments.empty() )
        return true;

    std::string tStr = comments;
    std::string tStr1;

    size_t slen = tStr.length();
    size_t slen1;

    if( slen < 72 )
    {
        tStr.append( 72 - slen, ' ' );
        startSection.push_back( tStr );
    }
    else if( slen > 72 )
    {
        for( size_t i = 0; i < slen; i += 72 )
        {
            tStr1 = tStr.substr(i, 72);
            slen1 = tStr1.length();

            if( slen1 < 72 )
                tStr1.append( 72 - slen1, ' ' );

            startSection.push_back( tStr1 );
        }
    }

    return true;
}

// write out the START SECTION
bool IGES::writeStart( std::ofstream& file )
{
    if( startSection.empty() )
        startSection.push_back( "# NOTE: no user-provided comment. This comment is provided to meet spec." );

    std::list<std::string>::iterator ssc = startSection.begin();
    std::list<std::string>::iterator esc = startSection.end();
    std::string tStr;
    std::string tStr1;

    size_t slen;
    size_t slen1;

    while( ssc != esc )
    {
        slen = (*ssc).length();

        if( slen < 72 )
        {
            (*ssc).append( 72 - slen, ' ' );
        }
        else if( slen > 72 )
        {
            tStr = *ssc;

            for( size_t i = 0; i < slen; i += 72 )
            {
                tStr1 = tStr.substr(i, 72);
                slen1 = tStr1.length();

                if( slen1 < 72 )
                    tStr1.append( 72 - slen1, ' ' );

                startSection.insert( ssc, tStr1  );
            }

            ssc = startSection.erase( ssc );
        }

        ++ssc;
    }

    ssc = startSection.begin();
    int nsc = 1;

    while( ssc != esc )
    {
        if( !FormatDEInt( tStr1, nsc++ ) )
        {
            ERRMSG << "\n + [INFO] could not format START section\n";
            return false;
        }

        tStr1[0] = 'S';
        tStr = *ssc + tStr1 + "\n";
        file << tStr;

        if( file.fail() )
        {
            ERRMSG << "\n + [INFO] could not write START section\n";
            return false;
        }

        ++ssc;
    }

    return true;
}


// write out the GLOBAL SECTION
bool IGES::writeGlobals( std::ofstream& file )
{
    std::string gstr;   // Global Section data as a single string
    std::string lstr;   // one line of Global Section Data being assembled
    std::string tstr;   // single item being formatted for output

    nGlobSecLines = 0;

    char pd = globalData.pdelim;
    char rd = globalData.rdelim;

    if( globalData.minResolution < 1e-12 )
        globalData.minResolution = 0.001;

    // Item 1: (HStr) Parameter Delimeter
    // REQ DEF ","
    if( pd == ',' )
        lstr = ",";
    else
        lstr = lstr + "1H" + pd + pd;

    // Item 2: (HStr) Record Delimeter
    // REQ DEF ";"
    if( rd == ';' )
        lstr = lstr + pd;
    else
        lstr = lstr + "1H" + rd + pd;

    // Item 3: (HStr) Product ID from Sending System
    // REQ NODEF (set to "none" if no ID provided by user)
    tstr = globalData.productIDSS;

    if( tstr.empty() )
        tstr = "none";

    int idx = 1;

    if( !AddSecHStr( tstr, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Product ID, Sending System\n";
        return false;
    }

    // Item 4: (HStr) Filename
    // REQ NODEF - value must be derived from the actual filename
    tstr = globalData.fileName;

    if( tstr.empty() )
    {
        ERRMSG << "\n + [BUG] file name not set by IGES::Write()\n";
        return false;
    }

    if( !AddSecHStr( tstr, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add File Name\n";
        return false;
    }

    // Item 5: (HStr) Native System ID
    // REQ NODEF - (set to "none" if no ID provided by application)
    tstr = globalData.nativeSystemID;

    if( tstr.empty() )
        tstr = "none";

    if( !AddSecHStr( tstr, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add File Name\n";
        return false;
    }

    // Item 6: (HStr) Preprocessor Version
    // REQ NODEF - Version String of libIGES
    ostringstream ostr;
    ostr << "libIGES Version " << LIB_VERSION_MAJOR << ".";
    ostr << LIB_VERSION_MINOR;

    if( !AddSecHStr( ostr.str(), lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Preprocessor Version\n";
        return false;
    }

    // Item 7: (int) number of binary bits for Integer representation
    // REQ NODEF = numeric_limits<unsigned int>::max() >> N
    unsigned int maxN = numeric_limits<unsigned int>::max();
    int nbit = 0;

    while( maxN )
    {
        maxN >>= 1;
        ++nbit;
    }

    ostr.str("");
    ostr << nbit << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add N bits for integer representation\n";
        return false;
    }

    // Item 8: (int) max power of 10 for single float
    // REQ NODEF = numeric_limits<float>::max_exponent10
    nbit = numeric_limits<float>::max_exponent10;

    ostr.str("");
    ostr << nbit << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Max Exponent (float)\n";
        return false;
    }

    // Item 9: (int) max number of significant digits for single float
    // REQ NODEF = numeric_limits<float>::(max_)digits10
    nbit = numeric_limits<float>::digits10;

    ostr.str("");
    ostr << nbit << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Max Digits (float)\n";
        return false;
    }

    // Item 10: (int) max power of 10 for double float
    // REQ NODEF = numeric_limits<double>::max_exponent10
    nbit = numeric_limits<double>::max_exponent10;

    ostr.str("");
    ostr << nbit << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Max Exponent (double)\n";
        return false;
    }

    // Item 11: (int) max number of significant digits for double float
    // REQ NODEF = numeric_limits<double>::(max_)digits10
    nbit = numeric_limits<double>::digits10;

    ostr.str("");
    ostr << nbit << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Max Digits (double)\n";
        return false;
    }

    // Item 12: (HStr) Product ID for Receiving System
    // REQ DEF = same as Item 3
    tstr = globalData.productIDRS;

    if( !AddSecHStr( tstr, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Product ID, Receiving System\n";
        return false;
    }

    // Item 13: (Real) Model Space Scale
    // REQ DEF = 1.0
    if( !FormatPDREal( tstr, globalData.modelScale, pd, globalData.minResolution ) )
    {
        ERRMSG << "\n + [INFO] failed to format Model Scale\n";
        return false;
    }

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Model Scale\n";
        return false;
    }

    // Item 14: (int) Units Flag
    // REQ DEF = 0 (INCH)
    ostr.str("");
    ostr << globalData.unitsFlag << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Units Flag\n";
        return false;
    }

    // Item 15: (HStr) Units Name
    // REQ DEF : UNIT_NAMES[Unit Flag]
    if( !AddSecHStr( UNIT_NAMES[globalData.unitsFlag - UNIT_START], lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Units Name\n";
        return false;
    }

    // Item 16: (int) Max. Number of Linewidth Gradations
    // REQ DEF = 1, must be > 0
    if( globalData.maxLinewidthGrad < 1 )
        globalData.maxLinewidthGrad = 1;

    ostr.str("");
    ostr << globalData.maxLinewidthGrad << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Max. Linewidth Gradations\n";
        return false;
    }

    // Item 17: (Real) Max. Width of Lines
    // REQ NODEF ( use 1.0 if none specified )
    if( globalData.maxLinewidth < 1e-6 )
        globalData.maxLinewidthGrad = 1.0;

    if( !FormatPDREal( tstr, globalData.maxLinewidthGrad, pd, globalData.minResolution ) )
    {
        ERRMSG << "\n + [INFO] failed to format Max. Linewidth\n";
        return false;
    }

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Max. Linewidth\n";
        return false;
    }

    // Item 18: (HStr) Creation Date (15HYYYYMMDD.hhmmss OR 13HYYYYMMDD.hhmmss)
    // only generate a date if none currently exist
    if( !checkDate(globalData.creationDate) )
    {
        time_t tt = time( NULL );
        struct tm tmt;
        gmtime_r( &tt, &tmt );
        ostr.str("");
        ostr << setw(4) << setfill('0') << (tmt.tm_year + 1900);
        ostr << setw(2) << (tmt.tm_mon + 1) << setw(2) << tmt.tm_mday << ".";
        ostr << setw(2) << tmt.tm_hour << setw(2) << tmt.tm_min;
        ostr << setw(2) << tmt.tm_sec << setw(0) << setfill(' ');
        tstr = ostr.str();
    }
    else
    {
        tstr = globalData.creationDate;
    }

    if( !AddSecHStr( tstr, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Creation Date\n";
        return false;
    }

    // Item 19: (Real) Min. user intended resolution/granularity
    // REQ NODEF ( use 0.001mm if none specified )
    if( !FormatPDREal( tstr, globalData.minResolution, pd, globalData.minResolution ) )
    {
        ERRMSG << "\n + [INFO] failed to format Min. Intended Resolution\n";
        return false;
    }

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Min. Intended Resolution\n";
        return false;
    }

    // Item 20: (Real) Approx. max. coordinate value or 0.
    // REQ DEF = 0.0; TODO: set to default until we can calculate a value
    // XXX - TO BE IMPLEMENTED
    tstr = "0.0";
    tstr += pd;

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Approx. Max. Coordinate\n";
        return false;
    }

    // Item 21: (HStr) Name of Author
    // REQ DEF = NULL
    if( !AddSecHStr( globalData.author, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Author\n";
        return false;
    }

    // Item 22: (HStr) Author's Organization
    // REQ DEF = NULL
    if( !AddSecHStr( globalData.organization, lstr, gstr, idx, pd, rd, pd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Organization\n";
        return false;
    }

    // Item 23: (int) Specification Version Flag
    // REQ DEF = 3 : value = 11 since this library supports IGES5.3 (6.0)
    tstr = "11";
    tstr += pd;

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Specification Version Flag\n";
        return false;
    }

    // Item 24: (int) Drafting Standard Flag
    // REQ DEF = 0 : NONE
    ostr.str("");
    ostr << globalData.draftStandard << pd;
    tstr = ostr.str();

    if( !AddSecItem( tstr, lstr, gstr, idx, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] failed to add Drafting Standard Flag\n";
        return false;
    }

    // Item 25: (HStr) Modification Date (must update it here)
    // Format: 15HYYYYMMDD.hhmmss
    do
    {
        time_t tt = time( NULL );
        struct tm tmt;
        gmtime_r( &tt, &tmt );
        ostr.str("");
        ostr << setw(4) << setfill('0') << (tmt.tm_year + 1900);
        ostr << setw(2) << (tmt.tm_mon + 1) << setw(2) << tmt.tm_mday << ".";
        ostr << setw(2) << tmt.tm_hour << setw(2) << tmt.tm_min;
        ostr << setw(2) << tmt.tm_sec << setw(0) << setfill(' ');
    } while(0);

    char delim = pd;

    if( globalData.applicationNote.empty() )
        delim = rd;

    if( !AddSecHStr( ostr.str(), lstr, gstr, idx, pd, rd, delim ) )
    {
        ERRMSG << "\n + [INFO] failed to add Modification Date\n";
        return false;
    }

    // Item 26: (HStr) Application Protocol
    // REQ DEF = NULL
    if( delim == pd )
    {
        if( !AddSecHStr( globalData.applicationNote, lstr, gstr, idx, pd, rd, rd ) )
        {
            ERRMSG << "\n + [INFO] failed to add Modification Date\n";
            return false;
        }
    }

    file << gstr;
    nGlobSecLines = idx - 1;

    if( file.fail() )
    {
        ERRMSG << "\n +[INFO] could not write Global Section to file\n";
        return false;
    }

    return true;
}

// export all entities to the given IGES* (to be used for creating Assemblies)
bool IGES::Export( IGES* newParent, IGES_ENTITY_308** packagedEntity )
{
    *packagedEntity = NULL;

    if( NULL == newParent )
    {
        ERRMSG << "\n + [BUG] Export() invoked without a valid IGES pointer\n";
        return false;
    }

    if( NULL == packagedEntity )
    {
        ERRMSG << "\n + [BUG] Export() invoked without a valid Entity 308 handle\n";
        return false;
    }

    if( entities.empty() )
        return true;

    // sextract information from parent IGES
    // + int maxLinewidthGrad
    // + double modelScale
    // + IGES_UNIT   unitsFlag
    int maxLWG = newParent->globalData.maxLinewidthGrad;
    double pms = newParent->globalData.modelScale;
    IGES_UNIT pUF = newParent->globalData.unitsFlag;

    // calculate a scale factor which yields the desired
    // final modelScale with the given Units. If this factor is
    // not 1.0 then trawl the list of entities and convert
    double cf = 1.0;
    bool adjScale = false;

    if( globalData.modelScale != pms )
    {
        cf = pms / globalData.modelScale;
        adjScale = true;
    }

    if( globalData.unitsFlag != pUF )
    {
        cf *= UNIT_TO_MM[globalData.unitsFlag] / UNIT_TO_MM[pUF];
        adjScale = true;
    }

    size_t nEnt = entities.size();

    if( adjScale )
    {
        // scale all existing entities
        for( size_t i = 0; i < nEnt; ++ i )
        {
            if( !entities[i]->rescale(cf) )
            {
                ERRMSG << "\n + [BUG] cannot convert units\n";
                return false;
            }
        }
    }

    // determine crude linewidth adjustment; the new linewidths are guaranteed to
    // be incorrect unless (a) they are 0 or (b) maxLWG and maxLW are the same
    // for the new parent IGES and the IGES being merged.
    double lws = maxLWG / globalData.maxLinewidthGrad;
    int llw;

    for( size_t i = 0; i < nEnt; ++ i )
    {
        llw = entities[i]->lineWeightNum;

        if( llw > 0 )
        {
            llw = (int)(double(llw) * lws);

            if( llw == 0 )
                llw = 1;
        }

        entities[i]->lineWeightNum = llw;
    }

    // iterate through the list of entities and store lists of
    // + (a) top level Entity 144 (Trimmed Parametric Surfaces)
    // + (b) top level Entity 408 (Singular Subfigure Instance)
    // If (b) is present then items in the list are to be stuffed
    // into an Entity 308 (Subfigure Definition), otherwise if (a)
    // exists then all entities within must be placed in an
    // Entity 308. If neither (a) nor (b) exist then the export
    // operation must return TRUE but the Entity308 handle must be NULL.
    //
    // Set *packagedEntity to equal our new subassembly (Entity 308)
    //

    list<IGES_ENTITY*> tplist;
    list<IGES_ENTITY*> sslist;
    int tEnt;
    size_t nRefs;

    for( size_t i = 0; i < nEnt; ++ i )
    {
        tEnt = entities[i]->GetEntityType();
        nRefs = entities[i]->GetNRefs();

        if( tEnt == ENT_PARAM_SPLINE_SURFACE && nRefs == 0 )
            tplist.push_back( entities[i] );
        else if( tEnt == ENT_SINGULAR_SUBFIGURE_INSTANCE && nRefs == 0 )
            sslist.push_back( entities[i] );
    }

    if( tplist.empty() && sslist.empty() )
        return true;

    IGES_ENTITY* ep;
    IGES_ENTITY_308* p308;

    if( !newParent->NewEntity( ENT_SUBFIGURE_DEFINITION, &ep ) )
    {
        ERRMSG << "\n + [BUG] could not create Subfigure Definition Entity\n";
        return false;
    }

    p308 = dynamic_cast<IGES_ENTITY_308*>(ep);

    if( NULL == p308 )
    {
        ERRMSG << "\n + [BUG] could not cast pointer to Subfigure Definition Entity pointer\n";
        return false;
    }

    list<IGES_ENTITY*>::iterator sEnt;
    list<IGES_ENTITY*>::iterator eEnt;

    if( !sslist.empty() )
    {
        sEnt = sslist.begin();
        eEnt = sslist.end();

        while( sEnt != eEnt )
        {
            if( !p308->AddDE( *sEnt ) )
            {
                ERRMSG << "\n + [INFO] could not transfer entity to Subfigure Definition\n";
                sEnt = sslist.begin();

                while( p308->DelDE( *sEnt ) && sEnt != eEnt );

                newParent->DelEntity( ep );
                return false;
            }

            ++sEnt;
        }
    }
    else
    {
        sEnt = tplist.begin();
        eEnt = tplist.end();

        while( sEnt != eEnt )
        {
            if( !p308->AddDE( *sEnt ) )
            {
                ERRMSG << "\n + [INFO] could not transfer entity to Subfigure Definition\n";
                sEnt = tplist.begin();

                while( p308->DelDE( *sEnt ) && sEnt != eEnt );

                newParent->DelEntity( ep );
                return false;
            }

            ++sEnt;
        }
    }

    for( size_t i = 0; i < nEnt; ++ i )
    {
        if( !newParent->AddEntity( entities[i] ) )
        {
            ERRMSG << "\n + [INFO] could not transfer entity to parent; both parent and child are now corrupted\n";

            for( size_t j = nEnt -1; j >= i; --j )
            {
                delete entities[j];
                entities.pop_back();
            }

            return false;
        }
    }

    *packagedEntity = p308;
    entities.clear();

    return true;
}
