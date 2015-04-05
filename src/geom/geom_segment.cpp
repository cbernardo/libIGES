/*
 * file: geom_segment.cpp
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

#include <cmath>
#include <iges.h>
#include <error_macros.h>
#include <iges_helpers.h>
#include <geom_segment.h>
#include <geom_wall.h>
#include <geom_cylinder.h>

#define SEGTYPE_NONE 0
#define SEGTYPE_LINE 1
#define SEGTYPE_ARC 2
#define SEGTYPE_CIRCLE 4


using namespace std;


IGES_GEOM_SEGMENT::IGES_GEOM_SEGMENT()
{
    init();
}


IGES_GEOM_SEGMENT::~IGES_GEOM_SEGMENT()
{
    return;
}


void IGES_GEOM_SEGMENT::init( void )
{
    msegtype = SEGTYPE_NONE;
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


// set the parameters for a line
bool IGES_GEOM_SEGMENT::SetParams( IGES_POINT aStart, IGES_POINT aEnd )
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
    msegtype = SEGTYPE_LINE;
    return true;
}

// set the parameters for an arc; the parameters must be specified such that
// the arc is traced in a counterclockwise direction as viewed from a positive
// Z location.
bool IGES_GEOM_SEGMENT::SetParams( IGES_POINT aCenter, IGES_POINT aStart, IGES_POINT aEnd )
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
        msegtype = SEGTYPE_CIRCLE;
        mcenter = aCenter;
        mstart = mcenter;
        mstart.x += mradius;
        mend = mstart;
        return true;
    }

    dx = aEnd.x - aCenter.x;
    dy = aEnd.y - aCenter.y;
    double r2 = sqrt( dx*dx + dy*dy );

    if( abs(r2 - mradius) > 1.0e-3 )
    {
        mradius = 0;
        ERRMSG << "\n + [ERROR] radii differ by > 1e-3\n";
        return false;
    }

    msang = atan2( aStart.y - aCenter.y, aStart.x - aCenter.x );
    meang = atan2( aEnd.y - aCenter.y, aEnd.x - aCenter.x );

    while( meang < msang )
        meang += 2.0 * M_PI;

    mcenter = aCenter;
    mstart = aStart;
    mend = aEnd;

    mstart = aStart;
    mend = aEnd;
    msegtype = SEGTYPE_ARC;
    return true;
}


// retrieve the representation of the curve as IGES 2D primitives
bool IGES_GEOM_SEGMENT::GetCurves( IGES* aModel, std::list<IGES_CURVE*>& aCurves, double zHeight )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// retrieve the curve as a parametric curve on plane
bool IGES_GEOM_SEGMENT::GetCurveOnPlane(  IGES* aModel, std::list<IGES_ENTITY_126*> aCurves,
                        double aMinX, double aMaxX, double aMinY, double aMaxY,
                        double zHeight )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


// retrieve a trimmed parametric surface representing a vertical side
bool IGES_GEOM_SEGMENT::GetVerticalSurface( IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                            double aTopZ, double aBotZ )
{
    if( !aModel )
    {
        ERRMSG << "\n + [ERROR] null pointer passed for IGES model\n";
        return false;
    }

    if( abs( aTopZ - aBotZ ) < 1e-6 )
    {
        ERRMSG << "\n + [ERROR] degenerate surface\n";
        return false;
    }

    if( !msegtype )
    {
        ERRMSG << "\n + [ERROR] no model data to work with\n";
        return false;
    }

    bool ok = false;

    switch( msegtype )
    {
        case SEGTYPE_CIRCLE:
        case SEGTYPE_ARC:
            do
            {
                IGES_GEOM_CYLINDER cyl;
                cyl.SetParams( mcenter, mstart, mend );
                ok = cyl.Instantiate( aModel, aTopZ, aBotZ, aSurface );
            } while( 0 );

            break;

        default:
            do
            {
                IGES_GEOM_WALL wall;
                IGES_POINT p0 = mstart;
                p0.z = aTopZ;
                IGES_POINT p1 = mend;
                p1.z = aTopZ;
                IGES_POINT p2 = mend;
                p2.z = aBotZ;
                IGES_POINT p3 = mstart;
                p3.z = aBotZ;
                wall.SetParams( p0, p1, p2, p3 );
                IGES_ENTITY_144* ep = wall.Instantiate( aModel );

                if( NULL == ep )
                {
                    ERRMSG << "\n + [ERROR] could not create solid model feature\n";
                    ok = false;
                }
                else
                {
                    aSurface.push_back( ep );
                    ok = true;
                }

            } while( 0 );

            break;
    }

    return ok;
}
