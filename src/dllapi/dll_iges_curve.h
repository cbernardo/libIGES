/*
 * file: dll_iges_curve.h
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
    DLL_IGES_CURVE();
    virtual ~DLL_IGES_CURVE();

    bool IsClosed( void );
    int GetNCurves( void );
    IGES_CURVE* GetCurve( int index );
    bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    bool GetEndPoint( MCAD_POINT& pt, bool xform = true );
    int GetNSegments( void );
    bool Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform = true );
};

#endif  // IGES_CURVE_H