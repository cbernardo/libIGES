/*
 * file: dll_entity308.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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

/*
 * Description: IGES Entity 308: Subfigure Definition Entity, Section 4.74, p.377(405+)
 */

#ifndef DLL_ENTITY_308_H
#define DLL_ENTITY_308_H

#include <libigesconf.h>
#include <dll_iges_entity.h>


class MCAD_API DLL_IGES_ENTITY_308 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_308( IGES* aParent, bool create );
    DLL_IGES_ENTITY_308( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_308();

    bool GetNumDE( size_t& aDESize );
    bool GetDEList( size_t& aDESize, IGES_ENTITY**& aDEList );
    bool AddDE(IGES_ENTITY* aPtr);
    bool AddDE(DLL_IGES_ENTITY*& aPtr);
    bool DelDE( IGES_ENTITY* aPtr );
    bool DelDE(DLL_IGES_ENTITY*& aPtr);
    bool GetNestDepth( int& aNestDepth );
    bool GetName( const char*& aName );
    bool SetName( const char*& aName );
};

#endif  // DLL_ENTITY_308_H
