/*
 * file: entity_template.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 0: NULL, Section 4.2, p.65 (93)
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

#error This is a template only and not suitable for compilation or inclusion

#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity_template.h>

using namespace std;


bool IGES_ENTITY_TEMP::Associate( std::vector<IGES_ENTITY*>* entities )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::format( int &index )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::rescale( double sf )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::Unlink( IGES_ENTITY* aChildEntity )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_TEMP::IGES_ENTITY_TEMP::AddReference( IGES_ENTITY* aParentEntity )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::DelReference( IGES_ENTITY* aParentEntity )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::SetEntityForm( int aForm )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::SetEntityUse( IGES_STAT_USE aUseCase )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_TEMP::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}
