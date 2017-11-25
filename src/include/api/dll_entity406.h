/*
 * file: dll_entity406.h
 *
 * Copyright 2017, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * This file is part of libIGES.
 *
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 * Description: IGES Entity 406: Property Entity, Section 4.98, p.476 (504)
 */

#ifndef DLL_ENTITY_406_H
#define DLL_ENTITY_406_H

#include <libigesconf.h>
#include <api/dll_iges_entity.h>

class MCAD_API DLL_IGES_ENTITY_406 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_406( IGES* aParent, bool create );
    DLL_IGES_ENTITY_406( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_406();

    virtual bool NewEntity( void );

    // ========================================================================
    //                Type / Form specific operators
    // ========================================================================

    // Form 15: Name

    // Returns a pointer to the Name if the Property is Form 15, otherwise NULL.
    const char *GetProperty_Name( void );

    // Sets the name and returns true if the Property is Form 15,
    // otherwise makes no changes and returns false. The name is
    // stored as a Hollerith string and in principle is only limited
    // by the number of remaining Property Entity lines.
    bool        SetProperty_Name( const char * aName );
};

#endif  // DLL_ENTITY_406_H
