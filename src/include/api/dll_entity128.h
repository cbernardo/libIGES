/*
 * file: dll_entity128.h
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
 * Description: IGES Entity 128: NURBS Surface, Section 4.24, p.137(165+)
 */

#ifndef DLL_ENTITY_128_H
#define DLL_ENTITY_128_H

#include <libigesconf.h>
#include <dll_iges_entity.h>
#include <mcad_elements.h>


class MCAD_API DLL_IGES_ENTITY_128 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_128( IGES* aParent, bool create );
    DLL_IGES_ENTITY_128( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_128();

    virtual bool NewEntity( void );

    bool GetNURBSData( int& nCoeff1, int& nCoeff2, int& order1, int& order2,
                       double** knot1, double** knot2, double** coeff,
                       bool& isRational, bool& isClosed1, bool& isClosed2,
                       bool& isPeriodic1, bool& isPeriodic2,
                       double& u0, double& u1, double& v0, double& v1 );

    bool SetNURBSData( int nCoeff1, int nCoeff2, int order1, int order2,
                       const double* knot1, const double* knot2,
                       const double* coeff, bool isRational,
                       bool isPeriodic1, bool isPeriodic2,
                       double u0, double u1, double v0, double v1 );

    bool IsRational( bool& aResult );
    bool isClosed1( bool& aResult );
    bool isClosed2( bool& aResult );
    bool isPeriodic1( bool& aResult );
    bool isPeriodic2( bool& aResult );
};

#endif  // DLL_ENTITY_128_H
