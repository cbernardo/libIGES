/*
 * file: dll_entity142.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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
 * Description: IGES Entity 142: Curve on a Parametric Surface, Section 4.32, p.178 (206+)
 */

#ifndef DLL_ENTITY_142_H
#define DLL_ENTITY_142_H

#include <libigesconf.h>
#include <api/dll_iges_entity.h>

enum BOUND_CURVE_PREF
{
    BOUND_PREF_UNSPECIFIED = 0,
    BOUND_PREF_PARAMSPACE,
    BOUND_PREF_MODELSPACE,
    BOUND_PREF_ANY
};


enum CURVE_CREATION
{
    CURVE_CREATE_UNSPECIFIED = 0,
    CURVE_CREATE_PROJECTION,
    CURVE_CREATE_INTERSECTION,
    CURVE_CREATE_PARAMETRIC
};


class MCAD_API DLL_IGES_ENTITY_142 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_142( IGES* aParent, bool create );
    DLL_IGES_ENTITY_142( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_142();

    virtual bool NewEntity( void );

    bool GetSurface( IGES_ENTITY** aPtr );
    bool SetSurface( IGES_ENTITY* aPtr );
    bool GetParameterSpaceBound( IGES_ENTITY** aPtr );
    bool SetParameterSpaceBound( IGES_ENTITY* aPtr );
    bool GetModelSpaceBound( IGES_ENTITY** aPtr );
    bool SetModelSpaceBound( IGES_ENTITY* aPtr );
    bool GetCurvePreference( BOUND_CURVE_PREF& aPref );
    bool SetCurvePreference( BOUND_CURVE_PREF aPref );
    bool GetCurveCreationFlag( CURVE_CREATION& aFlag );
    bool SetCurveCreationFlag( CURVE_CREATION aFlag );
};

#endif  // DLL_ENTITY_142_H
