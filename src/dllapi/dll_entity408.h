/*
 * file: dll_entity408.h
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
 * Description: IGES Entity 4-8: Subfigure Instance, Section 4.137, p.557(585+)
 */

#ifndef DLL_ENTITY_408_H
#define DLL_ENTITY_408_H

#include <libigesconf.h>
#include <mcad_elements.h>
#include <dll_iges_entity.h>

class IGES_ENTITY_308;

class MCAD_API DLL_IGES_ENTITY_408 : public DLL_IGES_ENTITY
{
public:
    DLL_IGES_ENTITY_408( IGES* aParent, bool create );
    DLL_IGES_ENTITY_408( DLL_IGES& aParent, bool create );
    virtual ~DLL_IGES_ENTITY_408();

    bool GetSubfigure( IGES_ENTITY_308*& aPtr );
    bool SetSubfigure( IGES_ENTITY_308* aPtr );
    bool GetSubfigParams( double& aX, double& aY, double& aZ, double& aScale );
    bool SetSubfigParams( double aX, double aY, double aZ, double aScale = 1.0 );

    // parameters
    double X;   //< X translation relative to type space of a referring entity
    double Y;   //< Y translation relative to type space of a referring entity
    double Z;   //< Z translation relative to type space of a referring entity
    double S;   //< scale factor relative to type space of a referring entity


    bool GetDE( IGES_ENTITY_308*& aPtr );
    bool SetDE( IGES_ENTITY_308* aPtr );
};

#endif  // DLL_ENTITY_408_H
