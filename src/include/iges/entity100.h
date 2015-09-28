/*
 * file: entity100.h
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
    IGES_ENTITY_100( IGES* aParent );
    virtual ~IGES_ENTITY_100();

    // Inherited from IGES_CURVE
    virtual bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    virtual bool GetEndPoint( MCAD_POINT& pt, bool xform = true );

    virtual int GetNSegments( void );
    virtual bool IsClosed( void );
    virtual int GetNCurves( void );
    virtual IGES_CURVE* GetCurve( int index );

    // Inherited from IGES_ENTITY
    virtual bool SetEntityForm( int aForm );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    /// Z plane of the circular arc
    union
    {
        double zOffset;     // ZT in the IGES spec
        double ZT;
    };

    /// X value, arc center
    union
    {
        double xCenter;     // X1
        double X1;
    };

    /// Y value, arc center
    union
    {
        double yCenter;     // Y1
        double Y1;
    };

    /// X value, arc start point
    union
    {
        double xStart;      // X2
        double X2;
    };

    /// Y value, arc start point
    union
    {
        double yStart;      // Y2
        double Y2;
    };

    /// X value, arc end point
    union
    {
        double xEnd;        // X3
        double X3;
    };

    /// X value, arc end point
    union
    {
        double yEnd;        // Y3
        double Y3;
    };
};

#endif  // ENTITY_100_H
