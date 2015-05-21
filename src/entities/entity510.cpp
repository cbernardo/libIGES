/*
 * file: entity514.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 514: Shell, Section 4.151, p.595+ (623+)
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

using namespace std;


IGES_ENTITY_510::IGES_ENTITY_510( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 510;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY;         // required by specification
    msurface = NULL;
    mOuterLoopFlag = false;

    return;
}


IGES_ENTITY_510::~IGES_ENTITY_510()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


bool IGES_ENTITY_510::Associate( std::vector<IGES_ENTITY*>* entities )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::format( int &index )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_510::Unlink( IGES_ENTITY* aChildEntity )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::IsOrphaned( void )
{
    if( refs.empty() || NULL == msurface || mloops.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_510::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_510::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    depends = STAT_DEP_PHY;         // required
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    lineWeightNum = 0;              // N.A.
    colorNum = 0;                   // N.A.

    if( form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Face\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_510::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::SetEntityForm( int aForm )
{
    if( aForm == 1 )
        return true;

    ERRMSG << "\n + [BUG] Face Entity only supports Form 1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_510::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Face Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_510::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Face Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_510::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


const std::list<IGES_ENTITY_508*>* IGES_ENTITY_510::GetBounds( void )
{
    return &mloops;
}


bool IGES_ENTITY_510::AddBound( IGES_ENTITY_508* aLoop )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::SetSurface( IGES_ENTITY* aSurface )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


IGES_ENTITY* IGES_ENTITY_510::GetSurface( void )
{
    return msurface;
}


void IGES_ENTITY_510::SetOuterLoopFlag( bool aFlag )
{
    mOuterLoopFlag = aFlag;
    return;
}


bool IGES_ENTITY_510::GetOuterLoopFlag( void )
{
    return mOuterLoopFlag;
}
