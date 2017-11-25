/*
 * file: mcad_segment.cpp
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

#include <cmath>
#include <error_macros.h>
#include <geom/mcad_segment.h>
#include <geom/mcad_helpers.h>

using namespace std;


// ensure the start angle a0 is in the range -M_PI < a0 <= +M_PI
// a0, a1 = start, end angle (must ensure CCW order)
static inline void NORMALIZE_ANGLES( double &a0, double &a1 )
{
    while( a0 > M_PI && a1 > M_PI)
    {
        a0 -= 2.0 * M_PI;
        a1 -= 2.0 * M_PI;
    }

    while( a0 <= -M_PI && a1 <= -M_PI )
    {
        a0 += 2.0 * M_PI;
        a1 += 2.0 * M_PI;
    }
}


MCAD_SEGMENT::MCAD_SEGMENT()
{
    init();
}


MCAD_SEGMENT::~MCAD_SEGMENT()
{
    list< bool* >::iterator sVF = m_validFlags.begin();
    list< bool* >::iterator eVF = m_validFlags.end();

    while( sVF != eVF )
    {
        **sVF = false;
        ++sVF;
    }

    return;
}


void MCAD_SEGMENT::init( void )
{
    msegtype = MCAD_SEGTYPE_NONE;
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


void MCAD_SEGMENT::AttachValidFlag( bool* aFlag )
{
    if( NULL == aFlag )
        return;

    list< bool* >::iterator sVF = m_validFlags.begin();
    list< bool* >::iterator eVF = m_validFlags.end();

    while( sVF != eVF )
    {
        if( *sVF == aFlag )
        {
            // exit if we already have this registered
            *aFlag = true;
            return;
        }

        ++sVF;
    }

    *aFlag = true;
    m_validFlags.push_back( aFlag );
    return;
}


void MCAD_SEGMENT::DetachValidFlag( bool* aFlag )
{
    if( NULL == aFlag )
        return;

    list< bool* >::iterator sVF = m_validFlags.begin();
    list< bool* >::iterator eVF = m_validFlags.end();

    while( sVF != eVF )
    {
        if( *sVF == aFlag )
        {
            *aFlag = false;
            m_validFlags.erase( sVF );
            return;
        }

        ++sVF;
    }

    return;
}


// set the parameters for a line
bool MCAD_SEGMENT::SetParams( MCAD_POINT aStart, MCAD_POINT aEnd )
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
    msegtype = MCAD_SEGTYPE_LINE;
    return true;
}


// set the parameters for an arc; the parameters must be specified such that
// the arc is traced in a counterclockwise direction as viewed from a positive
// Z location.
bool MCAD_SEGMENT::SetParams( MCAD_POINT aCenter, MCAD_POINT aStart,
                              MCAD_POINT aEnd, bool isCW )
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
        msegtype = MCAD_SEGTYPE_CIRCLE;
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
    msegtype = MCAD_SEGTYPE_ARC;
    mCWArc = isCW;

    return true;
}


double MCAD_SEGMENT::GetLength( void )
{
    switch( msegtype )
    {
        case MCAD_SEGTYPE_CIRCLE:
            return 2.0 * M_PI * mradius;
            break;

        case MCAD_SEGTYPE_ARC:
            return (GetEndAngle() - GetStartAngle()) * mradius;
            break;

        case MCAD_SEGTYPE_LINE:
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
bool MCAD_SEGMENT::GetIntersections( const MCAD_SEGMENT& aSegment,
                                          std::list<MCAD_POINT>& aIntersectList,
                                          MCAD_INTERSECT_FLAG& flags )
{
    flags = MCAD_IFLAG_NONE;

    if( MCAD_SEGTYPE_NONE == msegtype )
    {
        ERRMSG << "\n + [ERROR] no data in segment\n";
        return false;
    }

    // cases to check for:
    // a. circles are identical (bad geometry, return MCAD_IFLAG_IDENT)
    // b. *this is inside aSegment and both entities are circles
    //    (bad geometry, return MCAD_IFLAG_INSIDE)
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
    MCAD_SEGTYPE oSegType = aSegment.GetSegType();

    if( MCAD_SEGTYPE_NONE == oSegType )
    {
        ERRMSG << "\n + [ERROR] no data in second segment\n";
        return false;
    }

    // *this is a circle and it may intersect with a circle, arc, or line
    if( MCAD_SEGTYPE_CIRCLE == msegtype )
    {
        if( MCAD_SEGTYPE_CIRCLE == oSegType )
            return checkCircles( aSegment, aIntersectList, flags );

        if( MCAD_SEGTYPE_ARC == oSegType )
            return checkArcs(  aSegment, aIntersectList, flags );

        return checkArcLine(  aSegment, aIntersectList, flags );
    }

    // *this is an arc and it may intersect with a line, arc, or circle
    if( MCAD_SEGTYPE_ARC == msegtype )
    {
        if( MCAD_SEGTYPE_LINE == oSegType )
            return checkArcLine(  aSegment, aIntersectList, flags );

        return checkArcs(  aSegment, aIntersectList, flags );
    }

    // *this is a line and it may intersect with a line, arc or circle
    if( MCAD_SEGTYPE_LINE == oSegType )
        return checkLines(  aSegment, aIntersectList, flags );

    return checkArcLine( aSegment, aIntersectList, flags );
}


// split at the given list of intersections (1 or 2 intersections only)
bool MCAD_SEGMENT::Split( std::list<MCAD_POINT>& aIntersectList,
                          std::list<MCAD_SEGMENT*>& aNewSegmentList )
{
    if( MCAD_SEGTYPE_NONE == msegtype )
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

    if( MCAD_SEGTYPE_CIRCLE != msegtype )
    {
        list<MCAD_POINT>::iterator sP = aIntersectList.begin();
        list<MCAD_POINT>::iterator eP = aIntersectList.end();

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
        case MCAD_SEGTYPE_LINE:
            ok = splitLine( aIntersectList, aNewSegmentList );
            break;

        case MCAD_SEGTYPE_ARC:
            ok = splitArc( aIntersectList, aNewSegmentList );
            break;

        case MCAD_SEGTYPE_CIRCLE:
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


void MCAD_SEGMENT::calcCircleIntercepts( MCAD_POINT c2, double r2, double d,
    MCAD_POINT& p1, MCAD_POINT& p2 )
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
bool MCAD_SEGMENT::checkCircles( const MCAD_SEGMENT& aSegment,
                                 std::list<MCAD_POINT>& aIntersectList,
                                 MCAD_INTERSECT_FLAG& flags )
{
    MCAD_POINT c2 = aSegment.GetCenter();
    double r2 = aSegment.GetRadius();
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
        flags = MCAD_IFLAG_IDENT;
        return false;
    }

    if( abs( d - mradius - r2 ) < 0.001 )
    {
        flags = MCAD_IFLAG_TANGENT;
        return false;
    }

    if( d < mradius || d < r2)
    {
        // check if aSegment is inside this circle
        if( d <= (mradius - r2) )
        {
            flags = MCAD_IFLAG_ENCIRCLES;
            return false;
        }

        // check if this circle is inside aSegment
        if( d <= (r2 - mradius) )
        {
            flags = MCAD_IFLAG_INSIDE;
            return false;
        }
    }

    // there must be 2 intersection points
    MCAD_POINT p1;
    MCAD_POINT p2;
    calcCircleIntercepts( c2, r2, d, p1, p2 );
    aIntersectList.push_back( p1 );
    aIntersectList.push_back( p2 );

    return true;
}


// check case where both segments are arcs (one may be a circle)
bool MCAD_SEGMENT::checkArcs( const MCAD_SEGMENT& aSegment,
                              std::list<MCAD_POINT>& aIntersectList,
                              MCAD_INTERSECT_FLAG& flags )
{
    MCAD_POINT c2 = aSegment.GetCenter();
    double r2 = aSegment.GetRadius();
    double dx = mcenter.x - c2.x;
    double dy = mcenter.y - c2.y;
    double d = sqrt( dx*dx + dy*dy );
    double a0 = GetStartAngle();
    double a1 = GetEndAngle();
    double b0 = aSegment.GetStartAngle();
    double b1 = aSegment.GetEndAngle();

    // XXX -
    // WARNING: TO BE IMPLEMENTED
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
            flags = MCAD_IFLAG_TANGENT;

        return false;
    }

    // check if the arcs are identical
    if ( PointMatches( mcenter, c2, 1e-3 )
        && abs( mradius - r2 ) < 1e-3 )
    {
        // there may be an intersection along an edge
        if( MCAD_SEGTYPE_CIRCLE == msegtype )
        {
            aIntersectList.push_back( aSegment.GetStart() );
            aIntersectList.push_back( aSegment.GetEnd() );
            flags = MCAD_IFLAG_EDGE;
            return true;
        }

        if( MCAD_SEGTYPE_CIRCLE == aSegment.GetSegType() )
        {
            aIntersectList.push_back( GetStart() );
            aIntersectList.push_back( GetEnd() );
            flags = MCAD_IFLAG_EDGE;
            return true;
        }

        // Special case: arcs only intersect at their endpoints;
        // we must return the endpoints with the flag MCAD_IFLAG_ENDPOINT
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
            aIntersectList.push_back( GetStart() );
            aIntersectList.push_back( GetEnd() );
            flags = MCAD_IFLAG_ENDPOINT;
            return true;
        }

        // XXX - TO BE IMPLEMENTED
        // Special case: arcs intersect at only a single endpoint

        // determine if an entire segment is enveloped
        if( (b0 >= a0 && b1 <= a1)
            || ((b0 + 2.0*M_PI) >= a0 && (b1 + 2.0*M_PI) <= a1)
            || ((b0 - 2.0*M_PI) >= a0 && (b1 - 2.0*M_PI) <= a1) )
        {
            aIntersectList.push_back( aSegment.GetStart() );
            aIntersectList.push_back( aSegment.GetEnd() );
            flags = MCAD_IFLAG_EDGE;
            return true;
        }

        if( (a0 >= b0 && a1 <= b1)
            || (a0 >= (b0 + 2.0*M_PI) && a1 <= (b1 + 2.0*M_PI))
            || (a0 >= (b0 - 2.0*M_PI) && a1 <= (b1 - 2.0*M_PI)) )
        {
            aIntersectList.push_back( GetStart() );
            aIntersectList.push_back( GetEnd() );
            flags = MCAD_IFLAG_EDGE;
            return true;
        }

        // case 1: aSegment encompasses the start of *this
        if( (b0 <= a0 && b1 >= a0 && b1 <= a1)
            || ((b0 - 2.0*M_PI) <= a0 && (b1 - 2.0*M_PI) >= a0 && (b1 - 2.0*M_PI) <= a1) )
        {
            aIntersectList.push_back( GetStart() );
            aIntersectList.push_back( aSegment.GetEnd() );
            flags = MCAD_IFLAG_EDGE;
            return true;
        }

        // case 2: aSegment encompasses the end of *this
        if( (b0 >= a0 && b0 <= a1 && b1 >= a1)
            || ((b0 + 2.0*M_PI) >= a0 && (b0 + 2.0*M_PI) <= a1 && (b1 + 2.0*M_PI) >= a1)
            || ((b0 - 2.0*M_PI) >= a0 && (b0 - 2.0*M_PI) <= a1 && (b1 - 2.0*M_PI) >= a1) )
        {
            aIntersectList.push_back( GetStart() );
            aIntersectList.push_back( aSegment.GetEnd() );
            flags = MCAD_IFLAG_EDGE;
            return true;
        }

        return false;
    }

    // Special cases: arcs only intersect at their endpoints
    // 1. aSegment->radius < *this->radius : return flag MCAD_IFLAG_OUTSIDE; this
    //    return value can be used by the calling routine to determine whether
    //    nothing should be done (subtracting) or *this arc should be replaced
    //    by aSegment (adding) provided aSegment has no non-endpoint intersections
    //    with all other members of the outline.
    // 2. aSegment->radius > *this->radius and a point halfway along *this
    //    arc is inside aSegment : return flag MCAD_IFLAG_INSIDE. For a subtractive
    //    case, the calling routine can take the flag value to indicate that *this
    //    arc must be replaced by aSegment provided there are absolutely no other
    //    intersections in the outline which are not endpoints. For an additive case
    //    nothing is to be done
    // 3. aSegment->radius > *this->radius and a point halfway along *this
    //    arc is outside aSegment : return flag MCAD_IFLAG_OUTSIDE


    if( MCAD_SEGTYPE_ARC == msegtype && MCAD_SEGTYPE_ARC == aSegment.GetSegType() )
    {
        // special case: arcs intersect only at the endpoints
        if( ( PointMatches( GetStart(), aSegment.GetStart(), 1e-3 )
            && PointMatches( GetEnd(), aSegment.GetEnd(), 1e-3 ) )
            || (PointMatches( GetStart(), aSegment.GetEnd(), 1e-3 )
            && PointMatches( GetEnd(), aSegment.GetStart(), 1e-3 ) ) )
        {
            aIntersectList.push_back( GetStart() );
            aIntersectList.push_back( GetEnd() );

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
                    flags = MCAD_IFLAG_INSIDE;
                    return true;
                }
            }

            flags = MCAD_IFLAG_OUTSIDE;
            return true;
        }
    }

    // the arcs are not concentric so if there is any intersection
    // it is at 1 or 2 points
    MCAD_POINT p1;
    MCAD_POINT p2;
    calcCircleIntercepts( c2, r2, d, p1, p2 );

    // determine if any of the points lie on *this arc and
    // place them in a ccw order

    double angX0 = atan2( p1.y - mcenter.y, p1.x - mcenter.x );
    double angX1 = atan2( p2.y - mcenter.y, p2.x - mcenter.x );
    MCAD_POINT p0[2];
    double ang0[2];
    int np = 0;
    int isOnArc[2];
    double tang;

    isOnArc[0] = 0;
    isOnArc[1] = 0;

    if ( MCAD_SEGTYPE_CIRCLE == aSegment.GetSegType() )
    {
        ++isOnArc[0];
        ++isOnArc[1];
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
            ++isOnArc[0];
        }

        tang = angX1;

        if( tang > b1 )
            tang -= 2.0 * M_PI;

        if( tang < b0 )
            tang += 2.0 * M_PI;

        if( tang <= b1 )
        {
            ++isOnArc[1];
        }
    }

    if( MCAD_SEGTYPE_CIRCLE == msegtype )
    {
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
            ++isOnArc[0];
        }

        tang = angX1;

        if( tang > a1 )
            tang -= 2.0 * M_PI;

        if( tang < a0 )
            tang += 2.0 * M_PI;

        if( tang <= a1 )
        {
            ++isOnArc[1];
        }
    }

    if( angX0 < a0 )
        angX0 += 2.0 * M_PI;

    if( angX1 < a0 )
        angX1 += 2.0 * M_PI;

    if( 2 == isOnArc[0] )
    {
        p0[0] = p1;
        ang0[0] = angX0;
        ++np;
    }

    if( 2 == isOnArc[1] )
    {
        p0[np] = p2;
        ang0[np] = angX1;
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
bool MCAD_SEGMENT::checkArcLine( const MCAD_SEGMENT& aSegment,
                                 std::list<MCAD_POINT>& aIntersectList,
                                 MCAD_INTERSECT_FLAG& flags )
{
    flags = MCAD_IFLAG_NONE;

    // retrieve parameters of the arc and line segment
    MCAD_POINT arcC;
    MCAD_POINT arcS;
    MCAD_POINT arcE;
    double arcR;
    double arcSAng;
    double arcEAng;
    bool arcCircle = false;

    MCAD_POINT lS;
    MCAD_POINT lE;

    if( MCAD_SEGTYPE_ARC == msegtype || MCAD_SEGTYPE_CIRCLE == msegtype )
    {
        if( MCAD_SEGTYPE_CIRCLE == msegtype )
            arcCircle = true;

        arcSAng = GetStartAngle();
        arcEAng = GetEndAngle();
        arcR = mradius;
        arcC = mcenter;
        arcS = mstart;
        arcE = mend;

        lS = aSegment.GetStart();
        lE = aSegment.GetEnd();
    }
    else
    {
        if( MCAD_SEGTYPE_CIRCLE == aSegment.GetSegType() )
            arcCircle = true;

        arcSAng = aSegment.GetStartAngle();
        arcEAng = aSegment.GetEndAngle();
        arcR = aSegment.GetRadius();
        arcC = aSegment.GetCenter();
        arcS = aSegment.GetStart();
        arcE = aSegment.GetEnd();

        lS = GetStart();
        lE = GetEnd();
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

            flags = MCAD_IFLAG_TANGENT;
            MCAD_POINT p;
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
    MCAD_POINT p[2];
    MCAD_INTERSECT_FLAG f[2];

    if( t0 >= 0.0 && t0 <= 1.0 )
    {
        p[0].x = t0 * lS.x + (1.0 - t0) * lE.x;
        p[0].y = t0 * lS.y + (1.0 - t0) * lE.y;

        if( ( ( PointMatches( p[0], arcS, 1e-8 ) || PointMatches( p[0], arcE, 1e-8 ) )
            && !arcCircle )
            || abs( t0 ) < 1e-8 || abs( t0 - 1.0 ) < 1e-8 )
            f[0] = MCAD_IFLAG_ENDPOINT;
        else
            f[0] = MCAD_IFLAG_NONE;

        ++np;
    }

    if( !tangent && t1 >= 0.0 && t1 <= 1.0 )
    {
        p[np].x = t1 * lS.x + (1.0 - t1) * lE.x;
        p[np].y = t1 * lS.y + (1.0 - t1) * lE.y;

        if( ( ( PointMatches( p[np], arcS, 1e-8 ) || PointMatches( p[np], arcE, 1e-8 ) )
            && !arcCircle )
            || abs( t1 ) < 1e-8 || abs( t1 - 1.0 ) < 1e-8 )
            f[np] = MCAD_IFLAG_ENDPOINT;
        else
            f[np] = MCAD_IFLAG_NONE;

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

        if( MCAD_IFLAG_ENDPOINT == f[0] || MCAD_IFLAG_ENDPOINT == f[1] )
            flags = MCAD_IFLAG_ENDPOINT;

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

    MCAD_POINT pt[2];
    int np2 = 0;

    // check if each point is on the arc
    for( int i = 0; i < np; ++i )
    {
        // note: the endpoints are checked as well to account for discrepancies
        // in the angle calculations which may result in false negatives.
        if( ( ang[i] >= arcSAng && ang[i] <= arcEAng )
            || ( ( ang[i] + 2.0 * M_PI ) >= arcSAng && ( ang[i] + 2.0 * M_PI ) <= arcEAng )
            || ( abs( p[i].x - mstart.x ) < 1e-14 && abs( p[i].y - mstart.y ) < 1e-14 )
            || ( abs( p[i].x - mend.x ) < 1e-14 && abs( p[i].y - mend.y ) < 1e-14 ) )
        {
            pt[np2] = p[i];
            ++np2;
        }
    }

    if( 0 == np2 )
        return false;

    if( tangent )
    {
        flags = MCAD_IFLAG_TANGENT;
        aIntersectList.push_back( pt[0] );
        return true;
    }

    if( 1 == np2 )
    {
        flags = f[0];
        aIntersectList.push_back( pt[0] );
        return true;
    }

    if( MCAD_IFLAG_ENDPOINT == f[0] || MCAD_IFLAG_ENDPOINT == f[1] )
        flags = MCAD_IFLAG_ENDPOINT;

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
bool MCAD_SEGMENT::checkLines( const MCAD_SEGMENT& aSegment,
                               std::list<MCAD_POINT>& aIntersectList,
                               MCAD_INTERSECT_FLAG& flags )
{
    if( MCAD_SEGTYPE_NONE == msegtype || MCAD_SEGTYPE_NONE == aSegment.GetSegType() )
    {
        ERRMSG << "\n + [ERROR] one of the segments has no data\n";
        return false;
    }

    flags = MCAD_IFLAG_NONE;

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
    MCAD_POINT p0 = aSegment.GetStart();
    MCAD_POINT p1 = aSegment.GetEnd();
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

        flags = MCAD_IFLAG_EDGE;

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
            flags = MCAD_IFLAG_ENDPOINT;

        return true;
    }

    return false;
}


// calculate the bottom-left and top-right rectangular bounds
bool MCAD_SEGMENT::GetBoundingBox( MCAD_POINT& p0, MCAD_POINT& p1 )
{
    if( MCAD_SEGTYPE_NONE == msegtype )
    {
        return false;
    }

    if( MCAD_SEGTYPE_LINE == msegtype )
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

    if( MCAD_SEGTYPE_CIRCLE == msegtype )
    {
        p0.x = mcenter.x - mradius;
        p1.x = mcenter.x + mradius;

        p0.y = mcenter.y - mradius;
        p1.y = mcenter.y + mradius;

        return true;
    }

    // bounds of an arc
    double aS = GetStartAngle();
    double aE = GetEndAngle();

    MCAD_POINT m[4];    // x,y extrema of an arc
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
bool MCAD_SEGMENT::GetMidpoint( MCAD_POINT& p0 )
{
    switch( msegtype )
    {
        case MCAD_SEGTYPE_CIRCLE:
            p0 = mcenter;
            p0.x += mradius;
            break;

        case MCAD_SEGTYPE_ARC:
            do
            {
                double ang = ( msang + meang ) * 0.5;
                p0 = mcenter;
                p0.x += mradius * cos( ang );
                p0.y += mradius * sin( ang );
            } while( 0 );

            break;

        case MCAD_SEGTYPE_LINE:
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


MCAD_SEGTYPE MCAD_SEGMENT::GetSegType( void ) const
{
    return msegtype;
}


double MCAD_SEGMENT::GetRadius( void ) const
{
    return mradius;
}


double MCAD_SEGMENT::GetStartAngle( void ) const
{
    if( mCWArc )
        return meang;

    return msang;
}


double MCAD_SEGMENT::GetEndAngle( void ) const
{
    if( mCWArc )
        return msang;

    return meang;
}


double MCAD_SEGMENT::GetMSAngle( void ) const
{
    return msang;
}


double MCAD_SEGMENT::GetMEAngle( void ) const
{
    return meang;
}


MCAD_POINT MCAD_SEGMENT::GetCenter( void ) const
{
    return mcenter;
}


MCAD_POINT MCAD_SEGMENT::GetStart( void ) const
{
    // ensure that the start/end points given
    // describe a CCW arc
    if( mCWArc )
        return mend;

    return mstart;
}


MCAD_POINT MCAD_SEGMENT::GetMStart( void ) const
{
    return mstart;
}


MCAD_POINT MCAD_SEGMENT::GetEnd( void ) const
{
    // ensure that the start/end points given
    // describe a CCW arc
    if( mCWArc )
        return mstart;

    return mend;
}


MCAD_POINT MCAD_SEGMENT::GetMEnd( void ) const
{
    return mend;
}


// reverse the point order if applicable
void MCAD_SEGMENT::reverse( void )
{
    if( MCAD_SEGTYPE_NONE == msegtype || MCAD_SEGTYPE_CIRCLE == msegtype )
        return;

    MCAD_POINT tmp = mstart;
    mstart = mend;
    mend = tmp;

    if( MCAD_SEGTYPE_ARC == msegtype )
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

bool MCAD_SEGMENT::splitLine( std::list<MCAD_POINT>& aIntersectList,
                              std::list<MCAD_SEGMENT*>& aNewSegmentList )
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

    list<MCAD_POINT>::iterator sPL = aIntersectList.begin();
    list<MCAD_POINT>::iterator ePL = aIntersectList.end();

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
                ERRMSG << "\n + [ERROR] point " << np << " is not on the line (deviation: "
                    << (u - sPL->x) << ")\n +         point( " << sPL->x << ", "
                    << sPL->y << ")\n +         line[ (" << mstart.x << ", " << mstart.y
                    << "), (" << mend.x << ", " << mend.y << ") ]\n";
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

    MCAD_SEGMENT* sp = new MCAD_SEGMENT;
    MCAD_POINT p0;

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

    MCAD_POINT p1;

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
    sp = new MCAD_SEGMENT;
    sp->SetParams( p1, mend );
    aNewSegmentList.push_back( sp );

    // adjust this segment
    mend = p0;
    return true;
}


bool MCAD_SEGMENT::splitArc( std::list<MCAD_POINT>& aIntersectList,
                             std::list<MCAD_SEGMENT*>& aNewSegmentList )
{
    MCAD_POINT p0 = aIntersectList.front();
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

    if( a0 < GetStartAngle() )
        a0 += 2.0 * M_PI;

    if( a0 > GetEndAngle() )
    {
        ERRMSG << "\n + [ERROR] p0 is not on the arc\n";
        return false;
    }

    MCAD_SEGMENT* sp;

    if( 1 == aIntersectList.size() )
    {
        // create the new arc
        sp = new MCAD_SEGMENT;
        sp->mstart = p0;
        sp->mend = mend;
        sp->msang = a0;
        sp->meang = meang;
        sp->mCWArc = mCWArc;
        sp->msegtype = MCAD_SEGTYPE_ARC;
        sp->mradius = mradius;
        sp->mcenter = mcenter;
        aNewSegmentList.push_back( sp );

        // ensure normalized angles in new segment
        NORMALIZE_ANGLES( sp->msang, sp->meang );

        // adjust the extent of this arc
        mend = p0;
        meang = a0;

        return true;
    }

    MCAD_POINT p1 = aIntersectList.back();

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

    if( a1 < GetStartAngle() )
        a1 += 2.0 * M_PI;

    if( a1 > GetEndAngle() )
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
            MCAD_POINT tp0 = p0;
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
            MCAD_POINT tp0 = p0;
            p0 = p1;
            p1 = tp0;
        }
    }

    // create the first of 2 new arcs
    sp = new MCAD_SEGMENT;
    sp->mstart = p0;
    sp->mend = p1;
    sp->msang = a0;
    sp->meang = a1;
    sp->mCWArc = mCWArc;
    sp->msegtype = MCAD_SEGTYPE_ARC;
    sp->mradius = mradius;
    sp->mcenter = mcenter;
    aNewSegmentList.push_back( sp );

    // ensure normalized angles in new segment
    NORMALIZE_ANGLES( sp->msang, sp->meang );

    // create the second of 2 new arcs
    sp = new MCAD_SEGMENT;
    sp->mstart = p1;
    sp->mend = mend;
    sp->msang = a1;
    sp->meang = meang;
    sp->mCWArc = mCWArc;
    sp->msegtype = MCAD_SEGTYPE_ARC;
    sp->mradius = mradius;
    sp->mcenter = mcenter;
    aNewSegmentList.push_back( sp );

    // ensure normalized angles in new segment
    NORMALIZE_ANGLES( sp->msang, sp->meang );

    // adjust the extent of this arc
    mend = p0;
    meang = a0;

    return true;
}


bool MCAD_SEGMENT::splitCircle( std::list<MCAD_POINT>& aIntersectList,
                                std::list<MCAD_SEGMENT*>& aNewSegmentList )
{
    if( aIntersectList.size() != 2 )
    {
        ERRMSG << "\n + [ERROR] splitting a circle requires 2 points, " << aIntersectList.size() << " provided\n";
        return false;
    }

    MCAD_POINT p0 = aIntersectList.front();
    MCAD_POINT p1 = aIntersectList.back();

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

    MCAD_SEGMENT* sp = new MCAD_SEGMENT;

    if( !sp )
    {
        ERRMSG << "\n + [ERROR] could not allocate memory\n";
        return false;
    }

    // explicitly assign values for the new arc; this saves
    // some computation time over SetParams()
    sp->msegtype = MCAD_SEGTYPE_ARC;
    sp->mcenter = mcenter;
    sp->mstart = p0;
    sp->mend = p1;
    sp->msang = a0;
    sp->meang = a1;
    sp->mradius = mradius;
    sp->mCWArc = false;

    if( sp->msang > sp->meang )
        sp->meang += 2.0 * M_PI;

    // ensure normalized angles in new segment
    NORMALIZE_ANGLES( sp->msang, sp->meang );

    aNewSegmentList.push_back( sp );

    // The preserved section of the circle runs CCW from p1 to p0
    mstart = p1;
    mend = p0;
    msegtype = MCAD_SEGTYPE_ARC;
    mCWArc = false;
    msang = a1;
    meang = a0;

    if( msang > meang )
        meang += 2.0 * M_PI;

    // ensure normalized angles in old segment
    NORMALIZE_ANGLES( msang, meang );

    return true;
}
