/*
 * file: geom_outline.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: This class represents an outline which consists of
 * a list of segments. Once the segments form a closed loop no
 * more segments may be added and the internal segments are arranged
 * in a counterclockwise fashion.
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

#include <sstream>
#include <cmath>
#include <iges.h>
#include <error_macros.h>
#include <iges_helpers.h>
#include <geom_segment.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <geom_outline.h>

using namespace std;

#define GEOM_ERR( msg ) do { \
    msg.str(""); \
    msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "; \
} while( 0 )


struct GEOM_INTERSECT
{
    IGES_POINT vertex;
    IGES_GEOM_SEGMENT* segA;    // pointer to the segment operated upon
    IGES_GEOM_SEGMENT* segB;    // pointer to the segment modifying segA
    std::list<IGES_GEOM_SEGMENT*>::iterator iSegA;  // iterator to the segment operated upon
    std::list<IGES_GEOM_SEGMENT*>::iterator iSegB;  // iterator to the segment operation's argument, else = iSegA

    GEOM_INTERSECT()
    {
        segA = NULL;
        segB = NULL;
    }
};


static void print_point( IGES_POINT p0 )
{
    cout << "(" << p0.x << ", " << p0.y << ")\n";
}

static void print_seg( IGES_GEOM_SEGMENT* seg )
{
    cout << "      type: ";

    switch( seg->getSegType() )
    {
        case IGES_SEGTYPE_NONE:
            cout << "NONE\n";
            break;

        case IGES_SEGTYPE_ARC:
            cout << "ARC\n";
            cout << "c";
            print_point( seg->getCenter() );
            cout << "s";
            print_point( seg->getStart() );
            cout << "e";
            print_point( seg->getEnd() );
            break;

        case IGES_SEGTYPE_CIRCLE:
            cout << "CIRCLE\n";
            cout << "c";
            print_point( seg->getCenter() );
            cout << "r:" << seg->getRadius();
            break;

        case IGES_SEGTYPE_LINE:
            cout << "LINE\n";
            cout << "s";
            print_point( seg->getStart() );
            cout << "e";
            print_point( seg->getEnd() );
            break;

        default:
            cout << "INVALID\n";
            break;
    }
}


static void print_geom_intersects( const list<GEOM_INTERSECT>& aList )
{
    list<GEOM_INTERSECT>::const_iterator sL = aList.begin();
    list<GEOM_INTERSECT>::const_iterator eL = aList.end();

    while( sL != eL )
    {
        cerr << "** GEOM_INTERSECT\n";
        cerr << "   point(" << sL->vertex.x << ", " << sL->vertex.y << ")\n";
        cerr << "   segA: " << sL->segA << "\n";
        print_seg( sL->segA );
        ++sL;
    }
}


IGES_GEOM_OUTLINE::IGES_GEOM_OUTLINE()
{
    mIsClosed = false;
    mWinding = 0.0;
    return;
}


IGES_GEOM_OUTLINE::~IGES_GEOM_OUTLINE()
{
    while( !msegments.empty() )
    {
        delete msegments.back();
        msegments.pop_back();
    }

    while( !mcutouts.empty() )
    {
        delete mcutouts.back();
        mcutouts.pop_back();
    }

    while( !mholes.empty() )
    {
        delete mholes.back();
        mholes.pop_back();
    }

    return;
}


// Retrieve the error stack
const std::list< std::string >* IGES_GEOM_OUTLINE::GetErrors( void )
{
    return &errors;
}


// Clear the error stack
void IGES_GEOM_OUTLINE::ClearErrors( void )
{
    errors.clear();
    return;
}


// Returns 'true' if the outline is closed
bool IGES_GEOM_OUTLINE::IsClosed( void )
{
    return mIsClosed;
}


// Returns 'true' if the point is on or inside this outline
bool IGES_GEOM_OUTLINE::IsInside( IGES_POINT aPoint, bool& error )
{
    // always fail if the outline is not closed
    if( !mIsClosed )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] outline is not closed";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    // Steps:
    // 1. take a line passing through this point and directly to the
    //    left or right, whichever is the shortest segment.
    // 2. count nodes: IF an intersection is an endpoint, only
    //    count it as a node if ALL points of the segment touched
    //    are <= aPoint.y.
    // 3. odd nodes = inside, even nodes = outside

    IGES_POINT bb0 = mBottomLeft;
    IGES_POINT bb1 = mTopRight;

    // expand the limits to ensure non-zero segment lengths in all cases
    bb0.x -= 5.0;
    bb0.y -= 5.0;
    bb1.x += 5.0;
    bb1.y += 5.0;

    // note: if the point is out of bounds then it is clearly not
    // within the outline
    if( aPoint.x < mBottomLeft.x || aPoint.y < mBottomLeft.y
        || aPoint.x > mTopRight.x || aPoint.y > mTopRight.y )
    {
        cout << "XXX: point(" << aPoint.x << ", " << aPoint.y << ")\n";
        cout << "XXX: outside ( out of bounds )\n";
        return false;
    }

    IGES_POINT p2;

    if( (aPoint.x - mBottomLeft.x) <= (mTopRight.x - aPoint.x) )
        p2.x = bb0.x;
    else
        p2.x = bb1.x;

    p2.y = aPoint.y;

    IGES_GEOM_SEGMENT ls0;
    ls0.SetParams( aPoint, p2 );
    int nI = 0; // number of intersections with the outline

    list<IGES_GEOM_SEGMENT*>::iterator sSegs = msegments.begin();
    list<IGES_GEOM_SEGMENT*>::iterator eSegs = msegments.end();
    list<IGES_POINT> iList;
    IGES_INTERSECT_FLAG flag;

    while( sSegs != eSegs )
    {
        if( (*sSegs)->GetIntersections( ls0, iList, flag ) )
        {
            list<IGES_POINT>::iterator sL = iList.begin();
            list<IGES_POINT>::iterator eL = iList.end();

            while( sL != eL )
            {
                // note: handle the case of a circle differently
                if( IGES_SEGTYPE_CIRCLE == (*sSegs)->getSegType() )
                {
                    ++nI;
                }
                else
                {
                    if( PointMatches( *sL, (*sSegs)->getStart(), 1e-8 )
                        || PointMatches( *sL, (*sSegs)->getEnd(), 1e-8 ) )
                    {
                        (*sSegs)->GetBoundingBox( bb0, bb1 );

                        if( bb0.y <= aPoint.y && bb1.y <= aPoint.y )
                            ++nI;

                    }
                    else
                    {
                        ++nI;
                    }
                }

                ++sL;
            }

            iList.clear();
        }

        ++sSegs;
    }

    cout << "XXX: point(" << aPoint.x << ", " << aPoint.y << ")\n";
    cout << "XXX: nI: " << nI << "\n";

    // note: an odd number means the point is inside the outline
    if( nI % 2 )
        return true;

    return false;
}


// Add a segment to this outline; the user must ensure that
// the outline is closed before performing any other type
// of operation.
bool IGES_GEOM_OUTLINE::AddSegment( IGES_GEOM_SEGMENT* aSegment, bool& error )
{
    if( NULL == aSegment )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( IGES_SEGTYPE_NONE == aSegment->getSegType() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] invalid segment type";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( mIsClosed )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] outline is already closed";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    error = false;

    if( IGES_SEGTYPE_CIRCLE == aSegment->getSegType() )
    {
        if( !msegments.empty() )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[BUG] a circle cannot be added to a non-empty outline";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }

        msegments.push_back( aSegment );
        mIsClosed = true;
        aSegment->GetBoundingBox( mBottomLeft, mTopRight );
        return true;
    }

    // note: do not use getStart(), getEnd() as those functions
    // ensure CCW order on an arc whereas mstart, mend ensure
    // actual endpoint order
    IGES_POINT p0;
    IGES_POINT p1;
    IGES_GEOM_SEGMENT* pseg;

    if( !msegments.empty() )
    {
        // check for continuity
        pseg = msegments.back();
        p0 = msegments.back()->mend;
        p1 = aSegment->mstart;

        if( !PointMatches( p0, p1, 1e-8 ) )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[ERROR] endpoints do not match within 1e-8";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }
    }
    else
    {
        // retrieve the initial bouning box
        aSegment->GetBoundingBox( mBottomLeft, mTopRight );
    }

    msegments.push_back( aSegment );

    // calculate winding
    pseg = msegments.back();
    // note: do not use getStart(), getEnd() as those functions
    // ensure CCW order on an arc whereas mstart, mend ensure
    // actual endpoint order
    p0 = pseg->mend;
    p1 = pseg->mstart;

    mWinding += ( p0.x - p1.x ) * ( p0.y + p1.y );

    if( msegments.size() > 1 )
    {
        // adjust the bounding box
        IGES_POINT bb0;
        IGES_POINT bb1;

        aSegment->GetBoundingBox( bb0, bb1 );

        if( bb0.x < mBottomLeft.x )
            mBottomLeft.x = bb0.x;

        if( bb0.y < mBottomLeft.y )
            mBottomLeft.y = bb0.y;

        if( bb1.x > mTopRight.x )
            mTopRight.x = bb1.x;

        if( bb1.y > mTopRight.y )
            mTopRight.y = bb1.y;

        // check if the outline is closed
        p1 = msegments.front()->mstart;

        if( PointMatches( p0, p1, 1e-8 ) )
        {
            mIsClosed = true;

            // check the special case where we have only 2 segments
            if( msegments.size() == 2 )
            {
                IGES_GEOM_SEGMENT* sp0;
                IGES_GEOM_SEGMENT* sp1;

                if( msegments.front()->getSegType() == IGES_SEGTYPE_ARC
                    && msegments.back()->getSegType() == IGES_SEGTYPE_ARC )
                {
                    // 2 arcs: if both are CW then reverse the segments,
                    // otherwise change nothing
                    sp0 = msegments.front();
                    sp1 = msegments.back();

                    if( sp0->isArcCW() && sp1->isArcCW() )
                    {
                        sp0->reverse();
                        sp1->reverse();
                    }
                }
                else
                {
                    // 1 arc and 1 line

                    if( msegments.front()->getSegType() == IGES_SEGTYPE_ARC )
                    {
                        sp0 = msegments.front();
                        sp1 = msegments.back();
                    }
                    else
                    {
                        sp0 = msegments.back();
                        sp1 = msegments.front();
                    }

                    if( sp0->isArcCW() )
                    {
                        sp0->reverse();
                        sp1->reverse();
                    }
                }
            }
            else
            {
                if( mWinding < 0.0 )
                {
                    // reverse everything
                    // XXX - TO BE IMPLEMENTED
                }
            }   // if we have only 2 segments in the closed outline
        }       // if outline is now closed
    }           // if #Segments > 1

    return true;
}


// Merge the given closed outline with this one; to keep the
// code simple, the following restriction is imposed:
// the two outlines may only intersect at 2 points.
bool IGES_GEOM_OUTLINE::AddOutline( IGES_GEOM_OUTLINE* aOutline, bool& error )
{
    #warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    return false;
}


// Subtract the given circular segment from this outline; to keep the
// code simple, the following restriction is imposed:
// the two outlines may only intersect at 2 points.
bool IGES_GEOM_OUTLINE::SubOutline( IGES_GEOM_SEGMENT* aCircle, bool& error )
{
    cout << "XXX: SUB OUTLINE ITERATION\n";
    if( !mIsClosed )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] outline is not closed";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( NULL == aCircle )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( IGES_SEGTYPE_CIRCLE != aCircle->getSegType() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] segment is not a circle";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    list<GEOM_INTERSECT> intersects;
    list<IGES_POINT> iList;
    list<IGES_GEOM_SEGMENT*>::iterator iSeg = msegments.begin();
    list<IGES_GEOM_SEGMENT*>::iterator eSeg = msegments.end();
    IGES_INTERSECT_FLAG flag;

    int acc = 1;    // XXX - DEBUG
    while( iSeg != eSeg )
    {
        flag = IGES_IFLAG_NONE;
        iList.clear();

        if( (*iSeg)->GetIntersections( *aCircle, iList, flag ) )
        {
            cout << "XXX: INTERSECT in seg " << acc << " of " << msegments.size() << "\n";
            print_seg(*iSeg);

            if( IGES_IFLAG_NONE != flag )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[INFO] flag was set on intersect: " << flag << "(treated as invalid geom.)";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                return false;
            }

            std::list<IGES_POINT>::iterator iPts = iList.begin();
            std::list<IGES_POINT>::iterator ePts = iList.end();

            cout << "XXX: iList has " << iList.size() << " intersections\n";

            while( iPts != ePts )
            {
                GEOM_INTERSECT gi;
                gi.vertex = *iPts;
                cout << "XXX: v";
                print_point( *iPts );
                gi.segA = *iSeg;
                gi.segB = aCircle;
                gi.iSegA = iSeg;
                gi.iSegB = iSeg;
                intersects.push_back( gi );
                ++iPts;
            }
        }
        else
        {
            if( IGES_IFLAG_NONE != flag )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[INFO] invalid geometry: flag = " << flag;
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                return false;
            }
            cout << "XXX: NO X in seg " << acc << " of " << msegments.size() << "\n";
        }

        ++acc;
        ++iSeg;
    }

    // Possible number of *distinct* intersections:
    // a. 0: all is good, no intersection
    // b. 1: bad geometry, intersection at a point
    // c. 2: If both points are endpoints and both
    //       endpoints are *not* common to a single
    //       segment, then we have invalid geometry,
    //       otherwise we can trim the outline
    // d. 3 or more: bad geometry: violates 2-point
    //       restriction on intersections.

    if( intersects.empty() )
        return false;

    iList.clear();
    list<list<IGES_GEOM_SEGMENT*>::iterator> lSegs;

    // compute the number of unique intersecting points:
    int nI = 0;
    list<GEOM_INTERSECT>::iterator iIn = intersects.begin();
    list<GEOM_INTERSECT>::iterator eIn = intersects.end();

    while( iIn != eIn )
    {
        if( iList.empty() )
        {
            iList.push_back( iIn->vertex );
            lSegs.push_back( iIn->iSegA );
        }
        else
        {
            std::list<IGES_POINT>::iterator iPts = iList.begin();
            std::list<IGES_POINT>::iterator ePts = iList.end();
            bool isUnique = true;

            while( iPts != ePts )
            {
                if( PointMatches( *iPts, iIn->vertex, 1e-8 ) )
                {
                    isUnique = false;
                    break;
                }

                ++iPts;
            }

            if( isUnique )
            {
                iList.push_back( iIn->vertex );
                lSegs.push_back( iIn->iSegA );
            }
        }

        ++iIn;
    }

    nI = (int)iList.size();

    if( 1 > nI || 2 < nI )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] invalid geometry: violates restriction of 2 unique intersections (n = " << nI << ")";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        print_geom_intersects( intersects );
        return false;
    }

    // determine number of endpoints
    bool p1e = false;   // set to true if Point 1 is an endpoint
    bool p2e = false;   // set to true if Point 2 is an endpoint
    IGES_GEOM_SEGMENT* bypass = NULL;

    do
    {
        // check for endpoints
        list<GEOM_INTERSECT>::iterator iIn = intersects.begin();
        list<GEOM_INTERSECT>::iterator eIn = intersects.end();

        while( iIn != eIn )
        {
            if( iIn->segA->getSegType() != IGES_SEGTYPE_CIRCLE )
            {
                if( !p1e && ( PointMatches( iList.front(), iIn->segA->getStart(), 1e-8 )
                    || PointMatches( iList.front(), iIn->segA->getEnd(), 1e-8 ) ) )
                {
                    p1e = true;
                }

                if( !p2e && ( PointMatches( iList.back(), iIn->segA->getStart(), 1e-8 )
                    || PointMatches( iList.back(), iIn->segA->getEnd(), 1e-8 ) ) )
                {
                    p2e = true;
                }
            }

            ++iIn;
        }
    } while( 0 );

    // we can trim the entity using the given circle; determine which section of the
    // circle is inside the outline
    IGES_POINT p0 = aCircle->mcenter;
    IGES_POINT p1 = iList.front();

    double a1 = atan2( p1.y - p0.y, p1.x - p0.x );
    p1 = iList.back();
    double a2 = atan2( p1.y - p0.y, p1.x - p0.x );

    if( a1 < 0.0 )
        a1 += 2.0 * M_PI;

    if( a2 < 0.0 )
        a2 += 2.0 * M_PI;

    double a3 = (a1 + a2) / 2.0;

    IGES_POINT pX;  // a point midway along the 2nd CCW section of the circle
    pX.x = p0.x + aCircle->mradius * cos( a3 );
    pX.y = p0.y + aCircle->mradius * sin( a3 );

    error = 0;
    bool isIn = IsInside( pX, error );

    if( !isIn && error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] IsInside() failed; see previous messages";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    if( isIn )
        cout << "XXX: (POINT INSIDE)\n";
    else
    {
        cout << "XXX: (POINT OUTSIDE)\n";
        cout << "c";
        print_point(p0);
        cout << "s";
        print_point(iList.front());
        cout << "e";
        print_point(iList.back());
    }

    cout << "XXX: expect 2 intersections, have " << iList.size() << "\n";

    IGES_POINT pF[2];   // final point order
    bool isEnd[2];      // indicates if pF[n] is an endpoint
    list<IGES_GEOM_SEGMENT*>::iterator pSeg[2]; // segment iterators associated with each point

    // note: The IN segment must be put in CW order and
    // its endpoints shall split the appropriate outline
    // entities. From the new outline list, take the outline
    // whose 'mend' equals 'mstart' of the new arc segment
    // then eradicate all following items until we encounter
    // the segment whose 'mstart' equals the new arc's 'mend'.
    // Append the new arc to the FIRST segment mentioned above.
    //
    // Special case: if the outline is a circle then simply
    // split the circle using the (mend, mstart) points of the
    // new arc segment, add the new segment and discard the
    // second segment returned by the Split() operator.
    //
    // Splitting entities: Keep in mind that a circle may
    // split an arc or line into 2 or 3 parts. If a point
    // is also an endpoint then the Split() function shall
    // not be invoked for that point. All items to be
    // Split() (either 0, 1, or 2 entities) shall be put
    // into a list; if the list has 2 entities and they are
    // the same entity then the single Split() operator
    // must specify both split points.

    cout << "XXX: lSegs.size(): " << lSegs.size() << "\n";
    if( isIn )
    {
        if( a2 > a1 )
        {
            pF[1] = iList.front();
            pF[0] = iList.back();
            isEnd[1] = p1e;
            isEnd[0] = p2e;
            pSeg[1] = lSegs.front();
            pSeg[0] = lSegs.back();
        }
        else
        {
            pF[0] = iList.front();
            pF[1] = iList.back();
            isEnd[0] = p1e;
            isEnd[1] = p2e;
            pSeg[0] = lSegs.front();
            pSeg[1] = lSegs.back();
        }
    }
    else
    {
        if( a1 > a2 )
        {
            pF[1] = iList.front();
            pF[0] = iList.back();
            isEnd[1] = p1e;
            isEnd[0] = p2e;
            pSeg[1] = lSegs.front();
            pSeg[0] = lSegs.back();
        }
        else
        {
            pF[0] = iList.front();
            pF[1] = iList.back();
            isEnd[0] = p1e;
            isEnd[1] = p2e;
            pSeg[0] = lSegs.front();
            pSeg[1] = lSegs.back();
        }
    }

    IGES_GEOM_SEGMENT* sp = new IGES_GEOM_SEGMENT;

    if( !sp->SetParams( p0, pF[0], pF[1], true ) )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] intersections do not lie on the circular cutout";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        cout << "  c";
        print_point( p0 );
        cout << "  s";
        print_point( pF[0] );
        cout << "  e";
        print_point( pF[1] );
        delete sp;
        return false;
    }

    if( p1e && p2e )
    {
        // do not trim if length(sp) < length(seg)
        if( bypass )
        {
            if( sp->GetLength() < bypass->GetLength() )
            {
                cout << "XXX: not applying trim\n";
                delete sp;
                return false;
            }
        }
    }

    if( msegments.front()->getSegType() == IGES_SEGTYPE_CIRCLE )
    {
        // Special case: this outline is currently a circle
        iList.clear();
        iList.push_back(pF[0]);
        iList.push_back(pF[1]);
        list<IGES_GEOM_SEGMENT*> sList;

        if( !(*pSeg[0])->Split(iList, sList) )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[BUG] could not split circle";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            delete sp;
            return false;
        }

        // a single new segment should have been returned;
        // that segment should simply be discarded
        while( !sList.empty() )
        {
            delete sList.back();
            sList.pop_back();
        }

        msegments.push_back( sp );
        return true;
    }

    if( !p1e && !p2e )
    {
        // if both points lie on a single segment then split at 2 points
        if( pSeg[0] == pSeg[1] )
        {
            cout << "XXX: splitting at multiple points\n";
            iList.clear();
            iList.push_back(pF[0]);
            iList.push_back(pF[1]);
            list<IGES_GEOM_SEGMENT*> sList;

            if( !(*pSeg[0])->Split(iList, sList) )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] could not split segment";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                delete sp;
                return false;
            }

            if( sList.size() != 2 )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] expected 2 new segments, got " << sList.size();
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                delete sp;

                while( !sList.empty() )
                {
                    delete sList.back();
                    sList.pop_back();
                }

                return false;
            }

            // replace the first new segment with the arc
            delete sList.front();
            sList.pop_front();
            sList.push_front( sp );
            list<IGES_GEOM_SEGMENT*>::iterator pS0 = pSeg[0];
            msegments.insert( ++pS0, sList.begin(), sList.end() );
            return true;
        }
    }

    cout << "XXX: splitting at single points\n";
    cout << "XXX: (p1e, p2e) : (" << p1e << ", " << p2e << ")\n";

    // perform the splits
    for( int i = 0; i < 2; ++ i )
    {
        if( !isEnd[i] )
        {
            cout << "XXX: splitting, i = " << i << "\n";
            // split here
            iList.clear();
            iList.push_back(pF[i]);
            list<IGES_GEOM_SEGMENT*> sList;

            if( !(*pSeg[i])->Split(iList, sList) )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] could not split segment";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                cout << "Segment to be split:\n";
                print_seg(*pSeg[i]);
                cout << "Split point v";
                print_point(pF[i]);
                delete sp;
                return false;
            }

            if( sList.size() != 1 )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] expected 1 segment only, got " << sList.size();
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                delete sp;

                while( !sList.empty() )
                {
                    delete sList.back();
                    sList.pop_back();
                }

                return false;
            }

            // a single new segment should have been returned; add it
            // to the list of segments
            list<IGES_GEOM_SEGMENT*>::iterator pS0 = pSeg[i];
            msegments.insert( ++pS0, sList.front() );
        }
    }

    // either pSeg[0] or pSeg[0]-- must have an endpoint 'mend' which
    // is equal to pf[0]; find that iterator then eradicate all
    // subsequent entries (wrapping to begin() if necessary) until
    // we encounter a segment whose 'mstart' equals pf[1]. Finally,
    // insert 'sp' after the aforementioned iterator or, more
    // conveniently, before the first segment not eradicated.

    if( !PointMatches( (*pSeg[0])->mend, pF[0], 1e-8 ) )
    {
        if( msegments.front() == *pSeg[0] )
            pSeg[0] = --msegments.end();
        else
            --pSeg[0];
    }

    if( !PointMatches( (*pSeg[0])->mend, pF[0], 1e-8 ) )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] expected (*pSeg[0])->mend to match pF[0]";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        delete sp;
        return false;
    }

    list<IGES_GEOM_SEGMENT*>::iterator tSeg = pSeg[0];
    ++tSeg;

    while( true )
    {
        if( msegments.end() == tSeg )
        {
            tSeg = msegments.begin();

            if( msegments.empty() )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] deleted entire outline";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                delete sp;
                return false;
            }
        }

        if( PointMatches( (*tSeg)->mstart, pF[1], 1e-8 ) )
            break;

        delete *tSeg;
        tSeg = msegments.erase( tSeg );
    }

    msegments.insert( pSeg[0], sp );
    return true;
}


// Subtract the given outline from this one; to keep the
// code simple, the following restriction is imposed:
// the two outlines may only intersect at 2 points.
bool IGES_GEOM_OUTLINE::SubOutline( IGES_GEOM_OUTLINE* aOutline, bool& error )
{
    #warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    return false;
}


// Add the given cutout in preparation for exporting a solid model.
// If the cutout is known to be non-overlapping then the 'overlaps'
// flag may be set to 'false' to skip overlap tests. If the user
// does not know whether the outline overlaps or not, then the
// overlaps flag must be set to 'true' to ensure that checks are
// performed to ensure valid geometry.
bool IGES_GEOM_OUTLINE::AddCutout( IGES_GEOM_OUTLINE* aCutout, bool overlaps, bool& error )
{
    #warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    return false;
}

// Add the given circular segment as a cutout; if the segment is
// known to be non-overlapping then 'overlaps' may be set to 'false',
// otherwise it must be set to 'true'. If the function succeeds it
// will manage the given segment; if the function returns false
// then it is the caller's responsibility to dispose of the object.
bool IGES_GEOM_OUTLINE::AddCutout( IGES_GEOM_SEGMENT* aCircle, bool overlaps, bool& error )
{
    if( NULL == aCircle )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( IGES_SEGTYPE_CIRCLE != aCircle->getSegType() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] segment is not a circle";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( !overlaps )
    {
        cout << "XXX: adding without tests\n";
        mholes.push_back( aCircle );
        return true;
    }

    cout << "XXX: checking overlap\n";
    if( SubOutline( aCircle, error ) )
        return true;
    cout << "XXX: no overlap; checking for error\n";

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] could not apply cutout";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    cout << "XXX: no error; adding to list\n";
    mholes.push_back( aCircle );
    return true;
}

// retrieve trimmed parametric surfaces representing vertical sides
// of the main outline and all cutouts
bool IGES_GEOM_OUTLINE::GetVerticalSurface( IGES* aModel, bool& error,
                                            std::vector<IGES_ENTITY_144*>& aSurface,
                                            double aTopZ, double aBotZ )
{
    error = false;

    if( !mIsClosed )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] outline is not closed";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( msegments.empty() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] outline is empty";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    list<IGES_GEOM_SEGMENT*>::iterator sSeg = msegments.begin();
    list<IGES_GEOM_SEGMENT*>::iterator eSeg = msegments.end();

    while( sSeg != eSeg )
    {
        if( !(*sSeg)->GetVerticalSurface( aModel, aSurface, aTopZ, aBotZ ) )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[ERROR] could not render a vertical surface of a segment";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }

        ++sSeg;
    }

    if( !mholes.empty() )
    {
        sSeg = mholes.begin();
        eSeg = mholes.end();

        while( sSeg != eSeg )
        {
            if( !(*sSeg)->GetVerticalSurface( aModel, aSurface, aTopZ, aBotZ ) )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[ERROR] could not render a vertical surface of a hole";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                return false;
            }

            ++sSeg;
        }
    }

    if( !mcutouts.empty() )
    {
        list<IGES_GEOM_OUTLINE*>::iterator sOtln = mcutouts.begin();
        list<IGES_GEOM_OUTLINE*>::iterator eOtln = mcutouts.end();

        while( sOtln != eOtln )
        {
            if( !(*sOtln)->GetVerticalSurface( aModel, error, aSurface, aTopZ, aBotZ ) )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[ERROR] could not render a vertical surface of a cutout";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                return false;
            }

            ++sOtln;
        }
    }


    return true;
}
