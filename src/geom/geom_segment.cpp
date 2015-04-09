/*
 * file: geom_segment.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: object to aid in the creation of an IGES model
 * for the top and bottom surfaces of a PCB. A segment may be
 * a circular arc (aka arc), a circle, or a line and is capable
 * of computing its intersection with any other given segment.
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

#include <cmath>
#include <iges.h>
#include <error_macros.h>
#include <iges_helpers.h>
#include <geom_segment.h>
#include <geom_wall.h>
#include <geom_cylinder.h>

#define SEGTYPE_NONE 0
#define SEGTYPE_LINE 1
#define SEGTYPE_ARC 2
#define SEGTYPE_CIRCLE 4


using namespace std;


IGES_GEOM_SEGMENT::IGES_GEOM_SEGMENT()
{
    init();
}


IGES_GEOM_SEGMENT::~IGES_GEOM_SEGMENT()
{
    return;
}


void IGES_GEOM_SEGMENT::init( void )
{
    msegtype = SEGTYPE_NONE;
    mCWArc = false;
    mradius = 0.0;
    msang = 0.0;
    meang = 0.0;

    mcenter.x = 0.0;
    mcenter.y = 0.0;
    mcenter.z = 0.0;

    mstart.x = 0.0;
    mstart.y = 0.0;
    mstart.z = 0.0;

    mend.x = 0.0;
    mend.y = 0.0;
    mend.z = 0.0;

    return;
}


// set the parameters for a line
bool IGES_GEOM_SEGMENT::SetParams( IGES_POINT aStart, IGES_POINT aEnd )
{
    init();

    if( 0 != aStart.z || 0 != aEnd.z )
    {
        ERRMSG << "\n + [ERROR] non-0 z values in points\n";
        return false;
    }

    if( PointMatches( aStart, aEnd, 1e-8 ) )
    {
        ERRMSG << "\n + [ERROR] degenerate line\n";
        return false;
    }

    mstart = aStart;
    mend = aEnd;
    msegtype = SEGTYPE_LINE;
    return true;
}

// set the parameters for an arc; the parameters must be specified such that
// the arc is traced in a counterclockwise direction as viewed from a positive
// Z location.
bool IGES_GEOM_SEGMENT::SetParams( IGES_POINT aCenter, IGES_POINT aStart,
                                   IGES_POINT aEnd, bool isCW )
{
    init();

    if( 0 != aCenter.z || 0 != aStart.z || 0 != aEnd.z )
    {
        ERRMSG << "\n + [ERROR] non-0 z values in points\n";
        return false;
    }

    if( PointMatches( aCenter, aStart, 1e-8 )
        || PointMatches( aCenter, aEnd, 1e-8 ) )
    {
        ERRMSG << "\n + [ERROR] degenerate arc\n";
        return false;
    }

    double dx;
    double dy;

    dx = aStart.x - aCenter.x;
    dy = aStart.y - aCenter.y;
    mradius = sqrt( dx*dx + dy*dy );

    if( PointMatches( aStart, aEnd, 1e-8 ) )
    {
        msegtype = SEGTYPE_CIRCLE;
        mcenter = aCenter;
        mstart = mcenter;
        mstart.x += mradius;
        mend = mstart;
        return true;
    }

    dx = aEnd.x - aCenter.x;
    dy = aEnd.y - aCenter.y;
    double r2 = sqrt( dx*dx + dy*dy );

    if( abs(r2 - mradius) > 1.0e-3 )
    {
        mradius = 0;
        ERRMSG << "\n + [ERROR] radii differ by > 1e-3\n";
        return false;
    }

    msang = atan2( aStart.y - aCenter.y, aStart.x - aCenter.x );
    meang = atan2( aEnd.y - aCenter.y, aEnd.x - aCenter.x );

    // note: start/end angles are always according to CCW order
    if( isCW )
    {
        double x = msang;
        msang = meang;
        meang = x;
    }

    while( meang < msang )
        meang += 2.0 * M_PI;

    mcenter = aCenter;
    mstart = aStart;
    mend = aEnd;

    mstart = aStart;
    mend = aEnd;
    msegtype = SEGTYPE_ARC;
    mCWArc = isCW;
    return true;
}

// calculate intersections with another segment (list of points)
bool IGES_GEOM_SEGMENT::GetIntersections( const IGES_GEOM_SEGMENT& aSegment,
                                          std::list<IGES_POINT>& aIntersectList,
                                          IGES_INTERSECT_FLAG& flags )
{
    flags = IGES_IFLAG_NONE;

    if( SEGTYPE_NONE == msegtype )
    {
        ERRMSG << "\n + [ERROR] no data in segment\n";
        return false;
    }

    // cases to check for:
    // a. circles are identical (bad geometry, return IGES_IFLAG_IDENT)
    // b. *this is inside aSegment and both entities are circles
    //    (bad geometry, return IGES_IFLAG_INSIDE)
    // c. *this surrounds aSegment and both entities are circles

    // cases to evaluate:
    // a. circle, circle
    // b. circle, arc
    // c. arc, circle
    // d. circle, line
    // e. line, circle
    // f. arc, line
    // g. line, arc
    // h. line, line
    // i. arc, arc
    char oSegType = aSegment.getSegType();

    if( SEGTYPE_NONE == oSegType )
    {
        ERRMSG << "\n + [ERROR] no data in second segment\n";
        return false;
    }

    // *this is a circle and it may intersect with a circle, arc, or line
    if( SEGTYPE_CIRCLE == msegtype )
    {
        if( SEGTYPE_CIRCLE == oSegType )
            return checkCircles( aSegment, aIntersectList, flags );

        if( SEGTYPE_ARC == oSegType )
            return checkArcs(  aSegment, aIntersectList, flags );

        return checkArcLine(  aSegment, aIntersectList, flags );
    }

    // *this is an arc and it may intersect with a line, arc, or circle
    if( SEGTYPE_ARC == msegtype )
    {
        if( SEGTYPE_LINE == oSegType )
            return checkArcLine(  aSegment, aIntersectList, flags );

        return checkArcs(  aSegment, aIntersectList, flags );
    }

    // *this is a line and it may intersect with a line, arc or circle
    if( SEGTYPE_LINE == oSegType )
        return checkLines(  aSegment, aIntersectList, flags );

    return checkArcLine( aSegment, aIntersectList, flags );
}


// split at the given list of intersections (1 or 2 intersections only)
bool IGES_GEOM_SEGMENT::Split( std::list<IGES_POINT>& aIntersectList,
                               std::list<IGES_GEOM_SEGMENT*> aNewSegmentList )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// retrieve the representation of the curve as IGES 2D primitives
bool IGES_GEOM_SEGMENT::GetCurves( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// retrieve the curve as a parametric curve on plane
bool IGES_GEOM_SEGMENT::GetCurveOnPlane(  IGES* aModel, std::list<IGES_ENTITY_126*> aCurves,
                        double aMinX, double aMaxX, double aMinY, double aMaxY,
                        double zHeight )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// retrieve a trimmed parametric surface representing a vertical side
bool IGES_GEOM_SEGMENT::GetVerticalSurface( IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                            double aTopZ, double aBotZ )
{
    if( !aModel )
    {
        ERRMSG << "\n + [ERROR] null pointer passed for IGES model\n";
        return false;
    }

    if( abs( aTopZ - aBotZ ) < 1e-6 )
    {
        ERRMSG << "\n + [ERROR] degenerate surface\n";
        return false;
    }

    if( !msegtype )
    {
        ERRMSG << "\n + [ERROR] no model data to work with\n";
        return false;
    }

    bool ok = false;

    switch( msegtype )
    {
        case SEGTYPE_CIRCLE:
        case SEGTYPE_ARC:
            do
            {
                IGES_GEOM_CYLINDER cyl;

                if( !mCWArc )
                    cyl.SetParams( mcenter, mstart, mend );
                else
                    cyl.SetParams( mcenter, mend, mstart );

                ok = cyl.Instantiate( aModel, aTopZ, aBotZ, aSurface );
            } while( 0 );

            break;

        default:
            do
            {
                IGES_GEOM_WALL wall;
                IGES_POINT p0 = mstart;
                p0.z = aTopZ;
                IGES_POINT p1 = mend;
                p1.z = aTopZ;
                IGES_POINT p2 = mend;
                p2.z = aBotZ;
                IGES_POINT p3 = mstart;
                p3.z = aBotZ;
                wall.SetParams( p0, p1, p2, p3 );
                IGES_ENTITY_144* ep = wall.Instantiate( aModel );

                if( NULL == ep )
                {
                    ERRMSG << "\n + [ERROR] could not create solid model feature\n";
                    ok = false;
                }
                else
                {
                    aSurface.push_back( ep );
                    ok = true;
                }

            } while( 0 );

            break;
    }

    return ok;
}


void IGES_GEOM_SEGMENT::calcCircleIntercepts( IGES_POINT c2, double r2, double d,
    IGES_POINT& p1, IGES_POINT& p2 )
{
    // note: given distance d between 2 circle centers
    // where radii = R[1], R[2],
    // distance x to the radical line as measured from
    // C[1] is (d^2 - R[2]^2 + R[1]^2)/(2d)

    double rd = (d*d - r2*r2 + mradius*mradius)/(2.0 * d);
    double dy = c2.y - mcenter.y;
    double dx = c2.x - mcenter.x;

    // intersection of the radical line and the line passing through the centers:
    // the calculation is parameterized to avoid divisions by 0 provided d != 0.
    double x = rd / d * dx + mcenter.x;
    double y = rd / d * dy + mcenter.y;

    // height of the triangle / d
    double h = sqrt( mradius*mradius - rd*rd ) / d;

    // first intersection point
    double x0 = x + h * dy;
    double y0 = y + h * dx;

    // second intersection point
    double x1 = x - h * dy;
    double y1 = y - h * dx;

    // work out which intersection comes first
    // when going clockwise on C1
    double a0 = atan2( y0 - mcenter.y, x0 - mcenter.x );
    double a1 = atan2( y1 - mcenter.y, x1 - mcenter.x );

    if( ( a0 >= 0.0 && a1 >= 0.0 && a0 > a1 )
        || ( a0 < 0.0 && a1 < 0.0 && a0 > a1 )
        || ( a0 < 0.0 && a1 >= 0.0 ) )
    {
        double tv = x0;
        x0 = x1;
        x1 = tv;
        tv = y0;
        y0 = y1;
        y1 = tv;
    }

    p1.x = x0;
    p1.y = y0;
    p1.z = 0.0;

    p2.x = x1;
    p2.y = y1;
    p2.z = 0.0;

    return;
}


// check case where both segments are circles
bool IGES_GEOM_SEGMENT::checkCircles( const IGES_GEOM_SEGMENT& aSegment,
    std::list<IGES_POINT>& aIntersectList, IGES_INTERSECT_FLAG& flags )
{
    IGES_POINT c2 = aSegment.getCenter();
    double r2 = aSegment.getRadius();
    double dx = mcenter.x - c2.x;
    double dy = mcenter.y - c2.y;
    double d= sqrt( dx*dx + dy*dy );

    if( d > ( mradius + r2 ) )
        return false;

    // check if the circles are identical
    if ( PointMatches( mcenter, c2, 0.001 )
        && abs( mradius - r2 ) < 0.001 )
    {
        flags = IGES_IFLAG_IDENT;
        return false;
    }

    if( abs( d - mradius - r2 ) < 0.001 )
    {
        flags = IGES_IFLAG_TANGENT;
        return false;
    }

    if( d < mradius || d < r2)
    {
        // check if aSegment is inside this circle
        if( d <= (mradius - r2) )
        {
            flags = IGES_IFLAG_ENCIRCLES;
            return false;
        }

        // check if this circle is inside aSegment
        if( d <= (r2 - mradius) )
        {
            flags = IGES_IFLAG_INSIDE;
            return false;
        }
    }

    // there must be 2 intersection points
    IGES_POINT p1;
    IGES_POINT p2;
    calcCircleIntercepts( c2, r2, d, p1, p2 );
    aIntersectList.push_back( p1 );
    aIntersectList.push_back( p2 );

    return true;
}


// check case where both segments are arcs (one may be a circle)
bool IGES_GEOM_SEGMENT::checkArcs( const IGES_GEOM_SEGMENT& aSegment,
    std::list<IGES_POINT>& aIntersectList, IGES_INTERSECT_FLAG& flags )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// check case where one segment is an arc and one a line
bool IGES_GEOM_SEGMENT::checkArcLine( const IGES_GEOM_SEGMENT& aSegment,
    std::list<IGES_POINT>& aIntersectList, IGES_INTERSECT_FLAG& flags )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// check case where both segments are lines
bool IGES_GEOM_SEGMENT::checkLines( const IGES_GEOM_SEGMENT& aSegment,
    std::list<IGES_POINT>& aIntersectList, IGES_INTERSECT_FLAG& flags )
{
    // XXX - TO BE IMPLEMENTED
    // Writing each line as:
    // y0 = a0*x + b0
    // y1 = a1*x + b1
    // Solving for x at the intersection:
    // x = (b1 - b0) / (a0 - a1)
    //  >> if a0 == a1 we have parallel lines
    // Solving for y (non parallel cases only):
    // y = a0 * (b1 - b0) / (a0 - a1) + b0

    // cases:
    // 1. parallel: if there is an intersection a segment or the
    //      entire segments overlap
    // 2. non-parallel: if there is an intersection then it is
    //      a unique point which can be parameterized using either
    //      line equation.

    // if( qwerty )
    return false;
}
