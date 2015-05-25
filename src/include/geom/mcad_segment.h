/*
 * file: mcad_segment.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: intermediate ECAD representation of a boundary
 * segment. This class aids in the creation of an IGES model
 * for the top and bottom surfaces of a PCB.
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

#ifndef MCAD_SEGMENT_H
#define MCAD_SEGMENT_H

#include <list>
#include <vector>
#include <mcad_elements.h>

enum MCAD_SEGTYPE
{
    MCAD_SEGTYPE_NONE = 0,
    MCAD_SEGTYPE_LINE = 1,
    MCAD_SEGTYPE_ARC = 2,
    MCAD_SEGTYPE_CIRCLE = 4
};

// flag used for geometry intersection information
// Note that many of the cases in which we flag invalid geometry may in fact
// not be invalid geometry but the geom_* code is intended to help in an ECAD
// application rather than provide full MCAD support. This decision keeps this
// code simpler while forcing the ECAD designer to put more thought into the
// manufacturing design of the board.
enum MCAD_INTERSECT_FLAG
{
    MCAD_IFLAG_NONE = 0,    // no special conditions to report
    MCAD_IFLAG_ENDPOINT,    // intersection is at the endpoint of a segment
    MCAD_IFLAG_TANGENT,     // intersection is at a tangent (invalid geometry)
    MCAD_IFLAG_EDGE,        // intersection is along an edge; result contains
                            // start and end point of the edge. Initially the code
                            // shall enforce simple geometry so an EDGE flag
                            // shall be treated as invalid geometry.
    MCAD_IFLAG_INSIDE,      // this circle is inside the given circle (invalid geometry)
                            // or this arc is inside the given arc.
    MCAD_IFLAG_ENCIRCLES,   // this circle envelopes the given circle (invalid geometry)
    MCAD_IFLAG_OUTSIDE,     // this arc is outside the given arc
    MCAD_IFLAG_IDENT,       // 2 circles are identical
    MCAD_IFLAG_MULTIEDGE    // arcs overlap on 2 edges (invalid geometry)
};


class MCAD_SEGMENT
{
protected:
    friend class MCAD_OUTLINE;
    MCAD_SEGTYPE msegtype;  // segment type,
    double mradius;         // radius of arc or circle
    double msang;           // start angle of arc (always in CCW direction)
    double meang;           // end angle of arc (always in CCW direction)
    bool mCWArc;            // true if the arc is in the clockwise orientation

    MCAD_POINT mcenter;
    MCAD_POINT mstart;  // start point of arc; may be in CCW or CW direction
    MCAD_POINT mend;    // end point of arc; may be in CCW or CW direction

    void init( void );

    bool splitLine( std::list<MCAD_POINT>& aIntersectList,
                    std::list<MCAD_SEGMENT*>& aNewSegmentList );
    bool splitArc( std::list<MCAD_POINT>& aIntersectList,
                   std::list<MCAD_SEGMENT*>& aNewSegmentList );
    bool splitCircle( std::list<MCAD_POINT>& aIntersectList,
                      std::list<MCAD_SEGMENT*>& aNewSegmentList );

    // c2 = center of second circle
    // r2 = radius of second circle
    // d = distance between centers
    // p1 = first point of intersection, CW on c1 from 0 angle
    // p2 = second point of intersection, CW from p1 on c1
    void calcCircleIntercepts( MCAD_POINT c2, double r2, double d,
                               MCAD_POINT& p1, MCAD_POINT& p2 );

    // check case where both segments are circles
    bool checkCircles( const MCAD_SEGMENT& aSegment,
                       std::list<MCAD_POINT>& aIntersectList,
                       MCAD_INTERSECT_FLAG& flags );

    // check case where both segments are arcs (one may be a circle)
    bool checkArcs( const MCAD_SEGMENT& aSegment,
                    std::list<MCAD_POINT>& aIntersectList,
                    MCAD_INTERSECT_FLAG& flags );

    // check case where one segment is an arc and one a line
    bool checkArcLine( const MCAD_SEGMENT& aSegment,
                       std::list<MCAD_POINT>& aIntersectList,
                       MCAD_INTERSECT_FLAG& flags );

    // check case where both segments are lines
    bool checkLines( const MCAD_SEGMENT& aSegment,
                     std::list<MCAD_POINT>& aIntersectList,
                     MCAD_INTERSECT_FLAG& flags );

    // reverse the point order if applicable
    void reverse( void );

public:
    MCAD_SEGMENT();
    virtual ~MCAD_SEGMENT();

    MCAD_SEGTYPE GetSegType( void ) const;
    double GetRadius( void ) const;
    double GetStartAngle( void ) const;
    double GetEndAngle( void ) const;
    double GetMSAngle( void ) const;
    double GetMEAngle( void ) const;
    MCAD_POINT GetCenter( void ) const;
    MCAD_POINT GetStart( void ) const;
    MCAD_POINT GetEnd( void ) const;
    MCAD_POINT GetMStart( void ) const;
    MCAD_POINT GetMEnd( void ) const;

    // set the parameters for a line
    bool SetParams( MCAD_POINT aStart, MCAD_POINT aEnd );

    // set the parameters for an arc; the parameters must be specified such that
    // the arc is traced in a counterclockwise direction as viewed from a positive
    // Z location.
    bool SetParams( MCAD_POINT aCenter, MCAD_POINT aStart, MCAD_POINT aEnd, bool isCW );

    double GetLength( void );

    bool IsCW( void )
    {
        return mCWArc;
    }

    // + calculate intersections with another segment (list of points)
    bool GetIntersections( const MCAD_SEGMENT& aSegment,
                           std::list<MCAD_POINT>& aIntersectList,
                           MCAD_INTERSECT_FLAG& flags );

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
    bool Split( std::list<MCAD_POINT>& aIntersectList, std::list<MCAD_SEGMENT*>& aNewSegmentList );
};

#endif  // MCAD_SEGMENT_H
