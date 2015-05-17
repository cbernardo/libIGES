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

#include <iges_base.h>
#include <iges_entity.h>
#include <iges_elements.h>

class IGES;             // Overarching data structure and parent to all entities
struct IGES_RECORD;     // Partially parsed single line of data from an IGES file

// Base class for all IGES Curve entities
class IGES_CURVE : public IGES_ENTITY
{
protected:

    // members inherited from IGES_ENTITY
    virtual bool format( int &index ) = 0;
    virtual bool rescale( double sf ) = 0;

public:
    IGES_CURVE( IGES* aParent );
    virtual ~IGES_CURVE();
    virtual bool Associate(std::vector<IGES_ENTITY*>* entities) = 0;

    // specialized members of this class
    // methods required of parameterized curve entities

    // return true if this entity represents a closed curve
    virtual bool IsClosed( void ) = 0;
    // return the number of curves in this object;
    // -2 = Point or Point Association entity
    // -1 = no valid curve entities
    // 0 = simple curve entity
    // 1+ = # of internal curve entities (may also be composite)
    virtual int GetNCurves( void ) = 0;
    // return the specified curve object by index
    virtual IGES_CURVE* GetCurve( int index ) = 0;
    // return the start point of this curve object (normally transformed)
    virtual bool GetStartPoint( IGES_POINT& pt, bool xform = true ) = 0;
    // return the end point of this curve object (normally transformed)
    virtual bool GetEndPoint( IGES_POINT& pt, bool xform = true ) = 0;
    // return the number of segments within the curve; for
    // composite curves this may be the same as GetNCurves
    // but in the case of piece-wise linear collections this would
    // be the number of segments to iterate over
    virtual int GetNSegments( void ) = 0;
    // Interpolate on Segment #nSeg of the curve
    // pt: point which will hold the interpolated value
    // nSeg: Segment number to interpolate along (1 .. GetNSegments())
    // var: point along the segment, 0 .. 1
    // xform: true if the point is to be transformed; false if raw point data
    //        is to be retrieved.
    // Only simple curves (linear piecewise curve included) return an
    // interpolated value; composite curves shall return false; a
    // composite curve can be identified by a non-zero return
    // from GetNCurves()
    virtual bool Interpolate( IGES_POINT& pt, int nSeg, double var, bool xform = true ) = 0;

    // members inherited from IGES_ENTITY
    virtual bool Unlink( IGES_ENTITY* aChild ) = 0;
    virtual bool IsOrphaned( void ) = 0;
    virtual bool AddReference( IGES_ENTITY* aParentEntity ) = 0;
    virtual bool DelReference( IGES_ENTITY* aParentEntity ) = 0;
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar ) = 0;
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar ) = 0;
    virtual bool SetEntityForm( int aForm ) = 0;
    // XXX - consider adding a method to retrieve the LENGTH of Segment N;
    // this could be useful to aid in calculations for rendering entities.
};

#endif  // IGES_CURVE_H
