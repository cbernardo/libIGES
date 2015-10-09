/*
 * file: dll_entity314.h
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
 * Description: IGES Entity 314: Color, Section 4.77, p.386 (414)
 */

#ifndef DLL_ENTITY314_H
#define DLL_ENTITY314_H

#include <libigesconf.h>
#include <dll_iges_entity.h>

class MCAD_API DLL_IGES_ENTITY_314 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_314( IGES* aParent, bool create );
    DLL_IGES_ENTITY_314( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_314();

    virtual bool NewEntity( void );

    bool GetColor( double& aRed, double& aGreen, double& aBlue );
    bool SetColor( double aRed, double aGreen, double aBlue );
    bool GetName( const char*& aName );
    bool SetName( const char* aName );
};

#endif  // DLL_ENTITY314_H
