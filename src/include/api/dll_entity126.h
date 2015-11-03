/*
 * file: dll_entity126.h
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

#ifndef DLL_ENTITY_126_H
#define DLL_ENTITY_126_H

#include <libigesconf.h>
#include <api/dll_iges_curve.h>
#include <geom/mcad_elements.h>

class MCAD_API DLL_IGES_ENTITY_126 : public DLL_IGES_CURVE
{
public:
    DLL_IGES_ENTITY_126( IGES* aParent, bool create );
    DLL_IGES_ENTITY_126( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_126();

    virtual bool NewEntity( void );

    bool GetNURBSData( int& nCoeff, int& order, double** knot, double** coeff,
        bool& isRational, bool& isClosed, bool& isPeriodic, double& v0, double& v1 );
    bool SetNURBSData( int nCoeff, int order, const double* knot, const double* coeff,
       bool isRational, double v0, double v1 );
    bool IsPlanar( bool& aResult );
    bool IsRational( bool& aResult );
    bool isPeriodic( bool& aResult );
    bool GetNormal( MCAD_POINT& aNorm );
};

#endif  // DLL_ENTITY_126_H
