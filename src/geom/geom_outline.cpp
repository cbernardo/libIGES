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


// Add a segment to this outline; the user must ensure that
// the outline is closed before performing any other type
// of operation.
bool IGES_GEOM_OUTLINE::AddSegment( IGES_GEOM_SEGMENT* aSegment, bool& error )
{
    if( NULL == aSegment )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( IGES_SEGTYPE_NONE == aSegment->getSegType() )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] invalid segment type";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( mIsClosed )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[ERROR] outline is already closed";
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
            msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] a circle cannot be added to a non-empty outline";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }

        msegments.push_back( aSegment );
        mIsClosed = true;
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
            msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[ERROR] endpoints do not match within 1e-8";
            ERRMSG << msg.str() << "\n";
            errors.push_back( msg.str() );
            error = true;
            return false;
        }
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
    #warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    return false;

    if( NULL == aCircle )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( IGES_SEGTYPE_CIRCLE != aCircle->getSegType() )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] segment is not a circle";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    // XXX - TO BE IMPLEMENTED
    return false;
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
    #warning TO BE IMPLEMENTED
    // XXX - TO BE IMPLEMENTED
    return false;
    if( NULL == aCircle )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] NULL pointer";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        error = true;
        return false;
    }

    if( IGES_SEGTYPE_CIRCLE != aCircle->getSegType() )
    {
        ostringstream msg;
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[BUG] segment is not a circle";
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
        msg << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":" << "[ERROR] could not apply cutout";
        ERRMSG << msg.str() << "\n";
        errors.push_back( msg.str() );
        return false;
    }

    mholes.push_back( aCircle );
    return true;
}
