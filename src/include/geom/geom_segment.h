/*
 * file: geom_segment.h
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

#ifndef IGES_GEOM_SEGMENT_H
#define IGES_GEOM_SEGMENT_H

#include <list>
#include <vector>
#include <iges_elements.h>
#include <iges_curve.h>
#include <entity126.h>
#include <entity144.h>

class IGES_GEOM_SEGMENT
{
private:
    char msegtype;  // segment type
    double mradius; // radius of arc or circle
    double msang;   // start angle of arc
    double meang;   // end angle of arc

    IGES_POINT mcenter;
    IGES_POINT mstart;
    IGES_POINT mend;

    void init( void );

public:
    IGES_GEOM_SEGMENT();
    ~IGES_GEOM_SEGMENT();

    // set the parameters for a line
    bool SetParams( IGES_POINT aStart, IGES_POINT aEnd );

    // set the parameters for an arc; the parameters must be specified such that
    // the arc is traced in a counterclockwise direction as viewed from a positive
    // Z location.
    bool SetParams( IGES_POINT aCenter, IGES_POINT aStart, IGES_POINT aEnd );

    // XXX - TO BE IMPLEMENTED
    // + calculate intersections with another segment (list of points)
    // + split at the given list of intersections (1 or 2 intersections only)

    // retrieve the representation of the curve as IGES
    // 2D primitives (Entity 100 or Entity 110). An arc
    // or circle is automatically divided into multiple
    // segments for the consumption of MCADs.
    bool GetCurves( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight );

    // retrieve the curve as a parametric curve on plane; it is
    // assumed that the plane's first parameter is along X and
    // the second along Y. The segment is converted to parameter
    // space assuming uniform parameter values U:(0..1), V:(0..1)
    // and the given X,Y values specifying the plane. Arcs shall
    // automatically be broken into segments as necessary to ensure
    // a 1:1 mapping within each segment.
    // Restrictions:
    // 1. The plane must have the same coordinate system as the segment.
    // 2. The plane must encompass all points in the curve or else the
    //    operation shall fail since an out-of-bounds parameter would be
    //    calculated.
    bool GetCurveOnPlane(  IGES* aModel, std::list<IGES_ENTITY_126*> aCurves,
        double aMinX, double aMaxX, double aMinY, double aMaxY,
        double zHeight );

    // retrieve a trimmed parametric surface representing a vertical side
    bool GetVerticalSurface( IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
        double aTopZ, double aBotZ );
};

#endif  // IGES_GEOM_SEGMENT_H
