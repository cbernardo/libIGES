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

static bool newArc100( IGES* aModel, IGES_ENTITY_100** ap )
{
    IGES_ENTITY* ep;

    if( !aModel->NewEntity( ENT_CIRCULAR_ARC, &ep ) )
        return false;

    *ap = dynamic_cast<IGES_ENTITY_100*>( ep );

    if( !(*ap) )
    {
        aModel->DelEntity( ep );
        return false;
    }

    return true;
}


static bool newTx124( IGES* aModel, IGES_ENTITY_124** tp )
{
    IGES_ENTITY* ep;

    if( !aModel->NewEntity( ENT_TRANSFORMATION_MATRIX, &ep ) )
        return false;

    *tp = dynamic_cast<IGES_ENTITY_124*>(ep);

    if( !(*tp) )
    {
        aModel->DelEntity( ep );
        return false;
    }

    return true;
}


static bool newArc126( IGES* aModel, IGES_ENTITY_126** ap )
{
    IGES_ENTITY* ep;

    if( !aModel->NewEntity( ENT_NURBS_CURVE, &ep ) )
        return false;

    *ap = dynamic_cast<IGES_ENTITY_126*>( ep );

    if( !(*ap) )
    {
        aModel->DelEntity( ep );
        return false;
    }

    return true;
}


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
        ERRMSG << "\n + [ERROR] radii differ by > 1e-8 (" << abs(r2 - mradius) << ")\n";
        cerr << " + mradius: " << mradius << "\n";
        cerr << " +      r2: " << r2 << "\n";
        init();
        return false;
    }

    msang = atan2( aStart.y - aCenter.y, aStart.x - aCenter.x );
    meang = atan2( aEnd.y - aCenter.y, aEnd.x - aCenter.x );

    // note: start/end angles are always according to CCW order
    if( isCW )
    {
        if( msang < meang )
            msang += 2.0 * M_PI;
    }
    else
    {
        if( meang < msang )
            meang += 2.0 * M_PI;
    }

    mcenter = aCenter;
    mstart = aStart;
    mend = aEnd;

    mstart = aStart;
    mend = aEnd;
    msegtype = IGES_SEGTYPE_ARC;
    mCWArc = isCW;

    return true;
}


double IGES_GEOM_SEGMENT::GetLength( void )
{
    switch( msegtype )
    {
        case IGES_SEGTYPE_CIRCLE:
            return 2.0 * M_PI * mradius;
            break;

        case IGES_SEGTYPE_ARC:
            return (getEndAngle() - getStartAngle()) * mradius;
            break;

        case IGES_SEGTYPE_LINE:
            do
            {
                double dx = mend.x - mstart.x;
                double dy = mend.y - mstart.y;
                return sqrt(dx*dx + dy*dy);
            } while( 0 );
            break;

        default:
            break;
    }

    return 0.0;
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
                               std::list<IGES_GEOM_SEGMENT*>& aNewSegmentList )
{
    if( IGES_SEGTYPE_NONE == msegtype )
    {
        ERRMSG << "\n [BUG]: splitting a non-defined segment\n";
        return false;
    }

    switch( aIntersectList.size() )
    {
        case 0:
            return false;
            break;

        case 1:
        case 2:
            break;

        default:
            ERRMSG << "\n [BUG]: invalid split list (contains more than 2 points)\n";
            return false;
            break;
    }

    if( IGES_SEGTYPE_CIRCLE != msegtype )
    {
        list<IGES_POINT>::iterator sP = aIntersectList.begin();
        list<IGES_POINT>::iterator eP = aIntersectList.end();

        while( sP != eP )
        {
            if( PointMatches( *sP, mstart, 1e-8 )
                || PointMatches( *sP, mend, 1e-8 ) )
            {
                sP = aIntersectList.erase( sP );
                continue;
            }

            ++sP;
        }

        if( aIntersectList.empty() )
            return false;
    }

    bool ok = false;

    switch( msegtype )
    {
        case IGES_SEGTYPE_LINE:
            ok = splitLine( aIntersectList, aNewSegmentList );
            break;

        case IGES_SEGTYPE_ARC:
            ok = splitArc( aIntersectList, aNewSegmentList );
            break;

        case IGES_SEGTYPE_CIRCLE:
            ok = splitCircle( aIntersectList, aNewSegmentList );
            break;

        default:
            ERRMSG << "\n [BUG]: bad segment type (" << msegtype << ")\n";
            return false;
            break;
    }

    if( !ok )
    {
        ERRMSG << "\n [INFO]: could not split segment\n";
        return false;
    }

    return true;
}


// retrieve the representation of the curve as IGES 2D primitives which
// are suitable as a subordinate to the CPTR of a Trimmed Parametric Surface.
// It is essential that a list of curves be returned as it is up to the
// user to decide whether these curves shall be part of a loop consiting
// of multiple segments.
bool IGES_GEOM_SEGMENT::GetCurves( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight )
{
    bool ok;

    switch( msegtype )
    {
        case IGES_SEGTYPE_LINE:
            ok = getCurveLine( aModel, aCurves, zHeight );
            break;

        case IGES_SEGTYPE_ARC:
            ok = getCurveArc( aModel, aCurves, zHeight );
            break;

        case IGES_SEGTYPE_CIRCLE:
            ok = getCurveCircle( aModel, aCurves, zHeight );
            break;

        default:
            ERRMSG << "\n + [ERROR] invalid segment type (" << msegtype << ")\n";
            ok = false;
            break;
    }

    return ok;
}


// retrieve the curve as a list of parametric curves on plane which arc suitable
// as subordinates to the BPTR of a Trimmed Parametric Surface. It is up to the
// user to determine whether these curves shall be part of a loop consisting of
// multiple segments.
bool IGES_GEOM_SEGMENT::GetCurveOnPlane(  IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                        double aMinX, double aMaxX, double aMinY, double aMaxY,
                        double zHeight )
{
    double scale = 1.0 / ( aMaxX - aMinX ); // scale factor (must be same for X and Y axes)
    bool ok;

    switch( msegtype )
    {
        case IGES_SEGTYPE_CIRCLE:
            ok = copCircle( aModel, aCurves, aMinX, aMinY, scale, zHeight );
            break;

        case IGES_SEGTYPE_ARC:
            ok = copArc( aModel, aCurves, aMinX, aMinY, scale, zHeight );
            break;

        case IGES_SEGTYPE_LINE:
            ok = copLine( aModel, aCurves, aMinX, aMinY, scale, zHeight );
            break;

        default:
            do
            {
                ERRMSG << "\n + [INFO] invalid segment type: " << msegtype << "\n";
                return false;
            } while( 0 );

            break;
    }

    if( !ok )
    {
        ERRMSG << "\n + [INFO] failure; see messages above\n";
        return false;
    }

    return true;
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

    // height of the triangle
    double h = sqrt( mradius*mradius - rd*rd );
    double ra0 = atan2( dy, dx ) + M_PI / 2.0;
    double ra1 = ra0 + M_PI;

    // first intersection point
    double x0 = x + h * cos(ra0);
    double y0 = y + h * sin(ra0);

    // second intersection point
    double x1 = x + h * cos(ra1);
    double y1 = y + h * sin(ra1);

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
    double d = sqrt( dx*dx + dy*dy );
    double a0 = getStartAngle();
    double a1 = getEndAngle();
    double b0 = aSegment.getStartAngle();
    double b1 = aSegment.getEndAngle();

#warning TO BE IMPLEMENTED
    // XXX - we may have a condition where we have 2 edge overlaps;
    // this violates our constraint of 0 or 2 point intersections
    // and we must inform the calling routine of the problem (invalid geometry)

    // check if no intersection possible
    if( (d > ( mradius + r2 )) || ((d + r2) < mradius) || ((d + mradius) < r2) )
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

        // XXX - TO BE IMPLEMENTED
        // Special case: arcs intersect at only a single endpoint

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


    if( IGES_SEGTYPE_ARC == msegtype && IGES_SEGTYPE_ARC == aSegment.getSegType() )
    {
        // special case: arcs intersect only at the endpoints
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
    }

    // the arcs are not concentric so if there is any intersection
    // it is at 1 or 2 points
    IGES_POINT p1;
    IGES_POINT p2;
    calcCircleIntercepts( c2, r2, d, p1, p2 );

    // determine if any of the points lie on *this arc and
    // place them in a ccw order

    double angX0 = atan2( p1.y - mcenter.y, p1.x - mcenter.x );
    double angX1 = atan2( p2.y - mcenter.y, p2.x - mcenter.x );
    IGES_POINT p0[2];
    double ang0[2];
    int np = 0;
    int isOnArc[2];
    double tang;

    isOnArc[0] = 0;
    isOnArc[1] = 0;

    if ( IGES_SEGTYPE_CIRCLE == aSegment.getSegType() )
    {
        ++isOnArc[0];
        ++isOnArc[1];
        cout << "XXX: *aSegment is a circle, ++isOnArc\n";
    }
    else
    {
        // check if each point lies on the arc
        tang = angX0;

        if( tang > b1 )
            tang -= 2.0 * M_PI;

        if( tang < b0 )
            tang += 2.0 * M_PI;

        if( tang <= b1 )
        {
            cout << "XXX: p0 is on arc *aSegment\n";
            ++isOnArc[0];
        }
        else
        {
            cout << "XXX: p0 not on *aSegment\n";
            cout << "XXX: a(" << a0 << ", " << a1 << "), aX0: " << angX0 << ", tang: " << tang << "\n";
        }

        tang = angX1;

        if( tang > b1 )
            tang -= 2.0 * M_PI;

        if( tang < b0 )
            tang += 2.0 * M_PI;

        if( tang <= b1 )
        {
            cout << "XXX: p1 is on arc *aSegment\n";
            ++isOnArc[1];
        }
        else
        {
            cout << "XXX: p1 not on *aSegment\n";
            cout << "XXX: a(" << a0 << ", " << a1 << "), aX1: " << angX1 << ", tang: " << tang << "\n";
        }
    }

    if( IGES_SEGTYPE_CIRCLE == msegtype )
    {
        cout << "XXX: *this is a circle, ++isOnArc\n";
        ++isOnArc[0];
        ++isOnArc[1];
    }
    else
    {
        // check if each point lies on the arc
        tang = angX0;

        if( tang > a1 )
            tang -= 2.0 * M_PI;

        if( tang < a0 )
            tang += 2.0 * M_PI;

        if( tang <= a1 )
        {
            cout << "XXX: p0 is on arc *this\n";
            ++isOnArc[0];
        }
        else
        {
            cout << "XXX: p0 not on *this\n";
            cout << "XXX: a(" << a0 << ", " << a1 << "), aX0: " << angX0 << ", tang: " << tang << "\n";
        }

        tang = angX1;

        if( tang > a1 )
            tang -= 2.0 * M_PI;

        if( tang < a0 )
            tang += 2.0 * M_PI;

        if( tang <= a1 )
        {
            cout << "XXX: p1 is on arc *this\n";
            ++isOnArc[1];
        }
        else
        {
            cout << "XXX: p1 not on *this\n";
            cout << "XXX: a(" << a0 << ", " << a1 << "), aX1: " << angX1 << ", tang: " << tang << "\n";
        }
    }

    if( angX0 < a0 )
        angX0 += 2.0 * M_PI;

    if( angX1 < a0 )
        angX1 += 2.0 * M_PI;

    if( 2 == isOnArc[0] )
    {
        cout << "XXX: adding p0\n";
        p0[0] = p1;
        ang0[0] = angX0;
        ++np;
    }

    if( 2 == isOnArc[1] )
    {
        cout << "XXX: adding p1\n";
        p0[np] = p2;
        ang0[np] = angX1;
        ++np;
    }

    cout << "XXX: np: " << np << "\n";

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
            double t = -B / ( 2.0 * A );

            if( t < 0.0 || t > 1.0 )
                return false;

            flags = IGES_IFLAG_TANGENT;
            IGES_POINT p;
            p.x = t * lS.x + (1.0 - t) * lE.x;
            p.y = t * lS.y + (1.0 - t) * lE.y;
            aIntersectList.push_back( p );
            return true;
        }

        tangent = true;
    }

    if( D < 0 )
        return false;

    double t0;
    double t1;

    if( tangent )
    {
        t0 = -B / ( 2.0 * A );
        t1 = t0;
    }
    else
    {
        t0 = ( -B + sqrt( D ) ) / ( 2.0 * A );
        t1 = ( -B - sqrt( D ) ) / ( 2.0 * A );
    }

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

    if( !tangent && t1 >= 0.0 && t1 <= 1.0 )
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

    if( 0 == np )
        return false;

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
        // note: the endpoints are checked as well to account for discrepancies
        // in the angle calculations which may result in false negatives.
        if( ( ang[i] >= arcSAng && ang[i] <= arcEAng )
            || ( ( ang[i] + 2.0 * M_PI ) >= arcSAng && ( ang[i] + 2.0 * M_PI ) <= arcEAng )
            || ( abs( p[0].x - mstart.x ) < 1e-14 && abs( p[0].y - mstart.y ) < 1e-14 )
            || ( abs( p[0].x - mend.x ) < 1e-14 && abs( p[0].y - mend.y ) < 1e-14 ) )
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
        aIntersectList.push_back( pt[0] );
        return true;
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


// calculate the bottom-left and top-right rectangular bounds
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
            if( mstart.y <= mend.y )
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

        if( mstart.y < mend.y )
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

        p0.y = mcenter.y - mradius;
        p1.y = mcenter.y + mradius;

        return true;
    }

    // bounds of an arc
    double aS = getStartAngle();
    double aE = getEndAngle();

    IGES_POINT m[4];    // x,y extrema of an arc
    int ne = 0;

    // check if start..end encompasses (0d)
    if( ( 0.0 >= aS && 0.0 <= aE ) || ( 2.0 * M_PI >= aS && 2.0 * M_PI <= aE ) )
    {
        m[0].x = mcenter.x + mradius;
        m[0].y = mcenter.y;
        ++ne;
    }

    // check if start..end encompasses (90d)
    if( ( M_PI * 0.5 >= aS && M_PI * 0.5 <= aE )
        || ( 2.5 * M_PI >= aS && 2.5 * M_PI <= aE ) )
    {
        m[ne].x = mcenter.x;
        m[ne].y = mcenter.y + mradius;
        ++ne;
    }

    // check if start..end encompasses (180d)
    if( M_PI >= aS && M_PI <= aE )
    {
        m[ne].x = mcenter.x - mradius;
        m[ne].y = mcenter.y;
        ++ne;
    }

    // check if start..end encompasses (270d)
    if( ( -0.5 * M_PI >= aS && -0.5 * M_PI <= aE )
        || ( 1.5 * M_PI >= aS && 1.5 * M_PI <= aE ) )
    {
        m[ne].x = mcenter.x;
        m[ne].y = mcenter.y - mradius;
        ++ne;
    }

    if( mstart.x < mend.x )
    {
        p0.x = mstart.x;
        p1.x = mend.x;
    }
    else
    {
        p0.x = mend.x;
        p1.x = mstart.x;
    }

    if( mstart.y < mend.y )
    {
        p0.y = mstart.y;
        p1.y = mend.y;
    }
    else
    {
        p0.y = mend.y;
        p1.y = mstart.y;
    }

    for( int i = 0; i < ne; ++i )
    {
        if( m[i].x < p0.x )
            p0.x = m[i].x;

        if( m[i].x > p1.x )
            p1.x = m[i].x;

        if( m[i].y < p0.y )
            p0.y = m[i].y;

        if( m[i].y > p1.y )
            p1.y = m[i].y;
    }

    return true;
}


// + calculate the midpoint along the segment and return true;
//   for circles the reported midpoint is the point to the right
//   of the center.
bool IGES_GEOM_SEGMENT::GetMidpoint( IGES_POINT& p0 )
{
    switch( msegtype )
    {
        case IGES_SEGTYPE_CIRCLE:
            p0 = mcenter;
            p0.x += mradius;
            break;

        case IGES_SEGTYPE_ARC:
            do
            {
                double ang = ( msang + meang ) * 0.5;
                p0 = mcenter;
                p0.x += mradius * cos( ang );
                p0.y += mradius * sin( ang );
            } while( 0 );

            break;

        case IGES_SEGTYPE_LINE:
            p0.x = 0.5 * (mend.x - mstart.x) + mstart.x;
            p0.y = 0.5 * (mend.y - mstart.y) + mstart.y;
            break;

        default:
            ERRMSG << "\n + [BUG] unhandled segment type: " << msegtype << "\n";
            return false;

            break;
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
    if( mCWArc )
        return meang;

    return msang;
}


double IGES_GEOM_SEGMENT::getEndAngle( void ) const
{
    if( mCWArc )
        return msang;

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

        double tang = msang;
        msang = meang;
        meang = tang;
    }

    return;
}

bool IGES_GEOM_SEGMENT::splitLine( std::list<IGES_POINT>& aIntersectList,
                std::list<IGES_GEOM_SEGMENT*>& aNewSegmentList )
{
    if( aIntersectList.empty() )
    {
        ERRMSG << "\n + [BUG] empty split list\n";
        return false;
    }

    if( aIntersectList.size() > 2 )
    {
        ERRMSG << "\n + [BUG] too many split points (" << aIntersectList.size();
        cerr << "), max is 2\n";
        return false;
    }

    // Line segment parameterization:
    // X(t) = t(X2 - X1) + X1
    // Y(t) = t(Y2 - Y1) + Y1
    //
    // Solving for t given X(t):
    // t = (X - X1)/(X2 - X1)
    //
    // Solving for t given Y(t):
    // t = (Y - Y1)/(Y2 - Y1)

    bool yp = false;    // true if we prefer Y parameterization
    double num;
    double den;

    if( abs(mend.x - mstart.x) < abs(mend.y - mstart.y) )
    {
        yp = true;
        num = -mstart.y;
        den = 1.0 / (mend.y - mstart.y);
    }
    else
    {
        num = -mstart.x;
        den = 1.0 / (mend.x - mstart.x);
    }

    list<IGES_POINT>::iterator sPL = aIntersectList.begin();
    list<IGES_POINT>::iterator ePL = aIntersectList.end();

    double tp[2];   // parameter points
    int np = 0;     // number of points to break at
    double t;       // parameter
    double u;       // complementary coordinate value

    while( sPL != ePL )
    {
        if( yp )
        {
            t = (num + sPL->y) * den;

            if( t < 0.0 || t > 1.0 )
            {
                ERRMSG << "\n + [ERROR] point " << np << " is not on the line (t: " << t << ")\n";
                cerr << "p" << np << "(" << sPL->x << ", " << sPL->y << ")\n";
                cerr << "line: (" << mstart.x << ", " << mstart.y << ")--(";
                cerr << mend.x << ", " << mend.y << ")\n";
                return false;
            }

            u = t*(mend.x - mstart.x) + mstart.x;

            if( abs(u - sPL->x) > 1e-8 )
            {
                ERRMSG << "\n + [ERROR] point " << np << " is not on the line\n";
                return false;
            }

            tp[np] = t;
            ++np;
        }
        else
        {
            t = (num + sPL->x) * den;

            if( t < 0.0 || t > 1.0 )
            {
                ERRMSG << "\n + [ERROR] point " << np << " is not on the line (t: " << t << ")\n";
                cerr << "p" << np << "(" << sPL->x << ", " << sPL->y << ")\n";
                cerr << "line: (" << mstart.x << ", " << mstart.y << ")--(";
                cerr << mend.x << ", " << mend.y << ")\n";
                return false;
            }

            u = t*(mend.y - mstart.y) + mstart.y;

            if( abs(u - sPL->y) > 1e-8 )
            {
                ERRMSG << "\n + [ERROR] point " << np << " is not on the line\n";
                return false;
            }

            tp[np] = t;
            ++np;
        }

        ++sPL;
    }

    IGES_GEOM_SEGMENT* sp = new IGES_GEOM_SEGMENT;
    IGES_POINT p0;

    if( 1 == np )
    {
        // set up the new segment
        p0 = aIntersectList.front();
        sp->SetParams(p0, mend);
        aNewSegmentList.push_back( sp );

        // adjust this segment
        mend = p0;
        return true;
    }

    IGES_POINT p1;

    if( tp[0] < tp[1] )
    {
        p0 = aIntersectList.front();
        p1 = aIntersectList.back();
    }
    else
    {
        p1 = aIntersectList.front();
        p0 = aIntersectList.back();
    }

    // set up the first segment
    sp->SetParams( p0, p1 );
    aNewSegmentList.push_back( sp );

    // create and set up the second segment
    sp = new IGES_GEOM_SEGMENT;
    sp->SetParams( p1, mend );
    aNewSegmentList.push_back( sp );

    // adjust this segment
    mend = p0;
    return true;
}


bool IGES_GEOM_SEGMENT::splitArc( std::list<IGES_POINT>& aIntersectList,
               std::list<IGES_GEOM_SEGMENT*>& aNewSegmentList )
{
    IGES_POINT p0 = aIntersectList.front();
    double dx = p0.x - mcenter.x;
    double dy = p0.y - mcenter.y;
    double dd = dx*dx + dy*dy;
    double dr = mradius * mradius;

    if( abs(dd - dr) > 1e-8 )
    {
        ERRMSG << "\n + [ERROR] radius of p0 varies by more than 1e-8 from arc's radius (" << (dd - dr) << ")\n";
        return false;
    }

    double a0 = atan2( dy, dx );

    if( a0 < getStartAngle() )
        a0 += 2.0 * M_PI;

    if( a0 > getEndAngle() )
    {
        ERRMSG << "\n + [ERROR] p0 is not on the arc\n";
        return false;
    }

    IGES_GEOM_SEGMENT* sp;

    if( 1 == aIntersectList.size() )
    {
        cout << "XXX: arc0[c(" << mcenter.x << ", " << mcenter.y;
        cout << "), s(" << mstart.x << ", " << mstart.y;
        cout << "), e(" << p0.x << ", " << p0.y;
        cout << "), a(" << msang << ", " << a0 << "), r = ";
        cout << mradius << ", CW: " << mCWArc << "]\n";

        cout << "XXX: arc1[c(" << mcenter.x << ", " << mcenter.y;
        cout << "), s(" << p0.x << ", " << p0.y;
        cout << "), e(" << mend.x << ", " << mend.y;
        cout << "), a(" << a0 << ", " << meang << "), r = ";
        cout << mradius << ", CW: " << mCWArc << "]\n";

        // create the new arc
        sp = new IGES_GEOM_SEGMENT;
        sp->mstart = p0;
        sp->mend = mend;
        sp->msang = a0;
        sp->meang = meang;
        sp->mCWArc = mCWArc;
        sp->msegtype = IGES_SEGTYPE_ARC;
        sp->mradius = mradius;
        sp->mcenter = mcenter;
        aNewSegmentList.push_back( sp );

        // adjust the extent of this arc
        mend = p0;
        meang = a0;

        return true;
    }

    IGES_POINT p1 = aIntersectList.back();

    dx = p1.x - mcenter.x;
    dy = p1.y - mcenter.y;
    dd = dx*dx + dy*dy;
    dr = mradius * mradius;

    if( abs(dd - dr) > 1e-8 )
    {
        ERRMSG << "\n + [ERROR] radius of p1 varies by more than 1e-8 from arc's radius\n";
        return false;
    }

    double a1 = atan2( dy, dx );

    if( a1 < getStartAngle() )
        a1 += 2.0 * M_PI;

    if( a1 > getEndAngle() )
    {
        ERRMSG << "\n + [ERROR] p1 is not on the arc\n";
        return false;
    }

    // swap points if necessary to ensure correct order on the arc
    if( mCWArc )
    {
        if( a1 > a0 )
        {
            double t0 = a0;
            a0 = a1;
            a1 = t0;
            IGES_POINT tp0 = p0;
            p0 = p1;
            p1 = tp0;
        }
    }
    else
    {
        if( a1 < a0 )
        {
            double t0 = a0;
            a0 = a1;
            a1 = t0;
            IGES_POINT tp0 = p0;
            p0 = p1;
            p1 = tp0;
        }
    }

    cout << "XXX: arc0[c(" << mcenter.x << ", " << mcenter.y;
    cout << "), s(" << mstart.x << ", " << mstart.y;
    cout << "), e(" << p0.x << ", " << p0.y;
    cout << "), a(" << msang << ", " << a0 << "), r = ";
    cout << mradius << ", CW: " << mCWArc << "]\n";

    cout << "XXX: arc1[c(" << mcenter.x << ", " << mcenter.y;
    cout << "), s(" << p0.x << ", " << p0.y;
    cout << "), e(" << p1.x << ", " << p1.y;
    cout << "), a(" << a0 << ", " << a1 << "), r = ";
    cout << mradius << ", CW: " << mCWArc << "]\n";

    cout << "XXX: arc2[c(" << mcenter.x << ", " << mcenter.y;
    cout << "), s(" << p1.x << ", " << p1.y;
    cout << "), e(" << mend.x << ", " << mend.y;
    cout << "), a(" << a1 << ", " << meang << "), r = ";
    cout << mradius << ", CW: " << mCWArc << "]\n";

    // create the first of 2 new arcs
    sp = new IGES_GEOM_SEGMENT;
    sp->mstart = p0;
    sp->mend = p1;
    sp->msang = a0;
    sp->meang = a1;
    sp->mCWArc = mCWArc;
    sp->msegtype = IGES_SEGTYPE_ARC;
    sp->mradius = mradius;
    sp->mcenter = mcenter;
    aNewSegmentList.push_back( sp );

    // create the second of 2 new arcs
    sp = new IGES_GEOM_SEGMENT;
    sp->mstart = p1;
    sp->mend = mend;
    sp->msang = a1;
    sp->meang = meang;
    sp->mCWArc = mCWArc;
    sp->msegtype = IGES_SEGTYPE_ARC;
    sp->mradius = mradius;
    sp->mcenter = mcenter;
    aNewSegmentList.push_back( sp );

    // adjust the extent of this arc
    mend = p0;
    meang = a0;

    return true;
}


bool IGES_GEOM_SEGMENT::splitCircle( std::list<IGES_POINT>& aIntersectList,
                  std::list<IGES_GEOM_SEGMENT*>& aNewSegmentList )
{
    if( aIntersectList.size() != 2 )
    {
        ERRMSG << "\n + [ERROR] splitting a circle requires 2 points, " << aIntersectList.size() << " provided\n";
        return false;
    }

    IGES_POINT p0 = aIntersectList.front();
    IGES_POINT p1 = aIntersectList.back();

    if( abs(p0.x - p1.x) < 1e-8 && abs(p0.y - p1.y) < 1e-8 )
    {
        ERRMSG << "\n + [ERROR] split points are identical (invalid geometry)\n";
        return false;
    }

    double dx = p0.x - mcenter.x;
    double dy = p0.y - mcenter.y;
    double dd = dx*dx + dy*dy;
    double dr = mradius * mradius;

    if( abs(dd - dr) > 1e-8 )
    {
        ERRMSG << "\n + [ERROR] radius of p0 varies by more than 1e-8 from circle's radius\n";
        return false;
    }

    double a0 = atan2( dy, dx );
    dx = p1.x - mcenter.x;
    dy = p1.y - mcenter.y;
    dd = dx*dx + dy*dy;

    if( abs(dd - dr) > 1e-8 )
    {
        ERRMSG << "\n + [ERROR] radius of p1 varies by more than 1e-8 from circle's radius\n";
        return false;
    }

    double a1 = atan2( dy, dx );

    IGES_GEOM_SEGMENT* sp = new IGES_GEOM_SEGMENT;

    if( !sp )
    {
        ERRMSG << "\n + [ERROR] could not allocate memory\n";
        return false;
    }

    // explicitly assign values for the new arc; this saves
    // some computation time over SetParams()
    sp->msegtype = IGES_SEGTYPE_ARC;
    sp->mcenter = mcenter;
    sp->mstart = p0;
    sp->mend = p1;
    sp->msang = a0;
    sp->meang = a1;
    sp->mradius = mradius;
    sp->mCWArc = false;

    if( sp->msang > sp->meang )
        sp->meang += 2.0 * M_PI;

    aNewSegmentList.push_back( sp );

    // The preserved section of the circle runs CCW from p1 to p0
    mstart = p1;
    mend = p0;
    msegtype = IGES_SEGTYPE_ARC;
    mCWArc = false;
    msang = a1;
    meang = a0;

    if( msang > meang )
        meang += 2.0 * M_PI;

    return true;
}


bool IGES_GEOM_SEGMENT::getCurveCircle( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight )
{
    IGES_ENTITY_100* arcs[2];
    arcs[0] = NULL;
    arcs[1] = NULL;

    for( int i = 0; i < 2; ++i )
    {
        if( !newArc100( aModel, &arcs[i] ) )
        {
            for( int j = 0; j < i; ++j )
                aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

            ERRMSG << "\n + [INFO] could not create circular arc #" << i << "\n";
            return false;
        }
    }

    arcs[0]->zOffset = zHeight;
    arcs[0]->xCenter = mcenter.x;
    arcs[0]->yCenter = mcenter.y;
    arcs[0]->xStart = mcenter.x + mradius;
    arcs[0]->yStart = mcenter.y;
    arcs[0]->xEnd = mcenter.x - mradius;
    arcs[0]->yEnd = mcenter.y;

    arcs[1]->zOffset = zHeight;
    arcs[1]->xCenter = mcenter.x;
    arcs[1]->yCenter = mcenter.y;
    arcs[1]->xStart = mcenter.x - mradius;
    arcs[1]->yStart = mcenter.y;
    arcs[1]->xEnd = mcenter.x + mradius;
    arcs[1]->yEnd = mcenter.y;

    for( int i = 0; i < 2; ++i )
    {
        IGES_CURVE* cp = dynamic_cast<IGES_CURVE*>( arcs[i] );

        if( !cp )
        {
            for( int j = i; j < 2; ++j )
                aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

            ERRMSG << "\n + [BUG] could not typecast arcs to IGES_CURVE\n";
            return false;
        }

        aCurves.push_back( cp );
    }

    return true;
}


bool IGES_GEOM_SEGMENT::getCurveArc( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight )
{
    // note: we must be sensitive to whether the arc is CW or CCW

    int na = 0; // number of arcs (and transforms if we have a CW arc)

    IGES_ENTITY_100* arcs[3];   // the arc may consist of up to 3 segments
    IGES_ENTITY_124* tx[3];     // for CW arcs each arc has a corresponding transform

    double a0 = getStartAngle();
    double a1 = getEndAngle();
    IGES_POINT p0 = getStart();
    IGES_POINT p1 = getEnd();

    if( a0 >= 0.0 && a0 < M_PI )
    {
        if( a1 <= M_PI )
        {
            if( !newArc100( aModel, &arcs[0] ) )
            {
                ERRMSG << "\n + [INFO] could not instantiate IGES arc\n";
                return false;
            }

            arcs[0]->zOffset = zHeight;
            arcs[0]->xCenter = mcenter.x;
            arcs[0]->yCenter = mcenter.y;
            arcs[0]->xStart = p0.x;
            arcs[0]->yStart = p0.y;
            arcs[0]->xEnd = p1.x;
            arcs[0]->yEnd = p1.y;
            na = 1;
        }
        else
        {
            if( a1 <= 2.0 * M_PI )
            {
                for( int i = 0; i < 2; ++i )
                {
                    if( !newArc100( aModel, &arcs[i] ) )
                    {
                        for( int j = 0; j < i; ++j )
                            aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

                        ERRMSG << "\n + [INFO] could not create circular arc #" << i << "\n";
                        return false;
                    }
                }

                arcs[0]->zOffset = zHeight;
                arcs[0]->xCenter = mcenter.x;
                arcs[0]->yCenter = mcenter.y;
                arcs[0]->xStart = p0.x;
                arcs[0]->yStart = p0.y;
                arcs[0]->xEnd = mcenter.x - mradius;
                arcs[0]->yEnd = mcenter.y;

                arcs[1]->zOffset = zHeight;
                arcs[1]->xCenter = mcenter.x;
                arcs[1]->yCenter = mcenter.y;
                arcs[1]->xStart = mcenter.x - mradius;
                arcs[1]->yStart = mcenter.y;
                arcs[1]->xEnd = p1.x;
                arcs[1]->yEnd = p1.y;

                na = 2;
            } // M_PI < a1 <= 2*M_PI
            else
            {
                for( int i = 0; i < 3; ++i )
                {
                    if( !newArc100( aModel, &arcs[i] ) )
                    {
                        for( int j = 0; j < i; ++j )
                            aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

                        ERRMSG << "\n + [INFO] could not create circular arc #" << i << "\n";
                        return false;
                    }
                }

                arcs[0]->zOffset = zHeight;
                arcs[0]->xCenter = mcenter.x;
                arcs[0]->yCenter = mcenter.y;
                arcs[0]->xStart = p0.x;
                arcs[0]->yStart = p0.y;
                arcs[0]->xEnd = mcenter.x - mradius;
                arcs[0]->yEnd = mcenter.y;

                arcs[1]->zOffset = zHeight;
                arcs[1]->xCenter = mcenter.x;
                arcs[1]->yCenter = mcenter.y;
                arcs[1]->xStart = mcenter.x - mradius;
                arcs[1]->yStart = mcenter.y;
                arcs[1]->xEnd = mcenter.x + mradius;
                arcs[1]->yEnd = mcenter.y;

                arcs[2]->zOffset = zHeight;
                arcs[2]->xCenter = mcenter.x;
                arcs[2]->yCenter = mcenter.y;
                arcs[2]->xStart = mcenter.x + mradius;
                arcs[2]->yStart = mcenter.y;
                arcs[2]->xEnd = p1.x;
                arcs[2]->yEnd = p1.y;

                na = 3;
            }   // a1 > 2*M_PI
        }   // ( 0.0 <= a0 < M_PI )

    }
    else if( a0 == M_PI )
    {
        if( a1 <= 2.0 * M_PI )
        {
            if( !newArc100( aModel, &arcs[0] ) )
            {
                ERRMSG << "\n + [INFO] could not instantiate IGES arc\n";
                return false;
            }

            arcs[0]->zOffset = zHeight;
            arcs[0]->xCenter = mcenter.x;
            arcs[0]->yCenter = mcenter.y;
            arcs[0]->xStart = p0.x;
            arcs[0]->yStart = p0.y;
            arcs[0]->xEnd = p1.x;
            arcs[0]->yEnd = p1.y;
            na = 1;
        }
        else
        {
            for( int i = 0; i < 2; ++i )
            {
                if( !newArc100( aModel, &arcs[i] ) )
                {
                    for( int j = 0; j < i; ++j )
                        aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

                    ERRMSG << "\n + [INFO] could not create circular arc #" << i << "\n";
                    return false;
                }
            }

            arcs[0]->zOffset = zHeight;
            arcs[0]->xCenter = mcenter.x;
            arcs[0]->yCenter = mcenter.y;
            arcs[0]->xStart = p0.x;
            arcs[0]->yStart = p0.y;
            arcs[0]->xEnd = mcenter.x + mradius;
            arcs[0]->yEnd = mcenter.y;

            arcs[1]->zOffset = zHeight;
            arcs[1]->xCenter = mcenter.x;
            arcs[1]->yCenter = mcenter.y;
            arcs[1]->xStart = mcenter.x + mradius;
            arcs[1]->yStart = mcenter.y;
            arcs[1]->xEnd = p1.x;
            arcs[1]->yEnd = p1.y;

            na = 2;
        }
    }   // a0 == M_PI
    else
    {
        if( a1 <= 0.0 )
        {
            if( !newArc100( aModel, &arcs[0] ) )
            {
                ERRMSG << "\n + [INFO] could not instantiate IGES arc\n";
                return false;
            }

            arcs[0]->zOffset = zHeight;
            arcs[0]->xCenter = mcenter.x;
            arcs[0]->yCenter = mcenter.y;
            arcs[0]->xStart = p0.x;
            arcs[0]->yStart = p0.y;
            arcs[0]->xEnd = p1.x;
            arcs[0]->yEnd = p1.y;
            na = 1;
        }
        else if( a1 <= M_PI )
        {
            for( int i = 0; i < 2; ++i )
            {
                if( !newArc100( aModel, &arcs[i] ) )
                {
                    for( int j = 0; j < i; ++j )
                        aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

                    ERRMSG << "\n + [INFO] could not create circular arc #" << i << "\n";
                    return false;
                }
            }

            arcs[0]->zOffset = zHeight;
            arcs[0]->xCenter = mcenter.x;
            arcs[0]->yCenter = mcenter.y;
            arcs[0]->xStart = p0.x;
            arcs[0]->yStart = p0.y;
            arcs[0]->xEnd = mcenter.x + mradius;
            arcs[0]->yEnd = mcenter.y;

            arcs[1]->zOffset = zHeight;
            arcs[1]->xCenter = mcenter.x;
            arcs[1]->yCenter = mcenter.y;
            arcs[1]->xStart = mcenter.x + mradius;
            arcs[1]->yStart = mcenter.y;
            arcs[1]->xEnd = p1.x;
            arcs[1]->yEnd = p1.y;

            na = 2;
        }   // a0 < 0 && a1 <= M_PI
        else
        {
            for( int i = 0; i < 3; ++i )
            {
                if( !newArc100( aModel, &arcs[i] ) )
                {
                    for( int j = 0; j < i; ++j )
                        aModel->DelEntity( (IGES_ENTITY*)arcs[j] );

                    ERRMSG << "\n + [INFO] could not create circular arc #" << i << "\n";
                    return false;
                }
            }

            arcs[0]->zOffset = zHeight;
            arcs[0]->xCenter = mcenter.x;
            arcs[0]->yCenter = mcenter.y;
            arcs[0]->xStart = p0.x;
            arcs[0]->yStart = p0.y;
            arcs[0]->xEnd = mcenter.x + mradius;
            arcs[0]->yEnd = mcenter.y;

            arcs[1]->zOffset = zHeight;
            arcs[1]->xCenter = mcenter.x;
            arcs[1]->yCenter = mcenter.y;
            arcs[1]->xStart = mcenter.x + mradius;
            arcs[1]->yStart = mcenter.y;
            arcs[1]->xEnd = mcenter.x - mradius;
            arcs[1]->yEnd = mcenter.y;

            arcs[2]->zOffset = zHeight;
            arcs[2]->xCenter = mcenter.x;
            arcs[2]->yCenter = mcenter.y;
            arcs[2]->xStart = mcenter.x - mradius;
            arcs[2]->yStart = mcenter.y;
            arcs[2]->xEnd = p1.x;
            arcs[2]->yEnd = p1.y;

            na = 3;
        }   // a0 < 0 && a1 > M_PI
    }   // a0 < 0.0

    if( mCWArc )
    {
        // instantiate the transforms
        for( int i = 0; i < na; ++i )
        {
            if( !newTx124( aModel, &tx[i] ) )
            {
                ERRMSG << "\n + [INFO] could not instantiate IGES transform matrix\n";
                return false;
            }

            tx[i]->T.T.x = arcs[i]->xCenter;
            tx[i]->T.T.z = 2.0 * zHeight;
            tx[i]->T.R.v[0][0] = -1.0;
            tx[i]->T.R.v[2][2] = -1.0;
            tx[i]->SetEntityForm( 1 );

            if( !arcs[i]->SetTransform( tx[i] ) )
            {
                ERRMSG << "\n + [INFO] could not set transform on arc\n";
                return false;
            }

            // note: since curves appear in CCW order we must swap the order
            for( int i = (na - 1); i > 0; --i )
                aCurves.push_back( (IGES_CURVE*)(arcs[i]) );
        }

        return true;
    }

    for( int i = 0; i < na; ++i )
        aCurves.push_back( (IGES_CURVE*)(arcs[i]) );

    return true;
}


bool IGES_GEOM_SEGMENT::getCurveLine( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight )
{
    IGES_ENTITY*     ep;
    IGES_ENTITY_110* lp;
    IGES_CURVE*      cp;

    if( !aModel->NewEntity( ENT_CIRCULAR_ARC, &ep ) )
    {
        ERRMSG << "\n + [INFO] could not instantiate IGES line\n";
        return false;
    }

    lp = dynamic_cast<IGES_ENTITY_110*>(ep);

    if( !lp )
    {
        aModel->DelEntity( ep );
        ERRMSG << "\n + [BUG] could not typecast IGES line\n";
        return false;
    }

    cp = dynamic_cast<IGES_CURVE*>(ep);

    if( !cp )
    {
        aModel->DelEntity( ep );
        ERRMSG << "\n + [BUG] could not typecast line to IGES_CURVE\n";
        return false;
    }

    aCurves.push_back( cp );

    return true;
}


bool IGES_GEOM_SEGMENT::copCircle( IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                                   double offX, double offY, double aScale,
                                   double zHeight )
{
    IGES_ENTITY_126* cp[2];

    for( int i = 0; i < 2; ++ i )
    {
        if( !newArc126( aModel, &cp[i] ) )
        {
            for( int j = 0; j < i; ++j )
                aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );

            ERRMSG << "\n + [INFO] could not instantiate IGES NURBS curve\n";
            return false;
        }
    }

    double axis[3] = { 0.0, 0.0, 1.0 }; // normal to the plane of the arc
    double startp[3];
    double centrp[3];
    centrp[0] = ( mcenter.x - offX ) * aScale;
    centrp[1] = ( mcenter.y - offY ) * aScale;
    centrp[2] = zHeight;
    SISLCurve* pCurve[2] = { NULL, NULL };
    int stat = 0;

    for( int i = 0; i < 2; ++i )
    {
        if( 0 == i )
        {
            startp[0] = centrp[0] + mradius * aScale;
            startp[1] = centrp[1];
            startp[2] = centrp[2];
        }
        else
        {
            startp[0] = centrp[0] - mradius * aScale;
            startp[1] = centrp[1];
            startp[2] = centrp[2];
        }

        s1303( startp, 1e-8, M_PI, centrp, axis, 3, &pCurve[i], &stat );

        switch( stat )
        {
            case 0:
                break;

            case 1:
                ERRMSG << "\n + [WARNING] unspecified problems creating NURBS arc\n";
                stat = 0;
                break;

            default:
                for( int j = 0; j < i; ++j )
                    freeCurve( pCurve[j] );

                for( int j = 0; j < 2; ++j )
                    aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );

                ERRMSG << "\n + [ERROR] could not create NURBS arc\n";
                return false;
                break;
        }
    }

    for( int i = 0; i < 2; ++i )
    {
        if( !cp[i]->SetNURBSData( pCurve[i]->in, pCurve[i]->ik, pCurve[i]->et,
            pCurve[i]->ecoef, false ) )
        {
            for( int j = 0; j < 2; ++j )
            {
                freeCurve( pCurve[j] );
                aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );
            }

            ERRMSG << "\n + [WARNING] problems setting data in NURBS arc\n";
            return false;
        }
    }

    for( int i = 0; i < 2; ++i )
    {
        freeCurve( pCurve[i] );
        aCurves.push_back( cp[i] );
    }

    return true;
}


bool IGES_GEOM_SEGMENT::copArc( IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                                double offX, double offY, double aScale,
                                double zHeight )
{
    IGES_ENTITY_126* cp[3]; // we may have up to 3 entities to describe a single arc
    IGES_ENTITY_124* tx[3]; // each arc segment must have a Transform if the arc is CW

    for( int i = 0; i < 3; ++i )
    {
        cp[i] = NULL;
        tx[i] = NULL;
    }

    double sAng;
    double eAng;
    IGES_POINT ptArc[2];    // start and end point of the arc; contingent on direction
    IGES_POINT ptC;         // true center point; required for calculations for CW arcs
    double angles[3];       // up to 3 subtended angles
    double spt[3][3];       // start points for each angle
    double cpt[3];          // center point for all angles

    if( mCWArc )
    {
        ptArc[0] = mstart;
        ptArc[1] = mend;

        ptArc[0].x = ( mcenter.x - ptArc[0].x ) * aScale;
        ptArc[0].y = ( ptArc[0].y - offY ) * aScale;
        ptArc[0].z = zHeight;

        ptArc[1].x = ( mcenter.x - ptArc[1].x ) * aScale;
        ptArc[1].y = ( ptArc[1].y - offY ) * aScale;
        ptArc[1].z = zHeight;

        cpt[0] = 0.0;
        cpt[1] = ( mcenter.y - offY ) * aScale;
        cpt[2] = zHeight;

        ptC = mcenter;
        ptC.x = ( ptC.x - offX ) * aScale;
        ptC.y = ( ptC.y - offY ) * aScale;
        ptC.z = zHeight;

        sAng = atan2( ptArc[0].y - cpt[1], ptArc[0].x );
        eAng = atan2( ptArc[1].y - cpt[1], ptArc[1].x );

        if( eAng < sAng )
            eAng += 2.0 * M_PI;
    }
    else
    {
        sAng = msang;
        eAng = meang;
        ptArc[0] = mstart;
        ptArc[1] = mend;

        ptArc[0].x = ( ( 2.0 * mcenter.x - ptArc[0].x ) - offX ) * aScale;
        ptArc[0].y = ( ptArc[0].y - offY ) * aScale;
        ptArc[0].z = zHeight;

        ptArc[1].x = ( ( 2.0 * mcenter.x - ptArc[1].x ) - offX ) * aScale;
        ptArc[1].y = ( ptArc[1].y - offY ) * aScale;
        ptArc[1].z = zHeight;

        if( sAng > M_PI )
        {
            sAng -= 2.0 * M_PI;
            eAng -= 2.0 * M_PI;
        }
    }

    int na = 0; // number of arcs in the curve
    spt[0][0] = ptArc[0].x;
    spt[0][1] = ptArc[0].y;
    spt[0][2] = ptArc[0].z;

    if( sAng < 0.0 )
    {
        // range of angles will be > -M_PI .. < 2*M_PI
        if( eAng <= 0.0 )
        {
            angles[0] = eAng - sAng;
            na = 1;
        }
        else
        {
            angles[0] = -sAng;

            spt[1][0] = cpt[0] + mradius * aScale;
            spt[1][1] = cpt[1];
            spt[1][2] = cpt[2];

            if( eAng <= M_PI )
            {
                angles[1] = eAng;
                na = 2;
            }
            else
            {
                angles[1] = M_PI;
                angles[2] = eAng - M_PI;
                spt[2][0] = cpt[0] - mradius * aScale;
                spt[2][1] = cpt[1];
                spt[2][2] = cpt[2];
                na = 3;
            }
        }
    }
    else
    {
        // range of angles will be >= 0 .. < 3*M_PI
        if( eAng <= M_PI || ( sAng >= M_PI && eAng <= 2.0 * M_PI ) )
        {
            angles[0] = eAng - sAng;
            na = 1;
        }
        else
        {
            if( sAng < M_PI )
            {
                // CASE: sAng < M_PI && eAng > M_PI
                angles[0] = M_PI - sAng;
                spt[1][0] = cpt[0] - mradius * aScale;
                spt[1][1] = cpt[1];
                spt[1][2] = cpt[2];

                if( eAng <= 2.0 * M_PI )
                {
                    angles[1] = eAng - M_PI;
                    na = 2;
                }
                else
                {
                    angles[1] = M_PI;
                    angles[2] = eAng - 2.0 * M_PI;
                    spt[1][0] = cpt[0] + mradius * aScale;
                    spt[1][1] = cpt[1];
                    spt[1][2] = cpt[2];
                    na = 3;
                }
            }
            else
            {
                // CASE: sAng == M_PI && eAng > 2.0 * M_PI
                if( eAng <= 2.0 * M_PI )
                {
                    angles[0] = eAng - sAng;
                    na = 1;
                }
                else
                {
                    angles[0] = 2.0 * M_PI - sAng;
                    angles[1] = eAng - 2.0 * M_PI;
                    na = 2;
                }
            }
        }
    }

    for( int i = 0; i < na; ++ i )
    {
        if( !newArc126( aModel, &cp[i] ) )
        {
            for( int j = 0; j < i; ++j )
            {
                aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );

                if( tx[j] )
                    aModel->DelEntity( (IGES_ENTITY*)(tx[j]) );

            }

            ERRMSG << "\n + [INFO] could not instantiate IGES NURBS curve\n";
            return false;
        }

        if( mCWArc )
        {
            if( !newTx124( aModel, &tx[i] ) )
            {
                for( int j = 0; j < i; ++j )
                {
                    aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );

                    if( tx[j] )
                        aModel->DelEntity( (IGES_ENTITY*)(tx[j]) );

                }

                aModel->DelEntity( (IGES_ENTITY*)(cp[i]) );
                ERRMSG << "\n + [INFO] could not instantiate IGES NURBS curve\n";
                return false;
            }
            else
            {
                tx[i]->T.T.x = ptC.x;
                tx[i]->T.T.z = 2.0 * zHeight;
                tx[i]->T.R.v[0][0] = -1.0;
                tx[i]->T.R.v[2][2] = -1.0;
                tx[i]->SetEntityForm( 1 );
                cp[i]->SetTransform( tx[i] );
            }
        }
    }

    double axis[3] = { 0.0, 0.0, 1.0 }; // normal to the plane of the arc
    double startp[3];
    SISLCurve* pCurve[3] = { NULL, NULL, NULL };
    int stat = 0;

    // set up the NURBS data
    for( int i = 0; i < na; ++i )
    {
        startp[0] = spt[i][0];
        startp[1] = spt[i][1];
        startp[2] = spt[i][2];

        s1303( startp, 1e-8, angles[i], cpt, axis, 3, &pCurve[i], &stat );

        switch( stat )
        {
            case 0:
                break;

            case 1:
                ERRMSG << "\n + [WARNING] unspecified problems creating NURBS arc\n";
                stat = 0;
                break;

            default:
                for( int j = 0; j < i; ++j )
                    freeCurve( pCurve[j] );

                for( int j = 0; j < na; ++j )
                {
                    aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );

                    if( tx[j] )
                        aModel->DelEntity( (IGES_ENTITY*)(tx[j]) );

                }

                ERRMSG << "\n + [ERROR] could not create NURBS arc\n";
                return false;
                break;
        }
    }

    for( int i = 0; i < na; ++i )
    {
        if( !cp[i]->SetNURBSData( pCurve[i]->in, pCurve[i]->ik, pCurve[i]->et,
            pCurve[i]->ecoef, false ) )
        {
            for( int j = 0; j < na; ++j )
            {
                if( pCurve[j] )
                    freeCurve( pCurve[j] );

                aModel->DelEntity( (IGES_ENTITY*)(cp[j]) );

                if( tx[j] )
                    aModel->DelEntity( (IGES_ENTITY*)(tx[j]) );

            }

            ERRMSG << "\n + [WARNING] problems setting data in NURBS arc\n";
            return false;
        }
    }

    for( int i = 0; i < na; ++i )
    {
        freeCurve( pCurve[i] );
        aCurves.push_back( cp[i] );
    }

    return true;
}


bool IGES_GEOM_SEGMENT::copLine( IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                                 double offX, double offY, double aScale,
                                 double zHeight )
{
    IGES_ENTITY_126* cp;

    if( !newArc126( aModel, &cp ) )
    {
        ERRMSG << "\n + [INFO] could not instantiate IGES NURBS curve\n";
        return false;
    }

    double startp[3];
    double endp[3];
    SISLCurve* pCurve = NULL;

    startp[0] = (mstart.x - offX) * aScale;
    startp[1] = (mstart.y - offY) * aScale;
    startp[2] = zHeight;
    endp[0] = (mend.x - offX) * aScale;
    endp[1] = (mend.y - offY) * aScale;
    endp[2] = zHeight;

    double epar = 1e-8;
    int stat = 0;
    s1602( startp, endp, 2, 3, 0.0, &epar, &pCurve, &stat );

    switch( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified problems creating NURBS curve\n";
            stat = 0;
            break;

        default:
            ERRMSG << "\n + [ERROR] could not create NURBS curve\n";
            return false;
            break;
    }

    if( !cp->SetNURBSData( pCurve->in, pCurve->ik, pCurve->et, pCurve->ecoef, false ) )
    {
        ERRMSG << "\n + [WARNING] problems setting data in NURBS curve\n";
        aModel->DelEntity( (IGES_ENTITY*)cp );
        freeCurve( pCurve );
        return false;
    }

    freeCurve( pCurve );
    aCurves.push_back( cp );
    return true;
}
