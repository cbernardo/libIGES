/*
 * file: test_curves.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description:
 *  This program tests the MCAD_SEGMENT, GEOM_WALL, and GEOM_CYLINDER
 * objects used to create simple rectangular planes and cylindrical arcs.
 * Only vertical walls are created at this point as anything other than
 * a simple rectangular box requires the more complex MCAD_OUTLINE and
 * IGES_GEOM_PCB objects to model.
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

#include <cmath>
#include <iostream>
#include <error_macros.h>
#include <api/dll_iges.h>
#include <geom/geom_wall.h>
#include <geom/geom_cylinder.h>
#include <api/dll_mcad_segment.h>

using namespace std;

bool GetSegmentWall( DLL_IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                     double aTopZ, double aBotZ, DLL_MCAD_SEGMENT* aSegment );

int main()
{
    DLL_IGES model;
    bool arcDir = true;

    // create an X-shaped set of walls to demonstrate how the
    // PCB sides and drilled holes may be represented
    MCAD_POINT p[5];
    p[0].x = -3.0;  // < line
    p[0].y = -3.0;
    p[1].x = -1.0;  // > line, < arc
    p[1].y = -3.0;
    p[2].x = 0.0;  // > line, < arc
    p[2].y = -3.0;
    p[3].x = 1.0;  // > arc, < line
    p[3].y = -3.0;
    p[4].x = 3.0;  // > line
    p[4].y = -3.0;

    std::vector<IGES_ENTITY_144*> res;
    DLL_MCAD_SEGMENT seg( true );

    // wall1, seg 1/3
    seg.SetParams( p[0], p[1] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall1, seg 2/3
    seg.SetParams( p[2], p[3], p[1], arcDir );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall1, seg 3/3
    seg.SetParams( p[4], p[3] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );

    // WALL 2
    p[0].x = 3.0;  // < line
    p[0].y = -3.0;
    p[1].x = 3.0;  // > line, < arc
    p[1].y = -1.0;
    p[2].x = 3.0;  // > line, < arc
    p[2].y = 0.0;
    p[3].x = 3.0;  // > arc, < line
    p[3].y = 1.0;
    p[4].x = 3.0;  // > line
    p[4].y = 3.0;
    // wall2, seg 1/3
    seg.SetParams( p[0], p[1] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall2, seg 2/3
    seg.SetParams( p[2], p[3], p[1], arcDir );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall2, seg 3/3
    seg.SetParams( p[4], p[3] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );

    // WALL 3
    p[0].x = 3.0;  // < line
    p[0].y = 3.0;
    p[1].x = 1.0;  // > line, < arc
    p[1].y = 3.0;
    p[2].x = 0.0;  // > line, < arc
    p[2].y = 3.0;
    p[3].x = -1.0;  // > arc, < line
    p[3].y = 3.0;
    p[4].x = -3.0;  // > line
    p[4].y = 3.0;
    // wall3, seg 1/3
    seg.SetParams( p[0], p[1] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall3, seg 2/3
    seg.SetParams( p[2], p[3], p[1], arcDir );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall3, seg 3/3
    seg.SetParams( p[4], p[3] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );

    // WALL 4
    p[0].x = -3.0;  // < line
    p[0].y = 3.0;
    p[1].x = -3.0;  // > line, < arc
    p[1].y = 1.0;
    p[2].x = -3.0;  // > line, < arc
    p[2].y = 0.0;
    p[3].x = -3.0;  // > arc, < line
    p[3].y = -1.0;
    p[4].x = -3.0;  // > line
    p[4].y = -3.0;
    // wall4, seg 1/3
    seg.SetParams( p[0], p[1] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall4, seg 2/3
    seg.SetParams( p[2], p[3], p[1], arcDir );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );
    // wall4, seg 3/3
    seg.SetParams( p[4], p[3] );
    GetSegmentWall( &model, res, 1.5, -1.5, &seg );

    model.Write( "test_curves.igs", true );

    return 0;
}


bool GetSegmentWall( DLL_IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                     double aTopZ, double aBotZ, DLL_MCAD_SEGMENT* aSegment )
{
    if( !aModel )
    {
        ERRMSG << "\n + [ERROR] null pointer passed for DLL_IGES model\n";
        return false;
    }

    if( abs( aTopZ - aBotZ ) < 1e-6 )
    {
        ERRMSG << "\n + [ERROR] degenerate surface\n";
        return false;
    }

    MCAD_SEGTYPE sType;
    if( !aSegment->GetSegType( sType ) || !sType )
    {
        ERRMSG << "\n + [ERROR] no model data to work with\n";
        return false;
    }

    bool ok = false;

    switch( sType )
    {
        case MCAD_SEGTYPE_CIRCLE:
        case MCAD_SEGTYPE_ARC:
            do
            {
                IGES_GEOM_CYLINDER cyl;

                MCAD_POINT p0;
                MCAD_POINT p1;
                MCAD_POINT p2;
                aSegment->GetCenter( p0 );
                aSegment->GetStart( p1 );
                aSegment->GetEnd( p2 );
                cyl.SetParams( p0, p1, p2 );

                IGES_ENTITY_144** surfs = NULL;
                int nParts = 0;

                ok = cyl.Instantiate( aModel->GetRawPtr(), aTopZ, aBotZ, surfs, nParts );

                if( ok )
                {
                    for( int i = 0; i < nParts; ++i )
                        aSurface.push_back( surfs[i] );

                    delete [] surfs;
                }

            } while( 0 );

            break;

        default:
            do
            {
                IGES_GEOM_WALL wall;
                MCAD_POINT p0;
                aSegment->GetFirstPoint( p0 );
                p0.z = aTopZ;
                MCAD_POINT p1;
                aSegment->GetLastPoint( p1 );
                p1.z = aTopZ;
                MCAD_POINT p2;
                aSegment->GetLastPoint( p2 );
                p2.z = aBotZ;
                MCAD_POINT p3;
                aSegment->GetFirstPoint( p3 );
                p3.z = aBotZ;
                wall.SetParams( p0, p1, p2, p3 );
                IGES_ENTITY_144* ep = wall.Instantiate( aModel->GetRawPtr() );

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
