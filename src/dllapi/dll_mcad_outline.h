/*
 * file: dll_mcad_outline.h
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

#ifndef DLL_MCAD_OUTLINE_H
#define DLL_MCAD_OUTLINE_H

#include <libigesconf.h>
#include <mcad_elements.h>
#include <dll_mcad_segment.h>

class MCAD_SEGMENT;
class MCAD_OUTLINE;

class MCAD_API DLL_MCAD_OUTLINE
{
protected:
    MCAD_OUTLINE* m_outline;
    bool m_valid;

public:
    DLL_MCAD_OUTLINE( bool create );
    virtual~DLL_MCAD_OUTLINE();

    /**
     * Function IsValid
     * returns true if the object holds a valid MCAD_SEGMENT pointer
     */
    bool IsValid( void );

    // create a new segment type; if a segment already
    // exists then it is detached; if the user wishes to
    // destroy the segment then DelSegment() must be
    // invoked first
    virtual bool NewOutline( void );

    // delete the currently associated segment
    void DelOutline( void );

    // detach from the wrapped segment; if the segment is not
    // detached upon destruction then the segment itself will
    // also be destroyed
    void Detach( void );

    // attach
    virtual bool Attach( MCAD_OUTLINE* aOutline );

    // retrieve the raw pointer to the internal MCAD_OUTLINE object
    MCAD_OUTLINE* GetRawPtr();

    // retrieve the segments currently in the main outline;
    // the user must delete [] aSegmentList when it is no longer needed
    bool GetSegments( MCAD_SEGMENT**& aSegmentList, int& aListSize );
    // retrieve the cutouts;
    // the user must delete [] aCutoutList when it is no longer needed
    bool GetCutouts( MCAD_OUTLINE**& aCutoutList, int& aListSize );
    // retrieve the circular holes;
    // the user must delete [] aDrillHoleList when it is no longer needed
    bool GetDrillHoles( MCAD_SEGMENT**& aDrillHoleList, int& aListSize );
    // retrieve the array of error messages;
    // the user must delete [] anErrorList when it is no longer needed
    bool GetErrors( char const**& anErrorList, int& aListSize );
    // clear all error messages
    void ClearErrors( void );
    // sets aResult to 'true' if the outline is closed
    bool IsClosed( bool& aResult );
    // sets aResult to 'true' if the (closed) outline is contiguous
    bool IsContiguous( bool& aResult );

    // sets aResult to 'true' if the point is on or inside this outline
    bool IsInside( MCAD_POINT aPoint, bool& error );

    // Add a segment to this outline; the user must ensure that
    // the outline is closed before performing any other type
    // of operation. On success the segment will be managed
    // by the outline object and should not be deleted by the
    // caller.
    bool AddSegment( MCAD_SEGMENT* aSegment, bool& error );
    bool AddSegment( DLL_MCAD_SEGMENT& aSegment, bool& error );

    // Merge the given closed outline with this one; to keep the
    // code simple, the following restriction is imposed:
    // the two outlines may only intersect at 2 points.
    // On success aOutline becomes part of this object's list
    // and the MCAD_OUTLINE container is automatically destroyed;
    // in the case of a DLL_MCAD_OUTLINE the user may reuse the
    // container. On failure the aOutline object may be corrupted
    // and in the case of an MCAD_OUTLINE the user should destroy it;
    // for a DLL_MCAD_OUTLINE the user may call DelOutline() and
    // reuse the container.
    bool AddOutline( MCAD_OUTLINE* aOutline, bool& error );
    bool AddOutline( DLL_MCAD_OUTLINE& aOutline, bool& error );

    // Merge the given circle with this outline
    // On success aCircle becomes part of this object's list
    // and the MCAD_SEGMENT container is automatically destroyed;
    // in the case of a DLL_MCAD_SEGMENT the user may reuse the
    // container. On failure aCircle remains unchanged and the user
    // is responsible for disposing of it.
    bool AddOutline( MCAD_SEGMENT* aCircle, bool& error );
    bool AddOutline( DLL_MCAD_SEGMENT& aCircle, bool& error );

    // Subtract the given outline from this one; to keep the
    // code simple, the following restriction is imposed:
    // the two outlines may only intersect at 2 points.
    // On success/failure behavior is similar to AddOutline( MCAD_OUTLINE* ... )
    bool SubOutline( MCAD_OUTLINE* aOutline, bool& error );
    bool SubOutline( DLL_MCAD_OUTLINE& aOutline, bool& error );

    // Subtract the given circular segment from this outline; to keep the
    // code simple, the following restriction is imposed:
    // the two outlines may only intersect at 2 points.
    // On success/failure behavior is similar to AddOutline( MCAD_SEGMENT* ... )
    bool SubOutline( MCAD_SEGMENT* aCircle, bool& error );
    bool SubOutline( DLL_MCAD_SEGMENT& aCircle, bool& error );

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
    bool AddCutout( DLL_MCAD_OUTLINE& aCutout, bool overlaps, bool& error );

    // Add the given circular segment as a cutout; if the segment is
    // known to be non-overlapping then 'overlaps' may be set to 'false',
    // otherwise it must be set to 'true'. If the function succeeds it
    // will manage the given segment; if the function returns false
    // then it is the caller's responsibility to dispose of the object.
    // It is the caller's responsibility to ensure that the cutouts do not
    // overlap with any other cutouts, otherwise the geometry will be invalid.
    bool AddCutout( MCAD_SEGMENT* aCircle, bool overlaps, bool& error );
    bool AddCutout( DLL_MCAD_SEGMENT& aCircle, bool overlaps, bool& error );

};

#endif  // DLL_MCAD_OUTLINE_H
