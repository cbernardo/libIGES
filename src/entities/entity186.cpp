/*
 * file: entity186.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 186: Manifold Solid B-REP Object (MSBO)
 *              Section 4.49, p.216+ (244+)
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
#include <sstream>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity502.h>
#include <entity504.h>
#include <entity508.h>
#include <entity510.h>
#include <entity514.h>
#include <entity186.h>

using namespace std;


IGES_ENTITY_186::IGES_ENTITY_186( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 186;
    form = 0;

    visible = true;
    mDEshell = 0;
    mshell = NULL;
    mSOF = false;

    return;
}


IGES_ENTITY_186::~IGES_ENTITY_186()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


bool IGES_ENTITY_186::Associate( std::vector<IGES_ENTITY*>* entities )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_186::format( int &index )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_186::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_186::Unlink( IGES_ENTITY* aChildEntity )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_186::IsOrphaned( void )
{
    if( NULL == mshell )
        return true;

    return false;
}


bool IGES_ENTITY_186::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_186::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_186::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.

    if( 0 != form )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in MSBO\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_186::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Edge Entity\n";
        pdout.clear();
        return false;
    }
#warning TO BE IMPLEMENTED
    return true;

    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_186::SetEntityForm( int aForm )
{
    if( 0 == aForm )
        return true;

    ERRMSG << "\n + [BUG] MSBO Entity only supports Form 0 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}
