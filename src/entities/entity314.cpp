/*
 * file: entity314.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 314: Color, Section 4.77, p.386 (414)
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
#include <entity314.h>
#include <entity124.h>

using namespace std;

IGES_ENTITY_314::IGES_ENTITY_314( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 314;
    form = 0;
    red = 50.0;
    green = 0.0;
    blue = 0.0;
    colorNum = COLOR_RED;
}


IGES_ENTITY_314::~IGES_ENTITY_314()
{
    return;
}


bool IGES_ENTITY_314::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    structure = 0;
    lineFontPattern = 0;
    level = 0;
    view = 0;
    transform = 0;
    labelAssoc = 0;
    colorNum = 0;

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->DelReference( this );
        pStructure = NULL;
    }

    if( pLineFontPattern )
    {
        ERRMSG << "\n + [VIOLATION] Line Font Pattern entity is set\n";
        pLineFontPattern->DelReference( this );
        pLineFontPattern = NULL;
    }

    if( pLevel )
    {
        ERRMSG << "\n + [VIOLATION] Level entity is set\n";
        pLevel->DelReference( this );
        pLevel = NULL;
    }

    if( pView )
    {
        ERRMSG << "\n + [VIOLATION] View entity is set\n";
        pView->DelReference( this );
        pView = NULL;
    }

    if( pTransform )
    {
        ERRMSG << "\n + [VIOLATION] Transform entity is set\n";
        pTransform->DelReference( this );
        pTransform = NULL;
    }

    if( pLabelAssoc )
    {
        ERRMSG << "\n + [VIOLATION] Label Associativity is set\n";
        pLabelAssoc->DelReference( this );
        pLabelAssoc = NULL;
    }

    if( pColor )
    {
        ERRMSG << "\n + [VIOLATION] Color entity is set\n";
        pColor->DelReference( this );
        pColor = NULL;
    }

    return true;
}


bool IGES_ENTITY_314::format( int &index )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::rescale( double sf )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::Unlink( IGES_ENTITY* aChild )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::IsOrphaned( void )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::AddReference( IGES_ENTITY* aParentEntity )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::DelReference( IGES_ENTITY* aParentEntity )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetEntityForm( int aForm )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetEntityUse( IGES_STAT_USE aUseCase )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetLevel( int aLevel )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetLevel( IGES_ENTITY* aLevel )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetView( IGES_ENTITY* aView )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetTransform( IGES_ENTITY* aTransform )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetColor( IGES_COLOR aColor )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetColor( IGES_ENTITY* aColor )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}


bool IGES_ENTITY_314::SetLineWeightNum( int aLineWeight )
{
    // XXX - TO BE IMPLEMENTES
    return false;
}
