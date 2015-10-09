/*
 * file: dll_entity110.h
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
 * Description: IGES Entity 110: Line, Section 4.13, p.96+ (124+)
 */

#ifndef DLL_ENTITY_110_H
#define DLL_ENTITY_110_H

#include <libigesconf.h>
#include <dll_iges_curve.h>

class MCAD_API DLL_IGES_ENTITY_110 : public DLL_IGES_CURVE
{
public:
    DLL_IGES_ENTITY_110( IGES* aParent, bool create );
    DLL_IGES_ENTITY_110( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_110();

    virtual bool NewEntity( void );

    bool GetLineStart( MCAD_POINT& pt );
    bool GetLineStart( double& aX, double& aY, double& aZ );
    bool GetLineEnd( MCAD_POINT& pt );
    bool GetLineEnd( double& aX, double& aY, double& aZ );

    bool SetLineStart( MCAD_POINT pt );
    bool SetLineStart( double aX, double aY, double aZ );
    bool SetLineEnd( MCAD_POINT pt );
    bool SetLineEnd( double aX, double aY, double aZ );
};

#endif  // DLL_ENTITY_110_H
