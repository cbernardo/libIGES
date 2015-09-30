/*
 * file: dll_mcad_segment.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: intermediate ECAD representation of a boundary
 * segment. This class aids in the creation of an IGES model
 * for the top and bottom surfaces of a PCB.
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

#ifndef DLL_MCAD_SEGMENT_H
#define DLL_MCAD_SEGMENT_H

#include <libigesconf.h>
#include <mcad_elements.h>


class MCAD_SEGMENT;

class DLL_MCAD_SEGMENT
{
private:
    MCAD_SEGMENT* m_segment;
    bool m_valid;

public:
    DLL_MCAD_SEGMENT( bool create );
    ~DLL_MCAD_SEGMENT();

    /**
     * Function IsValid
     * returns true if the object holds a valid MCAD_SEGMENT pointer
     */
    bool IsValid( void );

    // create a new segment type; if a segment already
    // exists then it is detached; if the user wishes to
    // destroy the segment then DelSegment() must be
    // invoked first
    bool NewSegment( void );

    // delete the currently associated segment
    void DelSegment( void );

    // detach from the wrapped segment; if the segment is not
    // detached upon destruction then the segment itself will
    // also be destroyed
    void Detach( void );

    // attach
    bool Attach( MCAD_SEGMENT* aSegment );

    // retrieve the raw pointer to the internal MCAD_SEGMENT object
    MCAD_SEGMENT* GetRawPtr();

    bool GetSegType( MCAD_SEGTYPE& aSegType ) const;
    bool GetRadius( double& aRadius ) const;
    bool GetStartAngle( double& aStartAngle ) const;
    bool GetEndAngle( double& aEndAngle ) const;
    bool GetFirstAngle( double& aFirstAngle ) const;
    bool GetLastAngle( double& aLastAngle ) const;
    bool GetCenter( MCAD_POINT& aPoint ) const;
    bool GetStart( MCAD_POINT& aPoint ) const;
    bool GetEnd( MCAD_POINT& aPoint ) const;
    bool GetFirstPoint( MCAD_POINT& aPoint ) const;
    bool GetLastPoint( MCAD_POINT& aPoint ) const;

    // set the parameters for a line
    bool SetParams( MCAD_POINT aStart, MCAD_POINT aEnd );

    // set the parameters for an arc; the parameters must be specified such that
    // the arc is traced in a counterclockwise direction as viewed from a positive
    // Z location.
    bool SetParams( MCAD_POINT aCenter, MCAD_POINT aStart, MCAD_POINT aEnd, bool isCW );

    bool GetLength( double& aLength );

    bool IsCW( bool& aResult );

    // + calculate intersections with another segment (list of points)
    // if aIntersectList returns with a non-NULL value then the user is
    // responsible for its destruction and the destruction of its contents
    bool GetIntersections( MCAD_SEGMENT const* aSegment, MCAD_POINT*& aIntersectList,
        int& aNumIntersections, MCAD_INTERSECT_FLAG& flags );

    bool GetIntersections( DLL_MCAD_SEGMENT& aSegment, MCAD_POINT*& aIntersectList,
        int& aNumIntersections, MCAD_INTERSECT_FLAG& flags );

    // + calculate the bottom-left and top-right rectangular bounds
    bool GetBoundingBox( MCAD_POINT& p0, MCAD_POINT& p1 );

    // + calculate the midpoint along the segment and return true;
    //   for circles the reported midpoint is the point to the right
    //   of the center.
    bool GetMidpoint( MCAD_POINT& p0 );

    // + split at the given list of intersections (1 or 2 intersections only)
    // Cases:
    //   1. Circle: must have 2 points; the circle is split into 2 arcs with
    //              the new arc being CCW from point 1 to point 2; the old
    //              circle segment becomes a CCW arc from point 2 to point 1.
    //   2. Arc: may be 1 or 2 points; up to 2 new arcs are created with the
    //           same sense (CCW or CW) as the original arc.
    //   3. Line: may be 1 or 2 points; new segments are added such that they
    //            maintain the order of the original paramaterized segment;
    //            that is, each new segment has a parameter value 't' on the
    //            original segment such that 't' is monotonically increasing
    //            from 0 .. 1.
    bool Split( MCAD_POINT** aIntersectList, int aNumIntersects,
        MCAD_SEGMENT**& aNewSegmentList, int& aNumNewSegs );
};

#endif  // DLL_MCAD_SEGMENT_H
