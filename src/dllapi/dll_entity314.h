/*
 * file: dll_entity314.h
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

/*
 * Description: IGES Entity 314: Color, Section 4.77, p.386 (414)
 */

#ifndef DLL_ENTITY314_H
#define DLL_ENTITY314_H

#include <libigesconf.h>
#include <dll_iges_entity.h>

class DLL_IGES_ENTITY_314 : public DLL_IGES_ENTITY
{
    DLL_IGES_ENTITY_314( IGES* aParent, bool create );
    DLL_IGES_ENTITY_314( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_314();

    bool Attach( IGES_ENTITY* aEntity );

    bool GetColor( double& aRed, double& aGreen, double& aBlue );
    bool SetColor( double aRed, double aGreen, double aBlue );
    bool GetName( const char*& aName );
    bool SetName( const char*& aName );
};

#endif  // DLL_ENTITY314_H
