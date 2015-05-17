/*
 * file: entity504.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 504: Edge, Section 4.148, p.588+ (616+)
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

using namespace std;


IGES_ENTITY_504::IGES_ENTITY_504( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 504;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY;         // required by specification
    use = STAT_USE_GEOMETRY;
    hierarchy = STAT_HIER_NO_SUB;   // required by specification

    return;
}


IGES_ENTITY_504::~IGES_ENTITY_504()
{
    deItems.clear();
    vedges.clear();

    std::list<EDGE_DATA>::iterator sE = edges.begin();
    std::list<EDGE_DATA>::iterator eE = edges.end();

    while( sE != eE )
    {
        if( sE->curv )
            sE->curv->DelReference( this );

        ++sE;
    }

    edges.clear();

    list< pair<IGES_ENTITY_502*, int> >::iterator sV = vertices.begin();
    list< pair<IGES_ENTITY_502*, int> >::iterator eV = vertices.end();

    while( sV != eV )
    {
        sV->first->DelReference( this );
        ++sV;
    }

    vertices.clear();
    return;
}


bool IGES_ENTITY_504::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    // XXX - TO BE IMPLEMENTED: add the curves as references;
    // take care to count references for vertex list entities
    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_504::format( int &index )
{
#warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_504::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_504::Unlink( IGES_ENTITY* aChildEntity )
{
#warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    // XXX - if a curve is unlinked, take care to check its
    // SVP and TVP references and unlink as necessary

    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_504::IsOrphaned( void )
{
    if( refs.empty() || edges.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_504::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    bool ok = IGES_ENTITY::AddReference( aParentEntity, isDuplicate );

    if( ok )
        return true;

#warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED

    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_504::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_504::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
#warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED

    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_504::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
#warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED

    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_504::SetEntityForm( int aForm )
{
    if( aForm == 1 )
        return true;

    ERRMSG << "\n + [BUG] Edge Entity only supports Form 1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_504::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Edge Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_504::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Edge Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_504::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    if( STAT_HIER_NO_SUB == aHierarchy )
        return true;

    ERRMSG << "\n + [BUG] Edge Entity only supports hierarchy = STAT_HIER_NO_SUB\n";
    return true;
}


std::vector<EDGE_DATA>* IGES_ENTITY_504::GetEdges( void )
{
    if( edges.empty() )
    {
        vedges.clear();
        return &vedges;
    }

    if( vedges.empty() || vedges.size() != edges.size() )
    {
        vedges.clear();
        std::list<EDGE_DATA>::iterator sV = edges.begin();
        std::list<EDGE_DATA>::iterator eV = edges.end();

        while( sV != eV )
        {
            vedges.push_back( *sV );
            ++sV;
        }
    }

    return &vedges;
}

void IGES_ENTITY_504::AddEdge( IGES_ENTITY* aCurve,
                               IGES_ENTITY_502* aSVP, int aSV,
                               IGES_ENTITY_502* aTVP, int aTV )
{
#warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [BUG] NOT IMPLEMENTED\n";
}
