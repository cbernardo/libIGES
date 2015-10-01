/*
 * file: dll_iges_curve.h
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

#ifndef DLL_IGES_CURVE_H
#define DLL_IGES_CURVE_H

#include <libigesconf.h>
#include <dll_iges_entity.h>
#include <mcad_elements.h>

class IGES_ENTITY;
class IGES_CURVE;

/**
 * Class IGES_CURVE
 * is the base class of all IGES Curve entities
 */
class MCAD_API DLL_IGES_CURVE : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_CURVE( IGES* aParent );
    DLL_IGES_CURVE( DLL_IGES& aParent );
    virtual ~DLL_IGES_CURVE();

    virtual bool NewEntity( void ) = 0;

    bool IsClosed( void );
    int GetNCurves( void );
    IGES_CURVE* GetCurve( int index );
    bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    bool GetEndPoint( MCAD_POINT& pt, bool xform = true );
    int GetNSegments( void );
};

#endif  // IGES_CURVE_H
