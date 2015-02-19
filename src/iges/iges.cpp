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

#include <fstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <all_entities.h>

using namespace std;


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
    nStartSecLines = 0;
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
    if( !aFileName )
    {
        ERRMSG << "\n + [BUG] null pointer passed for filename\n";
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

    // XXX - TO BE IMPLEMENTED
    return false;
}


// open a file with the given name and write out all data
bool IGES::Write( const char* aFileName, bool fOverwrite )
{
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
