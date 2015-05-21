/*
 * file: entity508.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 508: Loop, Section 4.149, p.590+ (618+)
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
#include <entity508.h>

using namespace std;


IGES_ENTITY_508::IGES_ENTITY_508( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 508;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY;         // required by specification

    return;
}


IGES_ENTITY_508::~IGES_ENTITY_508()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


bool IGES_ENTITY_508::Associate( std::vector<IGES_ENTITY*>* entities )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_508::format( int &index )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_508::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_508::Unlink( IGES_ENTITY* aChildEntity )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_508::IsOrphaned( void )
{
    if( refs.empty() || edges.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_508::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_508::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_508::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    depends = STAT_DEP_PHY;         // required
    view = 0;                       // N.A.
    transform = 0;                  // N.A.

    if( 1 != form && 0 != form )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in Loop\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_508::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_508::SetEntityForm( int aForm )
{
    if( 1 == aForm || 0 == aForm )
        return true;

    ERRMSG << "\n + [BUG] Loop Entity only supports Form 0/1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_508::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Loop Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_508::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Loop Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_508::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    hierarchy = aHierarchy;
    return true;
}


bool IGES_ENTITY_508::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


// add a parent reference to a Vertex or Edge list entity and maintain a refcount
bool IGES_ENTITY_508::addEdge( IGES_ENTITY* aEdge )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// decrement refcount and release entity if appropriate
bool IGES_ENTITY_508::delEdge( IGES_ENTITY* aEdge )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// add a parent reference to a parameter space curve and ensure no duplicates
bool IGES_ENTITY_508::addPCurve( IGES_ENTITY* aCurve )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// delete parent reference from the given parameter space curve
bool IGES_ENTITY_508::delPCurve( IGES_ENTITY* aCurve )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


const std::list<LOOP_DATA>* IGES_ENTITY_508::GetLoopData( void )
{
    return &edges;
}

bool IGES_ENTITY_508::AddEdge( LOOP_DATA& aEdge )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}
