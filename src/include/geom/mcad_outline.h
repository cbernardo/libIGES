/*
 * file: mcad_outline.h
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

/*
 * NOTES:
 *
 * MESSAGES FOR THE USER:
 * + Operations may result in failures due to invalid geometry; this
 *   object must support a queue of messages in order to be able to
 *   pass meaningful messages onto users who are not using the
 *   software within a UNIX shell.
 *
 * OPERATIONS:
 * + AddSegment: adds another segment to the list; the added segment
 *   must have a start point which coincides with the terminal point
 *   of the previous segment.
 *
 * + AddOutline: the outline may be a GEOM_SEGMENT (circle only)
 *   or a GEOM_OUTLINE (generic outline). If the two outlines intersect
 *   then they are merged and the method returns TRUE, otherwise the
 *   method returns FALSE. If there is invalid geometry (the 2 outlines
 *   intersect at a point or at more than 2 points) the method returns
 *   FALSE and sets an appropriate flag. Intersection at more than 2 points
 *   implies either a floating island of material or a surface with a
 *   likely unintended cutout; in either case it is a serious mechanical
 *   design flaw and is prohibited without exceptions by this software.
 *
 * + SubOutline: the given outline (a GEOM_SEGMENT circle or a GEOM_OUTLINE)
 *   is cut out from *this outline if it intersects the edge and the method
 *   returns TRUE, otherwise the method returns FALSE. If there is invalid
 *   geometry (intersects at a point or at more than 2 points) the method
 *   returns FALSE and sets an appropriate flag. Intersection at more than
 *   2 points may imply that the outline will be split into at least 2
 *   separate bodies; this generally means very poor mechanical design so
 *   with this software it is prohibited without exception. It is possible that
 *   the main ouline will not be split into separate bodies but in such
 *   circumstances the user must work around the 2-point restriction by
 *   dividing the offending cutout into at least 2 separate bodies.
 */

#ifndef MCAD_OUTLINE_H
#define MCAD_OUTLINE_H

#include <list>
#include <string>
#include <libigesconf.h>

class MCAD_SEGMENT;

struct MCAD_INTERSECT
{
    MCAD_POINT vertex;
    MCAD_SEGMENT* segA;     // pointer to the segment operated upon
    MCAD_SEGMENT* segB;     // pointer to the segment modifying segA
    std::list<MCAD_SEGMENT*>::iterator iSegA;   // iterator to the segment operated upon
    std::list<MCAD_SEGMENT*>::iterator iSegB;   // iterator to the segment operation's argument, else = iSegA

    MCAD_INTERSECT()
    {
        segA = NULL;
        segB = NULL;
    }
};

class MCAD_OUTLINE
{
protected:
    std::list< std::string > errors;
    bool mIsClosed;     // true if the outline is closed
    double mWinding;    // accumulator to test for CW/CCW winding
    // operate on the circular outline (add/subtract)
    bool opOutline( MCAD_SEGMENT* aCircle, bool& error, bool opsub );
    // operate on the generic outline (add/subtract)
    bool opOutline( MCAD_OUTLINE* aOutline, bool& error, bool opsub );
    // recalculate the bounding box
    void calcBoundingBox( void );
    // adjust the bounding box in preparation for rendering a surface
    void adjustBoundingBox( void );
    bool mBBisOK;       // true if the bounding box has been calculated and
                        // no operations have been performed on the outline

    MCAD_POINT mBottomLeft; // bottom left coordinate
    MCAD_POINT mTopRight;   // top right coordinate of bounding box
    std::list<MCAD_SEGMENT*> msegments; // list of segments
    std::list<MCAD_OUTLINE*> mcutouts;  // list of non-overlapping cutouts
    std::list<MCAD_SEGMENT*> mholes;    // list of non-overlapping holes

public:
    MCAD_OUTLINE();
    virtual ~MCAD_OUTLINE();

    std::list<MCAD_SEGMENT*>* GetSegments( void );
    std::list<MCAD_OUTLINE*>* GetCutouts( void );
    std::list<MCAD_SEGMENT*>* GetDrillHoles( void );

    // Retrieve the error stack
    const std::list< std::string >* GetErrors( void );

    // Clear the error stack
    void ClearErrors( void );

    // Returns 'true' if the outline is closed
    bool IsClosed( void );

    // Returns 'true' if the (closed) outline is contiguous
    bool IsContiguous( void );

    // Returns 'true' if the point is on or inside this outline
    bool IsInside( MCAD_POINT aPoint, bool& error );

    // Add a segment to this outline; the user must ensure that
    // the outline is closed before performing any other type
    // of operation.
    bool AddSegment( MCAD_SEGMENT* aSegment, bool& error );

    // Merge the given closed outline with this one; to keep the
    // code simple, the following restriction is imposed:
    // the two outlines may only intersect at 2 points.
    bool AddOutline( MCAD_OUTLINE* aOutline, bool& error );

    // Merge the given circle with this outline
    bool AddOutline( MCAD_SEGMENT* aCircle, bool& error );

    // Subtract the given outline from this one; to keep the
    // code simple, the following restriction is imposed:
    // the two outlines may only intersect at 2 points.
    bool SubOutline( MCAD_OUTLINE* aOutline, bool& error );

    // Subtract the given circular segment from this outline; to keep the
    // code simple, the following restriction is imposed:
    // the two outlines may only intersect at 2 points.
    bool SubOutline( MCAD_SEGMENT* aCircle, bool& error );

    // Add the given cutout in preparation for exporting a solid model.
    // If the cutout is known to be non-overlapping then the 'overlaps'
    // flag may be set to 'false' to skip overlap tests. If the user
    // does not know whether the outline overlaps or not, then the
    // overlaps flag must be set to 'true' to ensure that checks are
    // performed to ensure valid geometry. If the function succeeds
    // it will manage the given cutout; if the function returns
    // false then the calling routine is responsible for the disposal
    // of the cutout object. It is the caller's responsibility to
    // ensure that the cutouts do not overlap with any other cutouts,
    // otherwise the geometry will be invalid.
    bool AddCutout( MCAD_OUTLINE* aCutout, bool overlaps, bool& error );

    // Add the given circular segment as a cutout; if the segment is
    // known to be non-overlapping then 'overlaps' may be set to 'false',
    // otherwise it must be set to 'true'. If the function succeeds it
    // will manage the given segment; if the function returns false
    // then it is the caller's responsibility to dispose of the object.
    // It is the caller's responsibility to ensure that the cutouts do not
    // overlap with any other cutouts, otherwise the geometry will be invalid.
    bool AddCutout( MCAD_SEGMENT* aCircle, bool overlaps, bool& error );

    // print routines for testing/debugging
    void PrintPoint( MCAD_POINT p0 );
    void PrintSeg( MCAD_SEGMENT* seg );
    void PrintGeomIntersects( const std::list<MCAD_INTERSECT>& aList );

};

#endif  // MCAD_OUTLINE_H
