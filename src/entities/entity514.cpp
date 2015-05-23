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
#include <entity514.h>

using namespace std;


IGES_ENTITY_514::IGES_ENTITY_514( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 514;
    form = 1;
    visible = true;

    return;
}


IGES_ENTITY_514::~IGES_ENTITY_514()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


bool IGES_ENTITY_514::Associate( std::vector<IGES_ENTITY*>* entities )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_514::format( int &index )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_514::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_514::Unlink( IGES_ENTITY* aChildEntity )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_514::IsOrphaned( void )
{
    if( (refs.empty() && depends) || mfaces.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_514::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
#warning TO BE IMPLEMENTED
    return true;
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_514::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_514::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    lineWeightNum = 0;              // N.A.
    colorNum = 0;                   // N.A.

    if( form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Shell\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_514::ReadPD( std::ifstream& aFile, int& aSequenceVar )
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


bool IGES_ENTITY_514::SetEntityForm( int aForm )
{
    if( 1 == aForm || 2 == aForm )
    {
        form = aForm;
        return true;
    }

    ERRMSG << "\n + [BUG] Shell Entity only supports Forms 1 and 2 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_514::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Shell Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_514::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}

// XXX - MORE TO BE ADDED
#warning UNIMPLEMENTED
