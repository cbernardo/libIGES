/*
 * file: entity100.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 100: Circle, Section 4.3, p.66+ (94+)
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

#ifndef ENTITY_100_H
#define ENTITY_100_H

#include <libigesconf.h>
#include <iges_curve.h>

// NOTE:
// Arcs must lie in the Xt-Yt plane (constant Z in definition space coordinates).
// Arcs are defined by their endpoints and center and are traced in a CCW fashion
// from First Endpoint to Second Endpoint (similar to the IDF specification).
//
// The associated parameter data are:
// + ZT: Real: Zt displacement of the arc from the Xt-Yt plane
// + X1: Real: Arc center, abscissa
// + Y1: Real: Arc center, ordinate
// + X2: Real: Start point, abscissa
// + Y2: Real: Start point, ordinate
// + X3: Real: End point, abscissa
// + Y3: Real: End point, ordinate
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class MCAD_API IGES_ENTITY_100 : public IGES_CURVE
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    // parameters used in interpolations
    double radius;
    double startAng;    // start angle
    double endAng;      // terminate angle

public:
    IGES_ENTITY_100( IGES* aParent );
    virtual ~IGES_ENTITY_100();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

    // Inherited from IGES_CURVE
    virtual bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    virtual bool GetEndPoint( MCAD_POINT& pt, bool xform = true );

    virtual int GetNSegments( void );
    virtual bool IsClosed( void );
    virtual int GetNCurves( void );
    virtual IGES_CURVE* GetCurve( int index );
    virtual bool Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform = true );

    // Inherited from IGES_ENTITY
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    union
    {
        double zOffset;     // ZT in the IGES spec
        double ZT;
    };

    union
    {
        double xCenter;     // X1
        double X1;
    };

    union
    {
        double yCenter;     // Y1
        double Y1;
    };

    union
    {
        double xStart;      // X2
        double X2;
    };

    union
    {
        double yStart;      // Y2
        double Y2;
    };

    union
    {
        double xEnd;        // X3
        double X3;
    };

    union
    {
        double yEnd;        // Y3
        double Y3;
    };
};

#endif  // ENTITY_100_H
