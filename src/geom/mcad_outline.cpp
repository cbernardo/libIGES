/*
 * file: mcad_outline.cpp
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

#include <sstream>
#include <cmath>
#include <algorithm>
#include <error_macros.h>
#include <geom/mcad_helpers.h>
#include <geom/mcad_segment.h>
#include <geom/mcad_outline.h>

using namespace std;

#define GEOM_ERR( msg ) do { \
    msg.str(""); \
    msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "; \
} while( 0 )


void MCAD_OUTLINE::PrintPoint( MCAD_POINT p0 )
{
    cerr << "(" << p0.x << ", " << p0.y << ")\n";
}


void MCAD_OUTLINE::PrintSeg( MCAD_SEGMENT* seg )
{
    cerr << "      type: ";

    switch( seg->GetSegType() )
    {
        case MCAD_SEGTYPE_NONE:
            cerr << "NONE\n";
            break;

        case MCAD_SEGTYPE_ARC:
            cerr << "ARC\n";
            cerr << "            c";
            PrintPoint( seg->GetCenter() );
            cerr << "            s";
            PrintPoint( seg->GetStart() );
            cerr << "            e";
            PrintPoint( seg->GetEnd() );
            cerr << "            cw: " << seg->IsCW() << "\n";
            cerr << "            ang_start/ang_end: ";
            cerr << seg->GetStartAngle() << ", " << seg->GetEndAngle() << "\n";
            break;

        case MCAD_SEGTYPE_CIRCLE:
            cerr << "CIRCLE\n";
            cerr << "            c";
            PrintPoint( seg->GetCenter() );
            cerr << "            r:" << seg->GetRadius();
            break;

        case MCAD_SEGTYPE_LINE:
            cerr << "LINE\n";
            cerr << "            s";
            PrintPoint( seg->GetStart() );
            cerr << "            e";
            PrintPoint( seg->GetEnd() );
            break;

        default:
            cerr << "INVALID\n";
            break;
    }
}


void MCAD_OUTLINE::PrintGeomIntersects( const list<MCAD_INTERSECT>& aList )
{
    list<MCAD_INTERSECT>::const_iterator sL = aList.begin();
    list<MCAD_INTERSECT>::const_iterator eL = aList.end();

    while( sL != eL )
    {
        cerr << "** MCAD_INTERSECT\n";
        cerr << "   point(" << sL->vertex.x << ", " << sL->vertex.y << ")\n";
        cerr << "   segA: " << sL->segA << "\n";
        PrintSeg( sL->segA );
        ++sL;
    }
}


MCAD_OUTLINE::MCAD_OUTLINE()
{
    mIsClosed = false;
    mWinding = 0.0;
    mBBisOK = false;
    m_OutlineType = MCAD_OT_BASE;
    return;
}


MCAD_OUTLINE::~MCAD_OUTLINE()
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

    list< bool* >::iterator sVF = m_validFlags.begin();
    list< bool* >::iterator eVF = m_validFlags.end();

    while( sVF != eVF )
    {
        **sVF = false;
        ++sVF;
    }

    return;
}


MCAD_OUTLINE_TYPE MCAD_OUTLINE::GetOutlineType( void )
{
    return m_OutlineType;
}


void MCAD_OUTLINE::AttachValidFlag( bool* aFlag )
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


void MCAD_OUTLINE::DetachValidFlag( bool* aFlag )
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


// Retrieve the error stack
const std::list< std::string >* MCAD_OUTLINE::GetErrors( void )
{
    return &errors;
}


// Clear the error stack
void MCAD_OUTLINE::ClearErrors( void )
{
    errors.clear();
    return;
}


// Returns 'true' if the outline is closed
bool MCAD_OUTLINE::IsClosed( void )
{
    return mIsClosed;
}


// Returns 'true' if the (closed) outline is contiguous
bool MCAD_OUTLINE::IsContiguous( void )
{
    if( msegments.empty() )
    {
        ERRMSG << "\n + [INFO] empty outline\n";
        return false;
    }

    if( !mIsClosed )
    {
        ERRMSG << "\n + [INFO] outline is not closed\n";
        return false;
    }

    list<MCAD_SEGMENT*>::iterator sO = msegments.begin();
    list<MCAD_SEGMENT*>::iterator eO = msegments.end();
    list<MCAD_SEGMENT*>::iterator pO = --msegments.end();

    if( MCAD_SEGTYPE_CIRCLE == (*sO)->GetSegType() )
        return true;

    int acc = 0;
    bool bad = false;

    while( sO != eO )
    {
        if( !PointMatches( (*sO)->mstart, (*pO)->mend, 1e-8 ) )
        {
            ERRMSG << "\n + [INFO] discontinuous at seg_" << acc << "\n";
            bad = true;
        }

        ++acc;
        pO = sO;
        ++sO;
    }

    if( bad )
        return false;

    return true;
}


// Returns 'true' if the point is on or inside this outline
bool MCAD_OUTLINE::IsInside( MCAD_POINT aPoint, bool& error )
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

    MCAD_POINT bb0 = mBottomLeft;
    MCAD_POINT bb1 = mTopRight;

    // expand the limits to ensure non-zero segment lengths in all cases
    bb0.x -= 5.0;
    bb0.y -= 5.0;
    bb1.x += 5.0;
    bb1.y += 5.0;

    MCAD_POINT p2;

    if( (aPoint.x - mBottomLeft.x) <= (mTopRight.x - aPoint.x) )
        p2.x = bb0.x;
    else
        p2.x = bb1.x;

    p2.y = aPoint.y;

    MCAD_SEGMENT ls0;
    ls0.SetParams( aPoint, p2 );
    int nI = 0; // number of intersections with the outline

    list<MCAD_SEGMENT*>::iterator sSegs = msegments.begin();
    list<MCAD_SEGMENT*>::iterator eSegs = msegments.end();
    list<MCAD_POINT> iList;
    MCAD_INTERSECT_FLAG flag;

    int acc = 0;

    while( sSegs != eSegs )
    {
        if( (*sSegs)->GetIntersections( ls0, iList, flag ) )
        {
            list<MCAD_POINT>::iterator sL = iList.begin();
            list<MCAD_POINT>::iterator eL = iList.end();

            while( sL != eL )
            {
                // note: handle the case of a circle differently
                if( MCAD_SEGTYPE_CIRCLE == (*sSegs)->GetSegType() )
                {
                    ++nI;
                }
                else
                {
                    bool isEnd0 = PointMatches( *sL, (*sSegs)->mstart, 1e-8 );
                    bool isEnd1 = PointMatches( *sL, (*sSegs)->mend, 1e-8 );

                    if( isEnd0 || isEnd1 )
                    {
                        (*sSegs)->GetBoundingBox( bb0, bb1 );

                        if( bb0.y <= aPoint.y && bb1.y <= aPoint.y )
                        {
                            ++nI;
                        }
                        else
                        {
                            // it is possible for the simple bounding box test to
                            // fail if the endpoint is on an arc; in such cases
                            // we must check the bounds of the adjacent curve and
                            // increment nI is those bounds are exclusively >= aPoint.y
                            if( MCAD_SEGTYPE_ARC == (*sSegs)->GetSegType() && bb0.y < aPoint.y )
                            {
                                list<MCAD_SEGMENT*>::iterator tSeg = sSegs;

                                if( isEnd1 )
                                {
                                    ++tSeg;

                                    if( tSeg == eSegs )
                                        tSeg = msegments.begin();
                                }
                                else if ( tSeg == msegments.begin() )
                                {
                                    tSeg = --msegments.end();
                                }
                                else
                                {
                                    --tSeg;
                                }

                                (*tSeg)->GetBoundingBox( bb0, bb1 );

                                if( bb0.y >= aPoint.y && bb1.y >= aPoint.y )
                                {
                                    ++nI;
                                }

                            }
                        }

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

    // note: an odd number means the point is inside the outline
    if( nI % 2 )
        return true;

    return false;
}


// Add a segment to this outline; the user must ensure that
// the outline is closed before performing any other type
// of operation.
bool MCAD_OUTLINE::AddSegment( MCAD_SEGMENT* aSegment, bool& error )
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

    if( MCAD_SEGTYPE_NONE == aSegment->GetSegType() )
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

    if( MCAD_SEGTYPE_CIRCLE == aSegment->GetSegType() )
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
        mBBisOK = true;
        adjustBoundingBox();
        return true;
    }

    // note: do not use GetStart(), GetEnd() as those functions
    // ensure CCW order on an arc whereas mstart, mend ensure
    // actual endpoint order
    MCAD_POINT p0;
    MCAD_POINT p1;
    MCAD_SEGMENT* pseg;

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
        // retrieve the initial bounding box
        aSegment->GetBoundingBox( mBottomLeft, mTopRight );
    }

    msegments.push_back( aSegment );

    // calculate winding based on area of the curve;
    // in this case mWinding > 0 = CW
    pseg = msegments.back();
    // note: do not use GetStart(), GetEnd() as those functions
    // ensure CCW order on an arc whereas mstart, mend ensure
    // actual endpoint order
    p0 = pseg->mend;
    p1 = pseg->mstart;

    if( MCAD_SEGTYPE_ARC == pseg->GetSegType() )
    {
        // To ensure correct winding calculations involving
        // arcs we must take the midpoint of the arc and
        // calculate the winding based on 2 segments.
        MCAD_POINT p2;
        pseg->GetMidpoint( p2 );

        mWinding += ( p0.x - p2.x ) * ( p0.y + p2.y );
        mWinding += ( p2.x - p1.x ) * ( p2.y + p1.y );
    }
    else
    {
        mWinding += ( p0.x - p1.x ) * ( p0.y + p1.y );
    }

    if( msegments.size() > 1 )
    {
        // adjust the bounding box
        MCAD_POINT bb0;
        MCAD_POINT bb1;

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
            mBBisOK = true;
            adjustBoundingBox();
            mIsClosed = true;

            // check the special case where we have only 2 segments
            if( msegments.size() == 2 )
            {
                MCAD_SEGMENT* sp0;
                MCAD_SEGMENT* sp1;

                // the 2 segments are either both arcs or an arc and a line
                sp0 = msegments.front();
                sp1 = msegments.back();

                if( mWinding > 0.0 )
                {
                    sp0->reverse();
                    sp1->reverse();
                }

            }
            else
            {
                if( mWinding > 0.0 )
                {
                    // reverse everything
                    list<MCAD_SEGMENT*>::iterator sSeg = msegments.begin();
                    list<MCAD_SEGMENT*>::iterator eSeg = msegments.end();
                    list<MCAD_SEGMENT*> tsegs;

                    while( sSeg != eSeg )
                    {
                        (*sSeg)->reverse();
                        tsegs.push_front( *sSeg );
                        ++sSeg;
                    }

                    msegments.clear();
                    msegments = tsegs;
                    return true;
                }
            }   // if we have only 2 segments in the closed outline
        }       // if outline is now closed
    }           // if #Segments > 1

    return true;
}

// operate on the outline (add/subtract)
bool MCAD_OUTLINE::opOutline( MCAD_SEGMENT* aCircle, bool& error, bool opsub )
{
    mBBisOK = false;

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

    if( MCAD_SEGTYPE_CIRCLE != aCircle->GetSegType() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] segment is not a circle";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    error = false;
    list<MCAD_INTERSECT> intersects;
    list<MCAD_POINT> iList;
    list<MCAD_SEGMENT*>::iterator iSeg = msegments.begin();
    list<MCAD_SEGMENT*>::iterator eSeg = msegments.end();
    MCAD_INTERSECT_FLAG flag;

    int acc = 1;    // XXX - DEBUG
    while( iSeg != eSeg )
    {
        flag = MCAD_IFLAG_NONE;
        iList.clear();

        if( (*iSeg)->GetIntersections( *aCircle, iList, flag ) )
        {
            if( MCAD_IFLAG_NONE != flag && MCAD_IFLAG_ENDPOINT != flag
                && MCAD_IFLAG_TANGENT != flag )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[INFO] flag was set on intersect: " << flag << " (treated as invalid geom.)";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                return false;
            }

            std::list<MCAD_POINT>::iterator iPts = iList.begin();
            std::list<MCAD_POINT>::iterator ePts = iList.end();

            while( iPts != ePts )
            {
                MCAD_INTERSECT gi;
                gi.vertex = *iPts;
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
            if( MCAD_IFLAG_NONE != flag )
            {
                if( opsub && MCAD_IFLAG_ENCIRCLES == flag )
                {
                    // we have a circle within a circle which is
                    // valid geometry in this case but there is
                    // no intersection
                    flag = MCAD_IFLAG_NONE;
                    return false;
                }

                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[INFO] invalid geometry: flag = " << flag;
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                return false;
            }
        }

        ++acc;
        ++iSeg;
    }

    // Possible number of *distinct* intersections:
    // a. 0: all is good, no intersection
    // b. 1: bad geometry, intersection at a point
    // c. 2: we can adjust the outline
    // d. 3 or more: bad geometry: violates 2-point
    //       restriction on intersections.

    if( intersects.empty() )
        return false;

    iList.clear();
    list<list<MCAD_SEGMENT*>::iterator> lSegs;

    // compute the number of unique intersecting points:
    list<MCAD_INTERSECT>::iterator iIn = intersects.begin();
    list<MCAD_INTERSECT>::iterator eIn = intersects.end();

    while( iIn != eIn )
    {
        if( iList.empty() )
        {
            iList.push_back( iIn->vertex );
            lSegs.push_back( iIn->iSegA );
        }
        else
        {
            std::list<MCAD_POINT>::iterator iPts = iList.begin();
            std::list<MCAD_POINT>::iterator ePts = iList.end();
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

    if( iList.size() != 2 )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] invalid geometry: violates restriction of 2 unique intersections (n = ";
        msg << iList.size() << ")";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        PrintGeomIntersects( intersects );
        return false;
    }

    // determine number of endpoints
    bool p1e = false;   // set to true if Point 1 is an endpoint
    bool p2e = false;   // set to true if Point 2 is an endpoint

    do
    {
        // check for endpoints
        list<MCAD_INTERSECT>::iterator iIn = intersects.begin();
        list<MCAD_INTERSECT>::iterator eIn = intersects.end();

        while( iIn != eIn )
        {
            if( iIn->segA->GetSegType() != MCAD_SEGTYPE_CIRCLE )
            {
                if( !p1e && ( PointMatches( iList.front(), iIn->segA->GetStart(), 1e-8 )
                    || PointMatches( iList.front(), iIn->segA->GetEnd(), 1e-8 ) ) )
                {
                    p1e = true;
                }

                if( !p2e && ( PointMatches( iList.back(), iIn->segA->GetStart(), 1e-8 )
                    || PointMatches( iList.back(), iIn->segA->GetEnd(), 1e-8 ) ) )
                {
                    p2e = true;
                }
            }

            ++iIn;
        }
    } while( 0 );

    // we can adjust the entity using the given circle; determine which section of the
    // circle is outside (add) or inside (subtract) the outline
    MCAD_POINT p0 = aCircle->mcenter;
    MCAD_POINT p1 = iList.front();

    double a1 = atan2( p1.y - p0.y, p1.x - p0.x );
    p1 = iList.back();
    double a2 = atan2( p1.y - p0.y, p1.x - p0.x );

    if( a2 < a1 )
        a2 += 2.0 * M_PI;

    double a3 = (a1 + a2) / 2.0;

    MCAD_POINT pX;  // a point midway along the 2nd CCW section of the circle
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

    // test if the other half of the circle is inside or outside
    do
    {
        a3 += M_PI;
        pX.x = p0.x + aCircle->mradius * cos( a3 );
        pX.y = p0.y + aCircle->mradius * sin( a3 );
        bool isIn2 = IsInside( pX, error );

        if( !isIn2 && error )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[INFO] IsInside() failed; see previous messages";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            return false;
        }

        if( isIn2 == isIn )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[INFO] Invalid geometry: circular arc is coincident with but ";

            if( isIn )
                msg << "inside ";
            else
                msg << "outside ";

            msg << "the outline";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            return false;
        }

    } while( 0 );

    MCAD_POINT pF[2];   // final point order
    bool isEnd[2];      // indicates if pF[n] is an endpoint
    list<MCAD_SEGMENT*>::iterator pSeg[2]; // segment iterators associated with each point

    // note: The OUT(/IN) segment must be put in CCW(/CW) order and
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

    if( isIn )
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

    MCAD_SEGMENT* sp = new MCAD_SEGMENT;

    if( !sp->SetParams( p0, pF[0], pF[1], opsub ) )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] intersections do not lie on the circular arc";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        cerr << "  c";
        PrintPoint( p0 );
        cerr << "  s";
        PrintPoint( pF[0] );
        cerr << "  e";
        PrintPoint( pF[1] );
        delete sp;
        return false;
    }

    if( msegments.front()->GetSegType() == MCAD_SEGTYPE_CIRCLE )
    {
        // Special case: this outline is currently a circle
        iList.clear();
        iList.push_back(pF[0]);
        iList.push_back(pF[1]);
        list<MCAD_SEGMENT*> sList;

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
            iList.clear();
            iList.push_back(pF[0]);
            iList.push_back(pF[1]);
            list<MCAD_SEGMENT*> sList;

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
            list<MCAD_SEGMENT*>::iterator pS0 = pSeg[0];
            msegments.insert( ++pS0, sList.begin(), sList.end() );

            return true;
        }
    }

    // perform the splits
    for( int i = 0; i < 2; ++ i )
    {
        if( !isEnd[i] )
        {
            // split here
            iList.clear();
            iList.push_back(pF[i]);
            list<MCAD_SEGMENT*> sList;

            if( !(*pSeg[i])->Split(iList, sList) )
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] could not split segment";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                error = true;
                cerr << "Segment to be split:\n";
                PrintSeg(*pSeg[i]);
                cerr << "Split point v";
                PrintPoint(pF[i]);
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
            list<MCAD_SEGMENT*>::iterator pS0 = pSeg[i];
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

    list<MCAD_SEGMENT*>::iterator tSeg = pSeg[0];
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

    msegments.insert( tSeg, sp );
    return true;
}   // opOutline( MCAD_SEGMENT* aCircle, bool& error, bool opsub )


// operate on the generic outline (add/subtract)
bool MCAD_OUTLINE::opOutline( MCAD_OUTLINE* aOutline, bool& error, bool opsub )
{
    // Implementation:
    // a. Determine intersection points on *this and aOutline
    //    and ensure that if there is an intersection that it
    //    is at 2 unique points.
    // b. Split *this at the unique points; for each point
    //    retain an iterator to the segment whose 'mstart'
    //    is the split point.
    // c. Split aOutline at the unique points; for each point
    //    retain an iterator to the segment whose 'mstart'
    //    is the split point.
    // d. Take a point along the segment in *this which was
    //    associated with the split point and determine whether
    //    it is inside or outside aOutline.
    // e. Take a point along the segment in aOutline which was
    //    associated with the split point and determine whether
    //    it is inside or outside *this.
    // f. A test for invalid geometry: if both (d) and (e) are
    //    inside (or outside) then we have invalid geometry due
    //    to a case of 2 non-overlapping outlines coinciding at
    //    2 points.
    // g. Trimming:
    //    1. Subtraction: determine the iterators for the aOutline
    //       segments "inside" *this and the iterators for the
    //       *this segments "outside" aOutline. Trim away the
    //       "outside" segments of aOutline and the "inside" segments
    //       of *this. Starting at the CCW point of the remaining
    //       segments in *this, insert the remaining segments of
    //       aOutline starting at the CW-most position and with each
    //       segment reversed.
    //    2. Addition: similar to subtraction, but eliminate the
    //       "inside" segments of aOutline and add them to the
    //       remaining segments of *this proceeding in the normal
    //       CCW order along aOutline.
    //

    mBBisOK = false;

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

    if( NULL == aOutline )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] NULL pointer passed for argument";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( !aOutline->IsClosed() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] argument outline is not closed";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    // XXX - TO BE IMPLEMENTED:
    // in the special case of the aOutline containing a
    // circle, attempt to add/subtract the circle and
    // if successful then destroy the aOutline container.

    error = false;
    list<MCAD_INTERSECT> intersects;
    list<MCAD_POINT> iList;
    list<MCAD_SEGMENT*>::iterator iSeg = msegments.begin();
    list<MCAD_SEGMENT*>::iterator eSeg = msegments.end();
    MCAD_INTERSECT_FLAG flag;

    int acc = 1;    // XXX - DEBUG
    while( iSeg != eSeg )
    {
        flag = MCAD_IFLAG_NONE;
        iList.clear();

        list<MCAD_SEGMENT*>::iterator sO = aOutline->msegments.begin();
        list<MCAD_SEGMENT*>::iterator eO = aOutline->msegments.end();

        while( sO != eO )
        {
            if( (*iSeg)->GetIntersections( **sO, iList, flag ) )
            {
                if( MCAD_IFLAG_NONE != flag && MCAD_IFLAG_ENDPOINT != flag
                    && MCAD_IFLAG_TANGENT != flag )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[INFO] flag was set on intersect: " << flag << " (treated as invalid geom.)";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }

                std::list<MCAD_POINT>::iterator iPts = iList.begin();
                std::list<MCAD_POINT>::iterator ePts = iList.end();

                while( iPts != ePts )
                {
                    MCAD_INTERSECT gi;
                    gi.vertex = *iPts;
                    gi.segA = *iSeg;
                    gi.segB = *sO;
                    gi.iSegA = iSeg;
                    gi.iSegB = sO;
                    intersects.push_back( gi );
                    ++iPts;
                }
            }
            else
            {
                if( MCAD_IFLAG_NONE != flag )
                {
                    if( opsub && MCAD_IFLAG_ENCIRCLES == flag )
                    {
                        // we have a circle within a circle which is
                        // valid geometry in this case but there is
                        // no intersection
                        flag = MCAD_IFLAG_NONE;
                        return false;
                    }

                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[INFO] invalid geometry: flag = " << flag;
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }
            }

            ++sO;
        }

        ++acc;
        ++iSeg;
    }

    // Possible number of *distinct* intersections:
    // a. 0: all is good, no intersection
    // b. 1: bad geometry, intersection at a point
    // c. 2: we can adjust the outline
    // d. 3 or more: bad geometry: violates 2-point
    //       restriction on intersections.

    if( intersects.empty() )
        return false;

    iList.clear();
    list<list<MCAD_SEGMENT*>::iterator> lSegs;
    list<list<MCAD_SEGMENT*>::iterator> oSegs;

    // compute the number of unique intersecting points:
    list<MCAD_INTERSECT>::iterator iIn = intersects.begin();
    list<MCAD_INTERSECT>::iterator eIn = intersects.end();

    while( iIn != eIn )
    {
        if( iList.empty() )
        {
            iList.push_back( iIn->vertex );
            lSegs.push_back( iIn->iSegA );
            oSegs.push_back( iIn->iSegB );
        }
        else
        {
            std::list<MCAD_POINT>::iterator iPts = iList.begin();
            std::list<MCAD_POINT>::iterator ePts = iList.end();
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
                oSegs.push_back( iIn->iSegB );
            }
        }

        ++iIn;
    }

    if( iList.size() != 2 )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] invalid geometry: violates restriction of 2 unique intersections (n = ";
        msg << iList.size() << ")";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        PrintGeomIntersects( intersects );
        return false;
    }

    // split *this
    bool p1e = false;   // set to true if Point 1 is an endpoint
    bool p2e = false;   // set to true if Point 2 is an endpoint

    if( ( MCAD_SEGTYPE_CIRCLE != (*lSegs.front())->GetSegType() )
        && ( PointMatches( iList.front(), (*lSegs.front())->mstart, 1e-8 )
        || PointMatches( iList.front(), (*lSegs.front())->mend, 1e-8 ) ) )
    {
        p1e = true;
    }

    if( ( MCAD_SEGTYPE_CIRCLE != (*lSegs.back())->GetSegType() )
        && ( PointMatches( iList.back(), (*lSegs.back())->mstart, 1e-8 )
        || PointMatches( iList.back(), (*lSegs.back())->mend, 1e-8 ) ) )
    {
        p2e = true;
    }

    if( !p1e && !p2e && *lSegs.front() == *lSegs.back() )
    {
        // we are splitting a single entity at 2 points
        list<MCAD_SEGMENT*> sList;

        if( !(*lSegs.front())->Split(iList, sList) )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[BUG] could not split *this outline";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }

        list<MCAD_SEGMENT*>::iterator sSL = sList.begin();
        list<MCAD_SEGMENT*>::iterator eSL = sList.end();
        list<MCAD_SEGMENT*>::iterator ps0 = lSegs.front();
        msegments.insert( ++ps0, sSL, eSL );
    }
    else
    {
        // we are splitting individual entities at single points
        list<list<MCAD_SEGMENT*>::iterator>::iterator sSegs = lSegs.begin();
        list<list<MCAD_SEGMENT*>::iterator>::iterator eSegs = lSegs.end();
        list<MCAD_POINT>::iterator iPts = iList.begin();

        while( sSegs != eSegs )
        {
            MCAD_SEGMENT* pSeg = **sSegs;

            if( !PointMatches( *iPts, pSeg->mstart, 1e-8 )
                && !PointMatches( *iPts, pSeg->mend, 1e-8 ) )
            {
                // this is not an endpoint; split the entity
                list<MCAD_SEGMENT*> sList;
                list<MCAD_POINT> pl;
                pl.push_back( *iPts );

                if( !pSeg->Split(pl, sList) )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[BUG] could not split *this outline";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }

                list<MCAD_SEGMENT*>::iterator ps0 = *sSegs;
                msegments.insert( ++ps0, sList.front() );
            }

            ++iPts;
            ++sSegs;
        }
    }

    // split aOutline
    p1e = false;
    p2e = false;

    if( ( MCAD_SEGTYPE_CIRCLE != (*oSegs.front())->GetSegType() )
        && ( PointMatches( iList.front(), (*oSegs.front())->mstart, 1e-8 )
        || PointMatches( iList.front(), (*oSegs.front())->mend, 1e-8 ) ) )
    {
        p1e = true;
    }

    if( ( MCAD_SEGTYPE_CIRCLE != (*oSegs.back())->GetSegType() )
        && ( PointMatches( iList.back(), (*oSegs.back())->mstart, 1e-8 )
        || PointMatches( iList.back(), (*oSegs.back())->mend, 1e-8 ) ) )
    {
        p2e = true;
    }

    if( !p1e && !p2e && *oSegs.front() == *oSegs.back() )
    {
        // we are splitting a single entity at 2 points
        list<MCAD_SEGMENT*> sList;

        if( !(*oSegs.front())->Split(iList, sList) )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[BUG] could not split aOutline";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }

        list<MCAD_SEGMENT*>::iterator sSL = sList.begin();
        list<MCAD_SEGMENT*>::iterator eSL = sList.end();
        list<MCAD_SEGMENT*>::iterator ps0 = oSegs.front();
        aOutline->msegments.insert( ++ps0, sSL, eSL );
    }
    else
    {
        // we are splitting individual entities at single points
        list<list<MCAD_SEGMENT*>::iterator>::iterator sSegs = oSegs.begin();
        list<list<MCAD_SEGMENT*>::iterator>::iterator eSegs = oSegs.end();
        list<MCAD_POINT>::iterator iPts = iList.begin();

        while( sSegs != eSegs )
        {
            MCAD_SEGMENT* pSeg = **sSegs;

            if( !PointMatches( *iPts, pSeg->mstart, 1e-8 )
                && !PointMatches( *iPts, pSeg->mend, 1e-8 ) )
            {
                // this is not an endpoint; split the entity
                list<MCAD_SEGMENT*> sList;
                list<MCAD_POINT> pl;
                pl.push_back( *iPts );

                if( !pSeg->Split(pl, sList) )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[BUG] could not split *this outline";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }

                list<MCAD_SEGMENT*>::iterator ps0 = *sSegs;
                aOutline->msegments.insert( ++ps0, sList.front() );
            }

            ++iPts;
            ++sSegs;
        }
    }

    // d. Take a point along the segment in *this which was
    //    associated with the split point and determine whether
    //    it is inside or outside aOutline.
    // e. Take a point along the segment in aOutline which was
    //    associated with the split point and determine whether
    //    it is inside or outside *this.

    MCAD_POINT pT;  // test point to use in determining if a segment is
                    // inside or outside a closed region

    // test if CCW point from the split point on *this is in or out
    if( PointMatches( iList.front(), (*lSegs.front())->mstart, 1e-8 ) )
    {
        (*lSegs.front())->GetMidpoint( pT );
    }
    else
    {
        list<MCAD_SEGMENT*>::iterator sSL = lSegs.front();
        ++sSL;

        if( sSL == msegments.end() )
            sSL = msegments.begin();

        if( PointMatches( iList.front(), (*sSL)->mstart, 1e-8 ) )
        {
            (*sSL)->GetMidpoint( pT );
            lSegs.front() = sSL;
        }
        else
        {
            ++sSL;

            if( sSL == msegments.end() )
                sSL = msegments.begin();

            if( PointMatches( iList.front(), (*sSL)->mstart, 1e-8 ) )
            {
                (*sSL)->GetMidpoint( pT );
                lSegs.front() = sSL;
            }
            else
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] cannot find *this segment starting at split point 0";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                return false;
            }
        }
    }

    bool tpIn0 = aOutline->IsInside( pT, error );

    if( !tpIn0 && error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] cannot test if a point is inside aOutline";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    // test if CCW point from the split point on aOutline is in or out
    if( PointMatches( iList.front(), (*oSegs.front())->mstart, 1e-8 ) )
    {
        (*oSegs.front())->GetMidpoint( pT );
    }
    else
    {
        list<MCAD_SEGMENT*>::iterator sSL = oSegs.front();
        ++sSL;

        if( sSL == aOutline->msegments.end() )
            sSL = aOutline->msegments.begin();

        if( PointMatches( iList.front(), (*sSL)->mstart, 1e-8 ) )
        {
            (*sSL)->GetMidpoint( pT );
            oSegs.front() = sSL;
        }
        else
        {
            ++sSL;

            if( sSL == aOutline->msegments.end() )
                sSL = aOutline->msegments.begin();

            if( PointMatches( iList.front(), (*sSL)->mstart, 1e-8 ) )
            {
                (*sSL)->GetMidpoint( pT );
                oSegs.front() = sSL;
            }
            else
            {
                ostringstream msg;
                GEOM_ERR( msg );
                msg << "[BUG] cannot find aOutline segment starting at split point 0";
                ERRMSG << msg.str() << "\n";
                errors.push_back( msg.str() );
                return false;
            }
        }
    }

    bool tpIn1 = IsInside( pT, error );

    if( !tpIn1 && error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] cannot test if a point is inside *this";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    // f. A test for invalid geometry: if both (d) and (e) are
    //    inside (or outside) then we have invalid geometry due
    //    to a case of 2 non-overlapping outlines coinciding at
    //    2 points.
    if( tpIn0 == tpIn1 )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] invalid geometry (non-overlapping regions coincident at 2 points)";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    // g. Trimming:
    //    1. Subtraction: determine the iterators for the aOutline
    //       segments "inside" *this and the iterators for the
    //       *this segments "outside" aOutline. Trim away the
    //       "outside" segments of aOutline and the "inside" segments
    //       of *this. Starting at the CCW point of the remaining
    //       segments in *this, insert the remaining segments of
    //       aOutline starting at the CW-most position and with each
    //       segment reversed.
    //    2. Addition: similar to subtraction, but eliminate the
    //       "inside" segments of aOutline and add them to the
    //       remaining segments of *this proceeding in the normal
    //       CCW order along aOutline.
    //

    // delete inside segments of *this
    // note: lSegs.front() must point to the first CW segment
    list<MCAD_SEGMENT*>::iterator eSegT = lSegs.front();

    if( tpIn0 )
    {
        while( true )
        {
            if( msegments.end() == eSegT )
            {
                eSegT = msegments.begin();

                if( msegments.empty() )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[BUG] deleted entire outline";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }
            }

            if( PointMatches( (*eSegT)->mstart, iList.back(), 1e-8 ) )
                break;

            delete *eSegT;
            eSegT = msegments.erase( eSegT );
        }

        lSegs.front() = eSegT;
    }
    else
    {
        list<MCAD_SEGMENT*> tSegs;

        while( true )
        {
            if( msegments.end() == eSegT )
            {
                eSegT = msegments.begin();

                if( msegments.empty() )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[BUG] deleted entire outline";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }
            }

            if( PointMatches( (*eSegT)->mstart, iList.back(), 1e-8 ) )
                break;

            tSegs.push_back( *eSegT );
            eSegT = msegments.erase( eSegT );
        }

        while( !msegments.empty() )
        {
            delete msegments.front();
            msegments.pop_front();
        }

        msegments = tSegs;
        lSegs.front() = msegments.begin();
    }

    // trim the applied outline
    list<MCAD_SEGMENT*>::iterator eSegO = oSegs.front();

    // note: oSegs.front() must point to the first CW segment
    // and eSeg0 must point to the first CCW segment
    if( ( tpIn1 && !opsub ) || ( opsub && !tpIn1 ) )
    {
        while( true )
        {
            if( aOutline->msegments.end() == eSegO )
            {
                eSegO = aOutline->msegments.begin();

                if( aOutline->msegments.empty() )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[BUG] deleted entire outline";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }
            }

            if( PointMatches( (*eSegO)->mstart, iList.back(), 1e-8 ) )
                break;

            delete *eSegO;
            eSegO = aOutline->msegments.erase( eSegO );
        }

        oSegs.front() = eSegO;

        if( aOutline->msegments.begin() == eSegO )
            eSegO = --aOutline->msegments.end();
        else
            --eSegO;

    }
    else
    {
        // store elements starting at oSegs.front()
        list<MCAD_SEGMENT*> tSegs;

        while( true )
        {
            if( aOutline->msegments.end() == eSegO )
            {
                eSegO = aOutline->msegments.begin();

                if( aOutline->msegments.empty() )
                {
                    ostringstream msg;
                    GEOM_ERR( msg );
                    msg << "[BUG] deleted entire outline";
                    ERRMSG << msg.str() << "\n";
                    errors.push_back( msg.str() );
                    error = true;
                    return false;
                }
            }

            if( PointMatches( (*eSegO)->mstart, iList.back(), 1e-8 ) )
                break;

            tSegs.push_back( *eSegO );
            eSegO = aOutline->msegments.erase( eSegO );
        }

        while( !aOutline->msegments.empty() )
        {
            delete aOutline->msegments.front();
            aOutline->msegments.pop_front();
        }

        aOutline->msegments = tSegs;
        oSegs.front() = --aOutline->msegments.end();
        eSegO = --aOutline->msegments.end();
    }

    // stitch partial outlines together
    if( opsub )
    {
        // insert the remaining segments of aOutline starting at
        // the CCW-most position and with each aOutline segment
        // reversed and in the reverse order within the list
        list<MCAD_SEGMENT*>::iterator eT = oSegs.front();
        list<MCAD_SEGMENT*>::iterator qT;

        if( eT == aOutline->msegments.begin() )
            eT = --aOutline->msegments.end();
        else
            --eT;

        eSegT = lSegs.front();

        while( !aOutline->msegments.empty() )
        {
            if( aOutline->msegments.size() > 1 )
            {
                if( eT == aOutline->msegments.begin() )
                    qT = aOutline->msegments.end();
                else
                    qT = eT;

                --qT;
            }
            else
            {
                qT = aOutline->msegments.end();
            }

            (*eT)->reverse();
            msegments.insert( eSegT, *eT );
            aOutline->msegments.erase( eT );
            eT = qT;
        }

        delete aOutline;
        return true;
    }
    else
    {
        // insert the remaining segments of aOutline starting at
        // the CCW-most position
        list<MCAD_SEGMENT*>::iterator eT = oSegs.front();

        eSegT = lSegs.front();
        MCAD_POINT lastPt;

        do
        {
            list<MCAD_SEGMENT*>::iterator tmpT = eSegT;

            if( tmpT == msegments.begin() )
                tmpT = --msegments.end();
            else
                --tmpT;

            lastPt = (*tmpT)->GetEnd();

        } while(0);

        // ensure that we start with the segment whose Start Point matches lastPt;
        // this maintains the correct segment order.
        while( !PointMatches( (*eT)->GetStart(), lastPt, 1e-8 ) )
        {
            ++eT;

            if( aOutline->msegments.end() == eT )
                eT = aOutline->msegments.begin();

        }

        while( eT != aOutline->msegments.end() )
        {
            msegments.insert( eSegT, *eT );
            eT = aOutline->msegments.erase( eT );
        }

        if( !aOutline->msegments.empty() )
        {
            eT = aOutline->msegments.begin();

            while( eT != aOutline->msegments.end() )
            {
                msegments.insert( eSegT, *eT );
                ++eT;
            }

            aOutline->msegments.clear();
        }

        delete aOutline;
    }

    return true;
}   // opOutline( MCAD_OUTLINE* aOutline, bool& error, bool opsub )


// Merge the given closed outline with this one; to keep the
// code simple, the following restriction is imposed:
// the two outlines may only intersect at 2 points.
bool MCAD_OUTLINE::AddOutline( MCAD_OUTLINE* aOutline, bool& error )
{
    bool res = opOutline( aOutline, error, false );

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] see above messages";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    return res;
}


bool MCAD_OUTLINE::AddOutline( MCAD_SEGMENT* aCircle, bool& error )
{
    bool res = opOutline( aCircle, error, false );

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] see above messages";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    if( res )
        delete aCircle;

    return res;
}   // AddOutline( MCAD_SEGMENT* aCircle, bool& error )


// Subtract the given circular segment from this outline; to keep the
// code simple, the following restriction is imposed:
// the two outlines may only intersect at 2 points.
bool MCAD_OUTLINE::SubOutline( MCAD_SEGMENT* aCircle, bool& error )
{
    bool res = opOutline( aCircle, error, true );

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] see above messages";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    if( res )
        delete aCircle;

    return res;
}   // SubOutline( MCAD_SEGMENT* aCircle, bool& error )


// Subtract the given outline from this one; to keep the
// code simple, the following restriction is imposed:
// the two outlines may only intersect at 2 points.
bool MCAD_OUTLINE::SubOutline( MCAD_OUTLINE* aOutline, bool& error )
{
    bool res = opOutline( aOutline, error, true );

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[INFO] see above messages";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    return res;
}


// Add the given cutout in preparation for exporting a solid model.
// If the cutout is known to be non-overlapping then the 'overlaps'
// flag may be set to 'false' to skip overlap tests. If the user
// does not know whether the outline overlaps or not, then the
// overlaps flag must be set to 'true' to ensure that checks are
// performed to ensure valid geometry.
bool MCAD_OUTLINE::AddCutout( MCAD_OUTLINE* aCutout, bool overlaps, bool& error )
{
    error = false;

    if( NULL == aCutout )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( !aCutout->IsClosed() )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[BUG] cutout is not a closed loop";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( !overlaps )
    {
        mcutouts.push_back( aCutout );
        return true;
    }

    if( SubOutline( aCutout, error ) )
        return true;

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] cannot add cutout to main outline";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    // check for overlaps with internal cutouts
    list<MCAD_OUTLINE*>::iterator sC = mcutouts.begin();
    list<MCAD_OUTLINE*>::iterator eC = mcutouts.end();

    while( sC != eC )
    {
        if( (*sC)->AddOutline( aCutout, error ) )
            return true;

        if( error )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[ERROR] could not apply cutout";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            return false;
        }

        ++sC;
    }

    mcutouts.push_back( aCutout );
    return true;
}

// Add the given circular segment as a cutout; if the segment is
// known to be non-overlapping then 'overlaps' may be set to 'false',
// otherwise it must be set to 'true'. If the function succeeds it
// will manage the given segment; if the function returns false
// then it is the caller's responsibility to dispose of the object.
bool MCAD_OUTLINE::AddCutout( MCAD_SEGMENT* aCircle, bool overlaps, bool& error )
{
    error = false;

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

    if( MCAD_SEGTYPE_CIRCLE != aCircle->GetSegType() )
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
        mholes.push_back( aCircle );
        return true;
    }

    if( SubOutline( aCircle, error ) )
        return true;

    if( error )
    {
        ostringstream msg;
        GEOM_ERR( msg );
        msg << "[ERROR] could not apply cutout to main outline";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    // check for overlaps with internal cutouts
    list<MCAD_OUTLINE*>::iterator sC = mcutouts.begin();
    list<MCAD_OUTLINE*>::iterator eC = mcutouts.end();

    while( sC != eC )
    {
        if( (*sC)->AddOutline( aCircle, error ) )
            return true;

        if( error )
        {
            ostringstream msg;
            GEOM_ERR( msg );
            msg << "[ERROR] could not apply cutout";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            return false;
        }

        ++sC;
    }

    mholes.push_back( aCircle );
    return true;
}


void MCAD_OUTLINE::calcBoundingBox( void )
{
    if( msegments.empty() || !mIsClosed )
        return;

    list<MCAD_SEGMENT*>::iterator sSeg = msegments.begin();
    list<MCAD_SEGMENT*>::iterator eSeg = msegments.end();

    MCAD_POINT bb0;
    MCAD_POINT bb1;
    (*sSeg)->GetBoundingBox( mBottomLeft, mTopRight );
    ++sSeg;

    while( sSeg != eSeg )
    {
        (*sSeg)->GetBoundingBox( bb0, bb1 );

        if( bb0.x < mBottomLeft.x )
            mBottomLeft.x = bb0.x;

        if( bb0.y < mBottomLeft.y )
            mBottomLeft.y = bb0.y;

        if( bb1.x > mTopRight.x )
            mTopRight.x = bb1.x;

        if( bb1.y > mTopRight.y )
            mTopRight.y = bb1.y;

        ++sSeg;
    }

    mBBisOK = true;
    adjustBoundingBox();
    return;
}


void MCAD_OUTLINE::adjustBoundingBox( void )
{
    double minX = floor( mBottomLeft.x );
    double maxX = ceil( mTopRight.x );

    double minY = floor( mBottomLeft.y );
    double maxY = ceil( mTopRight.y );

    int dx = (int)maxX - (int)minX;
    int dy = (int)maxY - (int)minY;

    if( dx % 2 )
        dx += 1;

    if( dy % 2 )
        dy += 1;

    // Ensure that the scaling remains the same in both dimensions
    // otherwise Curve on Surface calculations will become distorted.
    double dv = max( dx, dy );

    mBottomLeft.x = minX;
    mBottomLeft.y = minY;
    mTopRight.x = minX + dv;
    mTopRight.y = minY + dv;

    return;
}


std::list<MCAD_SEGMENT*>* MCAD_OUTLINE::GetSegments( void )
{
    return &msegments;
}


std::list<MCAD_OUTLINE*>* MCAD_OUTLINE::GetCutouts( void )
{
    return &mcutouts;
}


std::list<MCAD_SEGMENT*>* MCAD_OUTLINE::GetDrillHoles( void )
{
    return &mholes;
}
