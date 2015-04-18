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
    msegtype = IGES_SEGTYPE_NONE;
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
    msegtype = IGES_SEGTYPE_LINE;
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
        msegtype = IGES_SEGTYPE_CIRCLE;
        mcenter = aCenter;
        mstart = mcenter;
        mstart.x += mradius;
        mend = mstart;
        msang = 0.0;
        meang = 2.0 * M_PI;
        return true;
    }

    dx = aEnd.x - aCenter.x;
    dy = aEnd.y - aCenter.y;
    double r2 = sqrt( dx*dx + dy*dy );

    if( abs(r2 - mradius) > 1e-8 )
    {
        mradius = 0;
        ERRMSG << "\n + [ERROR] radii differ by > 1e-8\n";
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
    msegtype = IGES_SEGTYPE_ARC;
    mCWArc = isCW;
    return true;
}

// calculate intersections with another segment (list of points)
bool IGES_GEOM_SEGMENT::GetIntersections( const IGES_GEOM_SEGMENT& aSegment,
                                          std::list<IGES_POINT>& aIntersectList,
                                          IGES_INTERSECT_FLAG& flags )
{
    flags = IGES_IFLAG_NONE;

    if( IGES_SEGTYPE_NONE == msegtype )
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
    IGES_SEGTYPE oSegType = aSegment.getSegType();

    if( IGES_SEGTYPE_NONE == oSegType )
    {
        ERRMSG << "\n + [ERROR] no data in second segment\n";
        return false;
    }

    // *this is a circle and it may intersect with a circle, arc, or line
    if( IGES_SEGTYPE_CIRCLE == msegtype )
    {
        if( IGES_SEGTYPE_CIRCLE == oSegType )
            return checkCircles( aSegment, aIntersectList, flags );

        if( IGES_SEGTYPE_ARC == oSegType )
            return checkArcs(  aSegment, aIntersectList, flags );

        return checkArcLine(  aSegment, aIntersectList, flags );
    }

    // *this is an arc and it may intersect with a line, arc, or circle
    if( IGES_SEGTYPE_ARC == msegtype )
    {
        if( IGES_SEGTYPE_LINE == oSegType )
            return checkArcLine(  aSegment, aIntersectList, flags );

        return checkArcs(  aSegment, aIntersectList, flags );
    }

    // *this is a line and it may intersect with a line, arc or circle
    if( IGES_SEGTYPE_LINE == oSegType )
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
        case IGES_SEGTYPE_CIRCLE:
        case IGES_SEGTYPE_ARC:
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
    // note: lax evaluation of 1e-3 is used since any
    // smaller difference whether in inch or mm is really
    // too small for PCB manufacturability
    if ( PointMatches( mcenter, c2, 1e-3 )
        && abs( mradius - r2 ) < 1e-3 )
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
    IGES_POINT c2 = aSegment.getCenter();
    double r2 = aSegment.getRadius();
    double dx = mcenter.x - c2.x;
    double dy = mcenter.y - c2.y;
    double d= sqrt( dx*dx + dy*dy );
    double a0 = getStartAngle();
    double a1 = getEndAngle();
    double b0 = aSegment.getStartAngle();
    double b1 = aSegment.getEndAngle();

#warning TO BE IMPLEMENTED
    // XXX - we may have a condition where we have 2 edge overlaps;
    // this violates our constraint of 0 or 2 point intersections
    // and we must inform the calling routine of the problem (invalid geometry)

    // check if no intersection possible
    if( d > ( mradius + r2 ) )
        return false;

    if( abs( d - mradius - r2 ) < 0.001 )
    {
        // calculate the intersection point and check if it lies on both arcs
        double tang0 = atan2( c2.y - mcenter.y, c2.x - mcenter.x );
        double tang1;

        if( tang0 > 0.0 )
            tang1 = tang0 - M_PI;
        else
            tang1 = tang0 + M_PI;

        if( tang0 > a0 )
            tang0 -= 2.0 * M_PI;

        if( tang0 < a0 )
            tang0 += 2.0 * M_PI;

        if( tang1 > b0 )
            tang1 -= 2.0 * M_PI;

        if( tang1 < b0 )
            tang1 += 2.0 * M_PI;

        if( tang0 >= a0 && tang0 <= a1 && tang1 >= b0 && tang1 <= b1 )
            flags = IGES_IFLAG_TANGENT;

        return false;
    }

    // check if the arcs are identical
    if ( PointMatches( mcenter, c2, 1e-3 )
        && abs( mradius - r2 ) < 1e-3 )
    {
        // there may be an intersection along an edge
        if( IGES_SEGTYPE_CIRCLE == msegtype )
        {
            aIntersectList.push_back( aSegment.getStart() );
            aIntersectList.push_back( aSegment.getEnd() );
            flags = IGES_IFLAG_EDGE;
            return true;
        }

        if( IGES_SEGTYPE_CIRCLE == aSegment.getSegType() )
        {
            aIntersectList.push_back( getStart() );
            aIntersectList.push_back( getEnd() );
            flags = IGES_IFLAG_EDGE;
            return true;
        }

        // Special case: arcs only intersect at their endpoints;
        // we must return the endpoints with the flag IGES_IFLAG_ENDPOINT
        // possible sub-cases:
        // b1 == a0, b0 == a1 - 2*M_PI
        // a1 == b0, a0 == b1 - 2*M_PI
        // b0 == a1, b1 == a0 + 2*M_PI
        // a0 == b1, a1 == b0 + 2*M_PI
        if( (abs(b1 - a0) < 1e-8 && abs(b0 -a1 + 2.0*M_PI) < 1e-8)
            || (abs(a1 - b0) < 1e-8 && abs(a0 -b1 + 2.0*M_PI) < 1e-8)
            || (abs(b0 - a1) < 1e-8 && abs(b1 -a0 - 2.0*M_PI) < 1e-8)
            || (abs(a0 - b1) < 1e-8 && abs(a1 -b0 - 2.0*M_PI) < 1e-8) )
        {
            aIntersectList.push_back( getStart() );
            aIntersectList.push_back( getEnd() );
            flags = IGES_IFLAG_ENDPOINT;
            return true;
        }

        // determine if an entire segment is enveloped
        if( (b0 >= a0 && b1 <= a1)
            || ((b0 + 2.0*M_PI) >= a0 && (b1 + 2.0*M_PI) <= a1)
            || ((b0 - 2.0*M_PI) >= a0 && (b1 - 2.0*M_PI) <= a1) )
        {
            aIntersectList.push_back( aSegment.getStart() );
            aIntersectList.push_back( aSegment.getEnd() );
            flags = IGES_IFLAG_EDGE;
            return true;
        }

        if( (a0 >= b0 && a1 <= b1)
            || (a0 >= (b0 + 2.0*M_PI) && a1 <= (b1 + 2.0*M_PI))
            || (a0 >= (b0 - 2.0*M_PI) && a1 <= (b1 - 2.0*M_PI)) )
        {
            aIntersectList.push_back( getStart() );
            aIntersectList.push_back( getEnd() );
            flags = IGES_IFLAG_EDGE;
            return true;
        }

        // case 1: aSegment encompasses the start of *this
        if( (b0 <= a0 && b1 >= a0 && b1 <= a1)
            || ((b0 - 2.0*M_PI) <= a0 && (b1 - 2.0*M_PI) >= a0 && (b1 - 2.0*M_PI) <= a1) )
        {
            aIntersectList.push_back( getStart() );
            aIntersectList.push_back( aSegment.getEnd() );
            flags = IGES_IFLAG_EDGE;
            return true;
        }

        // case 2: aSegment encompasses the end of *this
        if( (b0 >= a0 && b0 <= a1 && b1 >= a1)
            || ((b0 + 2.0*M_PI) >= a0 && (b0 + 2.0*M_PI) <= a1 && (b1 + 2.0*M_PI) >= a1)
            || ((b0 - 2.0*M_PI) >= a0 && (b0 - 2.0*M_PI) <= a1 && (b1 - 2.0*M_PI) >= a1) )
        {
            aIntersectList.push_back( getStart() );
            aIntersectList.push_back( aSegment.getEnd() );
            flags = IGES_IFLAG_EDGE;
            return true;
        }

        return false;
    }

    // Special cases: arcs only intersect at their endpoints
    // 1. aSegment->radius < *this->radius : return flag IGES_IFLAG_OUTSIDE; this
    //    return value can be used by the calling routine to determine whether
    //    nothing should be done (subtracting) or *this arc should be replaced
    //    by aSegment (adding) provided aSegment has no non-endpoint intersections
    //    with all other members of the outline.
    // 2. aSegment->radius > *this->radius and a point halfway along *this
    //    arc is inside aSegment : return flag IGES_IFLAG_INSIDE. For a subtractive
    //    case, the calling routine can take the flag value to indicate that *this
    //    arc must be replaced by aSegment provided there are absolutely no other
    //    intersections in the outline which are not endpoints. For an additive case
    //    nothing is to be done
    // 3. aSegment->radius > *this->radius and a point halfway along *this
    //    arc is outside aSegment : return flag IGES_IFLAG_OUTSIDE

    if( ( PointMatches( getStart(), aSegment.getStart(), 1e-3 )
        && PointMatches( getEnd(), aSegment.getEnd(), 1e-3 ) )
        || (PointMatches( getStart(), aSegment.getEnd(), 1e-3 )
        && PointMatches( getEnd(), aSegment.getStart(), 1e-3 ) ) )
    {
        aIntersectList.push_back( getStart() );
        aIntersectList.push_back( getEnd() );

        if( r2 > mradius )
        {
            double mrad = (a0 + a1) * 0.5;
            double iaX = mcenter.x + mradius * cos( mrad );
            double iaY = mcenter.y + mradius * sin( mrad );

            iaX = iaX - c2.x;
            iaY = iaY - c2.y;

            mrad = sqrt(iaX*iaX + iaY*iaY);

            if( mrad < r2 )
            {
                flags = IGES_IFLAG_INSIDE;
                return true;
            }
        }

        flags = IGES_IFLAG_OUTSIDE;
        return true;
    }

    // the radii differ so if there is any intersection it is at 1 or 2 points
    IGES_POINT p1;
    IGES_POINT p2;
    calcCircleIntercepts( c2, r2, d, p1, p2 );

    // determine if any of the points lie on *this arc and
    // place them in a ccw order

    aIntersectList.push_back( p1 );
    aIntersectList.push_back( p2 );
    double angX = atan2( p1.y - mcenter.y, p1.x - mcenter.x );
    IGES_POINT p0[2];
    double ang0[2];
    int np = 0;

    if( (angX >= a0 && angX <= a1)
        || ((angX + 2.0*M_PI) >= a0 && (angX + 2.0*M_PI) <= a1)
        || ((angX - 2.0*M_PI) >= a0 && (angX - 2.0*M_PI) <= a1) )
    {
        p0[0] = p1;
        ang0[0] = angX;
        ++np;
    }

    angX = atan2( p2.y - mcenter.y, p2.x - mcenter.x );
    if( (angX >= a0 && angX <= a1)
        || ((angX + 2.0*M_PI) >= a0 && (angX + 2.0*M_PI) <= a1)
        || ((angX - 2.0*M_PI) >= a0 && (angX - 2.0*M_PI) <= a1) )
    {
        p0[np] = p2;
        ang0[np] = angX;
        ++np;
    }

    if( 0 == np )
    {
        return false;
    }

    if( 1 == np )
    {
        aIntersectList.push_back( p0[0] );
        return true;
    }

    // adjust range to fit within a0 .. a1
    for( int i = 0; i < np; ++i )
    {
        if( ang0[i] < a0 )
            ang0[i] += 2.0*M_PI;
        else if ( ang0[i] > a1 )
            ang0[i] -= 2.0*M_PI;
    }

    if( ang0[0] > ang0[1] )
    {
        aIntersectList.push_back( p0[1] );
        aIntersectList.push_back( p0[0] );
    }
    else
    {
        aIntersectList.push_back( p0[0] );
        aIntersectList.push_back( p0[1] );
    }

    return true;
}


// check case where one segment is an arc and one a line
bool IGES_GEOM_SEGMENT::checkArcLine( const IGES_GEOM_SEGMENT& aSegment,
    std::list<IGES_POINT>& aIntersectList, IGES_INTERSECT_FLAG& flags )
{
    flags = IGES_IFLAG_NONE;

    // retrieve parameters of the arc and line segment
    IGES_POINT arcC;
    IGES_POINT arcS;
    IGES_POINT arcE;
    double arcR;
    double arcSAng;
    double arcEAng;
    bool arcCircle = false;

    IGES_POINT lS;
    IGES_POINT lE;

    if( IGES_SEGTYPE_ARC == msegtype || IGES_SEGTYPE_CIRCLE == msegtype )
    {
        if( IGES_SEGTYPE_CIRCLE == msegtype )
            arcCircle = true;

        arcSAng = getStartAngle();
        arcEAng = getEndAngle();
        arcR = mradius;
        arcC = mcenter;
        arcS = mstart;
        arcE = mend;

        lS = aSegment.getStart();
        lE = aSegment.getEnd();
    }
    else
    {
        if( IGES_SEGTYPE_CIRCLE == aSegment.getSegType() )
            arcCircle = true;

        arcSAng = aSegment.getStartAngle();
        arcEAng = aSegment.getEndAngle();
        arcR = aSegment.getRadius();
        arcC = aSegment.getCenter();
        arcS = aSegment.getStart();
        arcE = aSegment.getEnd();

        lS = getStart();
        lE = getEnd();
    }

    // Step 1: the line segment must be parameterized:
    // x = t*x1 + (1-t)*x2
    // y = t*y1 + (1-t)*y2
    // Step 2: given a circle with center (x0, y0), solve for:
    // (x0 - x)^2 + (y0 - y)^2 = R^2
    // Intermediates:
    //      + expanding (x0 - x)^2 we get:
    //          t^2*(x1^2 -2x1*x2 +x2^2) +t*2*(x0*x2 -x0*x1 +x1*x2 -x2^2) + (x0^2 -2*x0*x2 +x2^2)
    //        (y0 - y)^2 expands to the same general expression
    //      + gathering known values into single coefficients we get:
    //          a0 = (x1^2 -2x1*x2 +x2^2)
    //          b0 = 2*(x0*x2 -x0*x1 +x1*x2 -x2^2)
    //          c0 = (x0^2 -2*x0*x2 +x2^2)
    //          a1 = (y1^2 -2y1*y2 +y2^2)
    //          b1 = 2*(y0*y2 -y0*y1 +y1*y2 -y2^2)
    //          c1 = (y0^2 -2*y0*y2 +y2^2)
    //      + Step 2 reduces to:
    //          (a0 + a1)*t^2 + (b0 + b1)*t + (c0 + c1 - R^2) = 0
    //        Which is equal to:
    //          A*t^2 + B*t +C = 0
    // Step 3: solution for t:
    //      t = (-B * sqrt( B^2 -4*A*C )) / (2*A)
    // First check the discriminant; if it is == 0 we have a tangent, if <0
    // we have no intersection, and if >0 we may have an intersection.
    // If the discriminant > 0, solve for t and for any value 0<= t <= 1
    // check if p(t) lies on the arc/circle

    double a0 = ( lS.x * lS.x - 2.0 * lS.x * lE.x + lE.x * lE.x );
    double b0 = 2.0 * ( arcC.x * lE.x - arcC.x * lS.x + lS.x * lE.x - lE.x * lE.x );
    double c0 = arcC.x * arcC.x - 2.0 * arcC.x * lE.x + lE.x * lE.x;

    double a1 = ( lS.y * lS.y - 2.0 * lS.y * lE.y + lE.y * lE.y );
    double b1 = 2.0 * ( arcC.y * lE.y - arcC.y * lS.y + lS.y * lE.y - lE.y * lE.y );
    double c1 = arcC.y * arcC.y - 2.0 * arcC.y * lE.y + lE.y * lE.y;

    double A = a0 + a1;
    double B = b0 + b1;
    double C = c0 + c1 - arcR * arcR;

    double D = B * B - 4.0 * A * C;

    bool tangent = false;

    if( abs( D ) < 1e-6 )
    {
        if( arcCircle )
        {
            flags = IGES_IFLAG_TANGENT;
            return false;
        }

        // we only have a problem with tangent geometry if
        // the line actually touches the arc
        tangent = true;
    }

    if( D < 0 )
        return false;

    double t0 = ( -B + sqrt( D ) ) / ( 2.0 * A );
    double t1 = ( -B - sqrt( D ) ) / ( 2.0 * A );

    int np = 0;
    IGES_POINT p[2];
    IGES_INTERSECT_FLAG f[2];

    if( t0 >= 0.0 && t0 <= 1.0 )
    {
        p[0].x = t0 * lS.x + (1.0 - t0) * lE.x;
        p[0].y = t0 * lS.y + (1.0 - t0) * lE.y;

        if( ( ( PointMatches( p[0], arcS, 1e-8 ) || PointMatches( p[0], arcE, 1e-8 ) )
            && !arcCircle )
            || abs( t0 ) < 1e-8 || abs( t0 - 1.0 ) < 1e-8 )
            f[0] = IGES_IFLAG_ENDPOINT;
        else
            f[0] = IGES_IFLAG_NONE;

        ++np;
    }

    if( t1 >= 0.0 && t1 <= 1.0 )
    {
        p[np].x = t1 * lS.x + (1.0 - t1) * lE.x;
        p[np].y = t1 * lS.y + (1.0 - t1) * lE.y;

        if( ( ( PointMatches( p[np], arcS, 1e-8 ) || PointMatches( p[np], arcE, 1e-8 ) )
            && !arcCircle )
            || abs( t1 ) < 1e-8 || abs( t1 - 1.0 ) < 1e-8 )
            f[np] = IGES_IFLAG_ENDPOINT;
        else
            f[np] = IGES_IFLAG_NONE;

        ++np;
    }

    double ang[2];

    for( int i = 0; i < np; ++i )
        ang[i] = atan2( p[i].y - arcC.y, p[i].x - arcC.x );

    if( arcCircle )
    {
        if( 1 == np )
        {
            aIntersectList.push_back( p[0] );
            flags = f[0];
            return true;
        }

        if( IGES_IFLAG_ENDPOINT == f[0] || IGES_IFLAG_ENDPOINT == f[1] )
            flags = IGES_IFLAG_ENDPOINT;

        bool swap = false; // set to true if p[0], p[1] must be swapped

        if( ang[0] >= 0.0 )
        {
            if( ang[1] >= 0.0 && ang[1] < ang[0] )
                    swap = true;
        }
        else
        {
            if( ang[1] >= 0.0 || ang[1] < ang[0] )
                swap = true;
        }

        if( swap )
        {
            aIntersectList.push_back( p[1] );
            aIntersectList.push_back( p[0] );
        }
        else
        {
            aIntersectList.push_back( p[0] );
            aIntersectList.push_back( p[1] );
        }

        return true;
    }

    IGES_POINT pt[2];
    int np2 = 0;

    // check if each point is on the arc
    for( int i = 0; i < np; ++i )
    {
        if( ( ang[i] >= arcSAng && ang[i] <= arcEAng )
            || ( ( ang[i] + 2.0 * M_PI ) >= arcSAng && ( ang[i] + 2.0 * M_PI ) <= arcEAng ) )
        {
            pt[i] = p[i];
            ++np2;
        }
    }

    if( 0 == np2 )
        return false;

    if( tangent )
    {
        flags = IGES_IFLAG_TANGENT;
        return false;
    }

    if( 1 == np2 )
    {
        flags = f[0];
        aIntersectList.push_back( pt[0] );
        return true;
    }

    if( IGES_IFLAG_ENDPOINT == f[0] || IGES_IFLAG_ENDPOINT == f[1] )
        flags = IGES_IFLAG_ENDPOINT;

    // determine point order on the arc
    if( ang[0] < arcSAng )
        ang[0] += 2.0 * M_PI;

    if( ang[1] < arcSAng )
        ang[1] += 2.0 * M_PI;

    if( ang[1] > ang[0] )
    {
        aIntersectList.push_back( pt[0] );
        aIntersectList.push_back( pt[1] );
    }
    else
    {
        aIntersectList.push_back( pt[1] );
        aIntersectList.push_back( pt[0] );
    }

    return true;
}


// check case where both segments are lines
bool IGES_GEOM_SEGMENT::checkLines( const IGES_GEOM_SEGMENT& aSegment,
    std::list<IGES_POINT>& aIntersectList, IGES_INTERSECT_FLAG& flags )
{
    if( IGES_SEGTYPE_NONE == msegtype || IGES_SEGTYPE_NONE == aSegment.getSegType() )
    {
        ERRMSG << "\n + [ERROR] one of the segments has no data\n";
        return false;
    }

    flags = IGES_IFLAG_NONE;

    // Step 1: line segment parameterization:
    // X(t) = t(X2 - X1) + X1
    // Y(t) = t(Y2 - Y1) + Y1
    // For segment 1:
    // X(t1) = t1 * XA1 + XB1
    // Y(t1) = t1 * YA1 + YB1
    // Segment 2:
    // X(t2) = t2 * XA2 + XB2
    // Y(t2) = t2 * YA2 + YB2

    // Step 2: solving for X(t1) = X(t2):
    // t1 * XA1 + XB1 = t2 * XA2 + XB2
    // t1 = (t2 * XA2 + XB2 - XB1) / XA1
    // Step 3: solving for Y(y1) = Y(t2):
    // t1 * YA1 + YB1 = t2 * YA2 + YB2
    // t1 = (t2 * YA2 + YB2 - YB1) / YA1

    // Step 3: solving for t2:
    // t2 * XA2 * YA1 + YA1 * (XB2 - XB1) = t2 * YA2 * XA1 + XA1 * (YB2 - YB1)
    // => t2 * (XA2*YA1 - YA2*XA1) = XA1 * (YB2 - YB1) - YA1 * (XB2 - XB1)
    // => t2 = (XA1 * (YB2 - YB1) - YA1 * (XB2 - XB1)) / (XA2*YA1 - YA2*XA1)
    // NOTE: t2 has no solution if the lines are parallel or colinear
    //       When colinear, both numerator and denominator are 0
    //       When parallel, the numerator is non-zero and the denominator is 0

    double XA1 = mend.x - mstart.x;
    double YA1 = mend.y - mstart.y;
    IGES_POINT p0 = aSegment.getStart();
    IGES_POINT p1 = aSegment.getEnd();
    double XA2 = p1.x - p0.x;
    double YA2 = p1.y - p0.y;

    double XB1 = mstart.x;
    double YB1 = mstart.y;
    double XB2 = p0.x;
    double YB2 = p0.y;

    double num = (XA1 * (YB2 - YB1) - YA1 * (XB2 - XB1));
    double den = (XA2*YA1 - YA2*XA1);

    double t1;
    double t2;

    if( abs( den ) < 1e-6 )
    {
        // check if lines are parallel
        if( abs( num ) > 1e-6 )
            return false;

        // lines are colinear, but do they intersect (overlap along a segment)?
        // if there is an overlap the points must be in order 0 <= t1 < t2 <= 1
        // as parameterized on *this segment.

        // cases:
        // 1. (S0, E0) not in (S1, E1) *and* (S1, E1) not in (S0, E0): no overlap
        // 2. [S0 and E0 in (S1, E1)] *or* [S1 and E1 in (S0, E0)]: one segment inside the other or equal
        // 3. S0 or E0 in (S1, E1) *and* S1 or E1 in (S0, E1): partial overlap

        bool s0i = false;
        bool e0i = false;
        bool s1i = false;
        bool e1i = false;

        if( abs( XA2 ) >= abs( YA2 ) )
        {
            t1 = ( mstart.x - XB2 ) / XA2;
            t2 = ( mend.x - XB2 ) / XA2;
        }
        else
        {
            t1 = ( mstart.y - YB2 ) / YA2;
            t2 = ( mend.y - YB2 ) / YA2;
        }

        if( abs( t1 ) < 1e-8 || abs( t1 - 1.0 ) < 1e-8 )
            s0i = true;

        if( abs( t2 ) < 1e-8 || abs( t2 - 1.0 ) < 1e-8 )
            e0i = true;

        if( abs( XA1 ) >= abs( YA1 ) )
        {
            t1 = ( p0.x - XB1 ) / XA1;
            t2 = ( p1.x - XB1 ) / XA1;
        }
        else
        {
            t1 = ( p0.y - YB1 ) / YA1;
            t2 = ( p1.y - YB1 ) / YA1;
        }

        if( abs( t1 ) < 1e-8 || abs( t1 - 1.0 ) < 1e-8 )
            s1i = true;

        if( abs( t2 ) < 1e-8 || abs( t2 - 1.0 ) < 1e-8 )
            e1i = true;

        if( !s0i && !e0i && !s1i && !e1i )
            return false;

        flags = IGES_IFLAG_EDGE;

        if( s0i && e0i )
        {
            aIntersectList.push_back( mstart );
            aIntersectList.push_back( mend );
            return true;
        }

        if( s1i && e1i )
        {
            if( abs( XA1 ) >= abs( YA1 ) )
            {
                t1 = ( p0.x - XB1 ) / XA1;
                t2 = ( p1.x - XB1 ) / XA1;
            }
            else
            {
                t1 = ( p0.y - YB1 ) / YA1;
                t2 = ( p1.y - YB1 ) / YA1;
            }

            if( t1 < t2 )
            {
                aIntersectList.push_back( p0 );
                aIntersectList.push_back( p1 );
            }
            else
            {
                aIntersectList.push_back( p1 );
                aIntersectList.push_back( p0 );
            }

            return true;
        }

        if( s0i && s1i )
        {
            aIntersectList.push_back( mstart );
            aIntersectList.push_back( p0 );
            return true;
        }

        if( s0i && e1i )
        {
            aIntersectList.push_back( mstart );
            aIntersectList.push_back( p1 );
            return true;
        }

        // must be e0i

        if( s1i )
        {
            aIntersectList.push_back( p0 );
            aIntersectList.push_back( mend );
            return true;
        }

        // must be e0i && e1i
        aIntersectList.push_back( p1 );
        aIntersectList.push_back( mend );
        return true;
    }

    t2 = num / den;

    if( abs( XA1 ) < abs( YA1 ) )
        t1 = (t2 * YA2 + YB2 - YB1) / YA1;
    else
        t1 = (t2 * XA2 + XB2 - XB1) / XA1;

    if( t2 > -1e-8 && t2 < (1 + 1e-8)
        && t1 > -1e-8 && t1 < (1 + 1e-8) )
    {
        p0.x = t2 * XA2 + XB2;
        p0.y = t2 * YA2 + YB2;
        aIntersectList.push_back( p0 );

        if( abs( t1 ) < 1e-8 || abs( t1 - 1.0 ) < 1e-8
            || abs( t2 ) < 1e-8 || abs( t2 - 1.0 ) < 1e-8 )
            flags = IGES_IFLAG_ENDPOINT;

        return true;
    }

    return false;
}


// + calculate the top-left and bottom-right rectangular bounds
bool IGES_GEOM_SEGMENT::GetBoundingBox( IGES_POINT& p0, IGES_POINT& p1 )
{
    if( IGES_SEGTYPE_NONE == msegtype )
    {
        return false;
    }

    if( IGES_SEGTYPE_LINE == msegtype )
    {
        if( abs( mstart.x - mend.x ) < 1e-8 )
        {
            // we have a vertical line
            if( mstart.y > mend.y )
            {
                p0 = mstart;
                p1 = mend;
            }
            else
            {
                p1 = mstart;
                p0 = mend;
            }

            return true;
        }

        if( mstart.x < mend.x )
        {
            p0.x = mstart.x;
            p1.x = mend.x;
        }
        else
        {
            p1.x = mstart.x;
            p0.x = mend.x;
        }

        if( mstart.y >= mend.y )
        {
            p0.y = mstart.y;
            p1.y = mend.y;
        }
        else
        {
            p1.y = mstart.y;
            p0.y = mend.y;
        }

        return true;
    }   // if line segment

    if( IGES_SEGTYPE_CIRCLE == msegtype )
    {
        p0.x = mcenter.x - mradius;
        p1.x = mcenter.x + mradius;

        p0.y = mcenter.y + mradius;
        p1.y = mcenter.y - mradius;

        return true;
    }

    // bounds of an arc
    double aS = getStartAngle();
    double aE = getEndAngle();

    IGES_POINT m[4];    // x,y extrema of an arc
    int ne = 0;

    if( ( 0.0 >= aS && 0.0 <= aE ) || ( 2.0 * M_PI >= aS && 2.0 * M_PI <= aE ) )
    {
        m[0].x = mcenter.x + mradius;
        m[0].y = mcenter.y;
        ++ne;
    }

    if( ( M_PI * 0.5 >= aS && M_PI * 0.5 <= aE )
        || ( 2.5 * M_PI >= aS && 2.5 * M_PI <= aE ) )
    {
        m[ne].x = mcenter.x;
        m[ne].y = mcenter.y + mradius;
        ++ne;
    }

    if( M_PI >= aS && M_PI <= aE )
    {
        m[ne].x = mcenter.x;
        m[ne].y = mcenter.y + mradius;
        ++ne;
    }

    if( ( -0.5 * M_PI >= aS && -0.5 * M_PI <= aE )
        || ( 1.5 * M_PI >= aS && 1.5 * M_PI <= aE ) )
    {
        m[ne].x = mcenter.x;
        m[ne].y = mcenter.y + mradius;
        ++ne;
    }

    if( mstart.x < mend.x )
        p0.x = mstart.x;
    else
        p0.x = mend.x;

    if( mstart.y > mend.y )
        p0.y = mstart.y;
    else
        p0.y = mend.y;

    for( int i = 0; i < ne; ++i )
    {
        if( m[i].x < p0.x )
            p0.x = m[i].x;

        if( m[i].x > p1.x )
            p1.x = m[i].x;

        if( m[i].y > p0.y )
            p0.y = m[i].y;

        if( m[i].y < p1.y )
            p1.y = m[i].y;
    }

    return true;
}


IGES_SEGTYPE IGES_GEOM_SEGMENT::getSegType( void ) const
{
    return msegtype;
}


double IGES_GEOM_SEGMENT::getRadius( void ) const
{
    return mradius;
}


double IGES_GEOM_SEGMENT::getStartAngle( void ) const
{
    return msang;
}


double IGES_GEOM_SEGMENT::getEndAngle( void ) const
{
    return meang;
}


bool IGES_GEOM_SEGMENT::isArcCW( void ) const
{
    return mCWArc;
}


IGES_POINT IGES_GEOM_SEGMENT::getCenter( void ) const
{
    return mcenter;
}


IGES_POINT IGES_GEOM_SEGMENT::getStart( void ) const
{
    // ensure that the start/end points given
    // describe a CCW arc
    if( mCWArc )
        return mend;

    return mstart;
}


IGES_POINT IGES_GEOM_SEGMENT::getEnd( void ) const
{
    // ensure that the start/end points given
    // describe a CCW arc
    if( mCWArc )
        return mstart;

    return mend;
}

// reverse the point order if applicable
void IGES_GEOM_SEGMENT::reverse( void )
{
    if( IGES_SEGTYPE_NONE == msegtype || IGES_SEGTYPE_CIRCLE == msegtype )
        return;

    IGES_POINT tmp = mstart;
    mstart = mend;
    mend = tmp;

    if( IGES_SEGTYPE_ARC == msegtype )
    {
        if( mCWArc )
            mCWArc = false;
        else
            mCWArc = true;
    }

    return;
}

