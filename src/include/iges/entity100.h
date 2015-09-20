/*
 * file: entity100.h
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
 * Description: IGES Entity 100: Circle, Section 4.3, p.66+ (94+)
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


/**
 * Class IGES_ENTITY_100
 * represents a circle or a counterclockwise circular arc comprised of
 * a Z plane, center point and arc start and end points; a circle has
 * an end point which is the same as its start point.
 */
class IGES_ENTITY_100 : public IGES_CURVE
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
    // public functions for internal libIGES use
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

public:
    MCAD_API IGES_ENTITY_100( IGES* aParent );
    virtual MCAD_API ~IGES_ENTITY_100();

    // Inherited from IGES_CURVE
    virtual MCAD_API bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    virtual MCAD_API bool GetEndPoint( MCAD_POINT& pt, bool xform = true );

    virtual MCAD_API int GetNSegments( void );
    virtual MCAD_API bool IsClosed( void );
    virtual MCAD_API int GetNCurves( void );
    virtual MCAD_API IGES_CURVE* GetCurve( int index );
    virtual MCAD_API bool Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform = true );

    // Inherited from IGES_ENTITY
    virtual MCAD_API bool SetEntityForm( int aForm );
    virtual MCAD_API bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    /// Z plane of the circular arc
    MCAD_API union
    {
        double zOffset;     // ZT in the IGES spec
        double ZT;
    };

    /// X value, arc center
    MCAD_API union
    {
        double xCenter;     // X1
        double X1;
    };

    /// Y value, arc center
    MCAD_API union
    {
        double yCenter;     // Y1
        double Y1;
    };

    /// X value, arc start point
    MCAD_API union
    {
        double xStart;      // X2
        double X2;
    };

    /// Y value, arc start point
    MCAD_API union
    {
        double yStart;      // Y2
        double Y2;
    };

    /// X value, arc end point
    MCAD_API union
    {
        double xEnd;        // X3
        double X3;
    };

    /// X value, arc end point
    MCAD_API union
    {
        double yEnd;        // Y3
        double Y3;
    };
};

#endif  // ENTITY_100_H
