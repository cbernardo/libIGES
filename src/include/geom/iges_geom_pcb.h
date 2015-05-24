/*
 * file: iges_geom_pcb.h
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

#ifndef IGES_GEOM_OUTLINE_H
#define IGES_GEOM_OUTLINE_H

#include <list>
#include <string>
#include <mcad_outline.h>

class IGES_GEOM_PCB : public MCAD_OUTLINE
{
private:
    // routines to create curves suitable as subordinates to the CPTR of
    // a trimmed parametric surface
    bool getCurveCircle( IGES* aModel, std::list<IGES_CURVE*>& aCurves,
                         double zHeight, MCAD_SEGMENT* aSegment );
    bool getCurveArc( IGES* aModel, std::list<IGES_CURVE*>& aCurves,
                      double zHeight, MCAD_SEGMENT* aSegment );
    bool getCurveLine( IGES* aModel, std::list<IGES_CURVE*>& aCurves,
                       double zHeight, MCAD_SEGMENT* aSegment );

    // routines to create curves suitable as subordinates to the BPTR of
    // a trimmed parametric surface
    bool copCircle( IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                    double offX, double offY, double aScale,
                    double zHeight, MCAD_SEGMENT* aSegment );
    bool copArc( IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                 double offX, double offY, double aScale,
                 double zHeight, MCAD_SEGMENT* aSegment );
    bool copLine( IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                  double offX, double offY, double aScale,
                  double zHeight, MCAD_SEGMENT* aSegment );

protected:
   // create a Trimmed Parametric Surface entity with only the PTS member instantiated
   IGES_ENTITY_144* getUntrimmedPlane( IGES* aModel, double aHeight );

public:
    IGES_GEOM_PCB();
    virtual ~IGES_GEOM_PCB();

    // retrieve trimmed parametric surfaces representing vertical sides
    // of the main outline and all cutouts
    bool GetVerticalSurface( IGES* aModel, bool& error,
                             std::vector<IGES_ENTITY_144*>& aSurface,
                             double aTopZ, double aBotZ );

    // retrieve the trimmed parametric surfaces representing the
    // top or bottom plane of the board
    bool GetTrimmedPlane( IGES* aModel, bool& error,
                          std::vector<IGES_ENTITY_144*>& aSurface,
                          double aHeight );

    // retrieve the representation of the curve as IGES
    // 2D primitives (Entity 100 or Entity 110). An arc
    // or circle is automatically divided into multiple
    // segments for the consumption of MCADs.
    bool GetCurves( IGES* aModel, std::list<IGES_CURVE*>& aCurves,
                    double zHeight, MCAD_SEGMENT* aSegment );

    // retrieve the curve as a list of parametric curves on plane; it is
    // assumed that the plane's first parameter is along X and the second
    // along Y. The segment is converted to parameter space assuming uniform
    // parameter values U:(0..1), V:(0..1) and the given X,Y values specifying
    // the plane. Arcs shall automatically be broken into segments as necessary
    // to ensure a 1:1 mapping within each segment.
    // Restrictions:
    // 1. The plane must have the same coordinate system as the segment.
    // 2. The plane must encompass all points in the curve or else the
    //    operation shall fail since an out-of-bounds parameter would be
    //    calculated.
    bool GetCurveOnPlane(  IGES* aModel, std::list<IGES_ENTITY_126*>& aCurves,
                           double aMinX, double aMaxX, double aMinY, double aMaxY,
                           double zHeight, MCAD_SEGMENT* aSegment );

    // retrieve a trimmed parametric surface representing a vertical side
    bool GetSegmentWall( IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                         double aTopZ, double aBotZ, MCAD_SEGMENT* aSegment );
};

#endif  // IGES_GEOM_PCB_H
