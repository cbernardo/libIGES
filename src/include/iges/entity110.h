/*
 * file: entity110.h
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
 * Description: IGES Entity 110: Line, Section 4.13, p.96+ (124+)
 */

#ifndef ENTITY_110_H
#define ENTITY_110_H

#include <libigesconf.h>
#include <iges_curve.h>

// NOTE:
// The associated parameter data are:
// + X1: Real: Start point
// + Y1: Real: Start point
// + Z1: Real: Start point
// + X2: Real: End point
// + Y2: Real: End point
// + Z2: Real: End point
//
// Forms:
//  0: line from Xt(1),Yt(1),Zt(1) to Xt(2),Yt(2),Zt(2)
//  1: semi-bounded (has origin at P1 but no terminus)
//  2: unbounded (infinite line for drawing)
//  The difference in the forms lies in the imposed limitations
//  on the parameterization variable (0..1, 0..+inf, -inf..+inf)
//
// Unused DE items:
// + Structure
//


/**
 * Class IGES_ENTITY_110
 * represents a line segment, ray, or line
 */
class MCAD_API IGES_ENTITY_110 : public IGES_CURVE
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    IGES_ENTITY_110( IGES* aParent );
    virtual ~IGES_ENTITY_110();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm(int aForm);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    double X1;  // X value, start point
    double Y1;  // Y value, start point
    double Z1;  // Z value, start point
    double X2;  // X value, end point
    double Y2;  // Y value, end point
    double Z2;  // Z value, end point

    // methods required of parameterized curve entities
    virtual bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    virtual bool GetEndPoint( MCAD_POINT& pt, bool xform = true );
    virtual int GetNSegments( void );
    virtual bool IsClosed( void );
    virtual int GetNCurves( void );
    virtual IGES_CURVE* GetCurve( int index );
    virtual bool Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform = true );
};

#endif  // ENTITY_110_H
