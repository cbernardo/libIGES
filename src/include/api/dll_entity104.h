/*
 * file: dll_entity104.h
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
 * Description: IGES Entity 104: Conic Arc, Section 4.5, p.74+ (102+)
 */

#ifndef DLL_ENTITY_104_H
#define DLL_ENTITY_104_H

#include <libigesconf.h>
#include <dll_iges_curve.h>


class MCAD_API DLL_IGES_ENTITY_104 : public DLL_IGES_CURVE
{
public:
    DLL_IGES_ENTITY_104( IGES* aParent, bool create );
    DLL_IGES_ENTITY_104( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_104();

    virtual bool NewEntity( void );

    bool GetConicStart( MCAD_POINT& pt );
    bool GetConicStart( double& aX, double& aY, double& aZ );
    bool GetConicEnd( double& aX, double& aY, double& aZ );
    bool GetConicEnd( MCAD_POINT& pt );
    bool GetConicParams( double& P1, double& P2, double& P3, double& P4,
                         double& P5, double& P6 );

    bool SetConicStart( MCAD_POINT pt );
    bool SetConicStart( double aX, double aY, double aZ );
    bool SetConicEnd( MCAD_POINT pt );
    bool SetConicEnd( double aX, double aY );
    bool SetConicParams( double P1, double P2, double P3, double P4,
                         double P5, double P6 );
};

#endif  // DLL_ENTITY_104_H
