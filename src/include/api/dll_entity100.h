/*
 * file: dll_entity100.h
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


#ifndef DLL_ENTITY_100_H
#define DLL_ENTITY_100_H

#include <libigesconf.h>
#include <api/dll_iges_curve.h>

class MCAD_API DLL_IGES_ENTITY_100 : public DLL_IGES_CURVE
{
public:
    // note: all implementations must have constructors which:
    // a. takes a IGES* and creation flag
    // b. takes a DLL_IGES& and creation flag
    // If the creation flag is 'true' then a new IGES_ENTITY
    // of the specified type is created; otherwise the DLL entity
    // serves as a convenient manipulator of an IGES_ENTITY to
    // be specified by the user.
    DLL_IGES_ENTITY_100( IGES* aParent, bool create );
    DLL_IGES_ENTITY_100( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_100();

    virtual bool NewEntity( void );

    bool GetCircleCenter( double& aX, double& aY, double& aZ ) const;
    bool GetCircleCenter( MCAD_POINT& aPoint ) const;

    bool GetCircleStart( double& aX, double& aY, double& aZ ) const;
    bool GetCircleStart( MCAD_POINT& aPoint ) const;

    bool GetCircleEnd( double& aX, double& aY, double& aZ ) const;
    bool GetCircleEnd( MCAD_POINT& aPoint ) const;

    bool SetCircleCenter( double aX, double aY, double aZ );
    bool SetCircleCenter( const MCAD_POINT& aPoint );

    bool SetCircleStart( double aX, double aY );
    bool SetCircleStart( const MCAD_POINT& aPoint );

    bool SetCircleEnd( double aX, double aY );
    bool SetCircleEnd( const MCAD_POINT& aPoint );
};

#endif  // DLL_ENTITY_100_H
