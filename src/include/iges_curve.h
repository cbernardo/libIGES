/*
 * file: iges_curve.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Base entity of all IGES Entity classes which represent curves.
 * These include:
 *      100 ENT_CIRCULAR_ARC
 *      104 ENT_CONIC_ARC
 *      110 ENT_LINE
 *      112 ENT_PARAM_SPLINE_CURVE
 *      116 *ENT_POINT
 *      126 ENT_NURBS_CURVE
 *      132 *ENT_CONNECT_POINT
 *      106 ENT_COPIOUS_DATA FORMS:
 *          1, 2, 3
 *          11, 12, 13
 *          63
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


#ifndef IGES_CURVE_H
#define IGES_CURVE_H

#include <iostream>
#include <string>
#include <list>

#include "iges_base.h"
#include "iges_entity.h"
#include "iges_elements.h"

class IGES;             // Overarching data structure and parent to all entities
struct IGES_RECORD;     // Partially parsed single line of data from an IGES file

// Base class for all IGES Curve entities
class IGES_CURVE : public IGES_ENTITY
{
protected:

    // members inherited from IGES_ENTITY
    virtual bool associate(std::vector<IGES_ENTITY*>* entities) = 0;
    virtual bool format( int &index ) = 0;
    virtual bool rescale( double sf ) = 0;

public:
    IGES_CURVE( IGES* aParent );
    virtual ~IGES_CURVE();

    // specialized members of this class
    // methods required of parameterized curve entities
    virtual IGES_POINT GetStartPoint( bool xform = true ) = 0;
    virtual IGES_POINT GetEndPoint( bool xform = true ) = 0;
    virtual int GetNSegments( void ) = 0;
    // XXX - Interpolator: bool F( Point&, nSeg, var, )


    // members inherited from IGES_ENTITY
    virtual bool Unlink( IGES_ENTITY* aChild ) = 0;
    virtual bool IsOrphaned( void ) = 0;
    virtual bool AddReference( IGES_ENTITY* aParentEntity ) = 0;
    virtual bool DelReference( IGES_ENTITY* aParentEntity ) = 0;
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar ) = 0;
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar ) = 0;
    virtual bool SetEntityForm( int aForm ) = 0;
};

#endif  // IGES_CURVE_H
