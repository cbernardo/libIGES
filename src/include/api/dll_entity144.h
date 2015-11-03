/*
 * file: dll_entity144.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 144: Trimmed Parametric Surface, Section 4.34, p.181 (209+)
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

#ifndef DLL_ENTITY_144_H
#define DLL_ENTITY_144_H

#include <libigesconf.h>
#include <api/dll_iges_curve.h>

class IGES_ENTITY_142;

class MCAD_API DLL_IGES_ENTITY_144 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_144( IGES* aParent, bool create );
    DLL_IGES_ENTITY_144( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_144();

    virtual bool NewEntity( void );

    bool GetSurface( IGES_ENTITY*& aPtr );
    bool SetSurface( IGES_ENTITY* aPtr );
    bool GetBoundCurve( IGES_ENTITY_142*& aPtr );
    bool SetBoundCurve( IGES_ENTITY_142* aPtr );
    bool GetNCutouts( int& aNCutouts );
    bool GetCutouts( size_t& aListSize, IGES_ENTITY_142**& aCutoutList );
    bool AddCutout( IGES_ENTITY_142* aPtr );
    bool DelCutout( IGES_ENTITY_142* aPtr );
};

#endif  // DLL_ENTITY_144_H
