/*
 * file: dll_entity124.h
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
 * Description: IGES Entity 124: Transformation Matrix, Section 4.21, p.123+ (151+)
 */

#ifndef DLL_ENTITY_124_H
#define DLL_ENTITY_124_H

#include <libigesconf.h>
#include <geom/mcad_elements.h>
#include <api/dll_iges_entity.h>

class MCAD_API DLL_IGES_ENTITY_124 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_124( IGES* aParent, bool create );
    DLL_IGES_ENTITY_124( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_124();

    virtual bool NewEntity( void );

    bool GetOverallTransform( MCAD_TRANSFORM& aTX );
    bool GetRootTransform( MCAD_TRANSFORM& aTX );
    bool SetRootTransform( MCAD_TRANSFORM*& aTX );
    // XXX - add DLL_ENTITY accessors
};

#endif  // DLL_ENTITY_124_H
