/*
 * file: entity100.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 100: Circle, Section 4.3, p.66+ (94+)
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
#include <entity100.h>

using namespace std;

IGES_ENTITY_100::IGES_ENTITY_100( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 100;
    form = 0;

    zOffset = 0.0;
    xCenter = 0.0;
    yCenter = 0.0;
    xStart = 1.0;
    yStart = 0.0;
    xEnd = 1.0;
    yEnd = 0.0;

    return;
}   // IGES_ENTITY_100( IGES* aParent )


IGES_ENTITY_100::~IGES_ENTITY_100()
{
    return;
}   // ~IGES_ENTITY_100()


bool IGES_ENTITY_100::associate( std::vector<IGES_ENTITY*>* entities )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_100::format( int &index )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_100::Unlink( IGES_ENTITY* aChild )
{
    return IGES_ENTITY::Unlink( aChild );
}


bool IGES_ENTITY_100::IsOrphaned( void )
{
    // TRUE when:
    // a. there are no parent references AND
    // b. the object is not Independent (depends == 0)

    if( refs.empty() && depends != 0 )
        return true;

    return false;
}


bool IGES_ENTITY_100::IGES_ENTITY_100::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_100::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_100::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    //qwerty;
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;  // N.A.

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Circle\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_100::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::WriteDE( std::ofstream& aFile )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::WritePD( std::ofstream& aFile )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetEntityForm( int aForm )
{
    if( aForm == 0 )
        return true;

    ERRMSG << "\n + [BUG] Circle Entity only supports Form 0 (requested form: ";
    cerr << aForm << "\n";
    return false;
}


bool IGES_ENTITY_100::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetLevel( int aLevel )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetLevel( IGES_ENTITY* aLevel )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetView( IGES_ENTITY* aView )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetTransform( IGES_ENTITY* aTransform )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetColor( IGES_COLOR aColor )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetColor( IGES_ENTITY* aColor )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetLineWeightNum( int aLineWeight )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetEntityUse( IGES_STAT_USE aUseCase )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_100::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}
