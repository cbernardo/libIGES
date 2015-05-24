/*
 * Description:
 *  This is an attempt to do some simple modeling of
 * board outlines, cutouts, and drill holes as SINTEF
 * curves and surfaces and to transfer that information
 * into an IGES object.
 *
 * Items which we need to implement:
 * 1. cylinder: given a center point (x,y), radius,
 *    start and end angles, and top and bottom parameters,
 *    generate a vertical cylindrical surface.
 * 2. wall: given a line[(x0,y0),(x1,y1)] and Z top and
 *    bottom, generate a vertical surface for rendering
 * 3. plane: this is the most complex item. It must
 *    take a main outline, cutouts, and drill holes to
 *    perform complex manipulations and ultimately
 *    generate a trimmed surface representing the top and
 *    bottom of the PCB.
 *
 * Note: This test predates the refactored MCAD_SEGMENT,
 * formerly IGES_GEOM_SEGMENT, and requires the duplicate
 * implementation of GetSegmentWall() to permit testing
 * without instantiating an IGES_GEOM_OUTLINE object.
 */

#include <cmath>
#include <iostream>
#include <error_macros.h>
#include <iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <mcad_segment.h>

using namespace std;

int test_cyl_wall( void );

bool GetSegmentWall( IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                     double aTopZ, double aBotZ, MCAD_SEGMENT* aSegment );

int main()
{
    IGES model;
    bool arcDir = true;

    // create an X-shaped set of walls to demonstrate how the
    // PCB sides and drilled holes may be represented
    IGES_POINT p[5];
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
    MCAD_SEGMENT seg;
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

    model.Write( "test_segs.igs", true );

    return 0;
}

int test_cyl_wall( void )
{
    IGES model;
    IGES_GEOM_WALL plane[6];

    IGES_POINT v[8];
    v[0].x = -0.5;
    v[0].y = -1.0;
    v[0].z = 0.0;

    v[1].x = 0.5;
    v[1].y = -1.0;
    v[1].z = 0.0;

    v[2].x = 0.5;
    v[2].y = 1.0;
    v[2].z = 0.0;

    v[3].x = -0.5;
    v[3].y = 1.0;
    v[3].z = 0.0;

    v[4].x = -0.5;
    v[4].y = -1.0;
    v[4].z = 1.0;

    v[5].x = 0.5;
    v[5].y = -1.0;
    v[5].z = 1.0;

    v[6].x = 0.5;
    v[6].y = 1.0;
    v[6].z = 1.0;

    v[7].x = -0.5;
    v[7].y = 1.0;
    v[7].z = 1.0;

    // 0-1-2-3
    if( !plane[0].SetParams( v[0], v[1], v[2], v[3] ) )
    {
        cerr << "[FAIL]: could not initialize wall\n";
        return -1;
    }

    // 4-5-6-7
    if( !plane[1].SetParams( v[4], v[5], v[6], v[7] ) )
    {
        cerr << "[FAIL]: could not initialize wall\n";
        return -1;
    }

    // 3-0-4-7
    if( !plane[2].SetParams( v[3], v[0], v[4], v[7] ) )
    {
        cerr << "[FAIL]: could not initialize wall\n";
        return -1;
    }

    // 1-2-6-5
    if( !plane[3].SetParams( v[1], v[2], v[6], v[5] ) )
    {
        cerr << "[FAIL]: could not initialize wall\n";
        return -1;
    }

    // 0-1-5-4
    if( !plane[4].SetParams( v[0], v[1], v[5], v[4] ) )
    {
        cerr << "[FAIL]: could not initialize wall\n";
        return -1;
    }

    // 2-3-7-6
    if( !plane[5].SetParams( v[2], v[3], v[7], v[6] ) )
    {
        cerr << "[FAIL]: could not initialize wall\n";
        return -1;
    }

    for( int i = 0; i < 6; ++i )
    {
        if( NULL == plane[i].Instantiate( &model ) )
        {
            cerr << "[FAIL]: could not instantiate wall\n";
            return -1;
        }
    }

    IGES_GEOM_CYLINDER cyl;

    IGES_POINT p0, p1, p2;
    p0.x = -0.5;
    p0.y = -0.5;
    p0.z = 0.0;
    p1.x = -1.0;
    p1.y = 0.0;
    p1.z = 0.0;
    p2.x = 0.0;
    p2.y = 0.0;
    p2.z = 0.0;

    if( !cyl.SetParams( p0, p1, p2 ) )
    {
        cerr << "[FAIL]: could not initialize cylinder\n";
        return -1;
    }

    vector<IGES_ENTITY_144*> tlist;

    if( !cyl.Instantiate( &model, 1.5, -0.5, tlist ) )
    {
        cerr << "[FAIL]: could not instantiate cylinder\n";
        return -1;
    }

    model.Write( "test_cyl_wall.igs", true );

    return 0;
}

bool GetSegmentWall( IGES* aModel, std::vector<IGES_ENTITY_144*>& aSurface,
                     double aTopZ, double aBotZ, MCAD_SEGMENT* aSegment )
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

    if( !aSegment->GetSegType() )
    {
        ERRMSG << "\n + [ERROR] no model data to work with\n";
        return false;
    }

    bool ok = false;

    switch( aSegment->GetSegType() )
    {
        case MCAD_SEGTYPE_CIRCLE:
        case MCAD_SEGTYPE_ARC:
            do
            {
                IGES_GEOM_CYLINDER cyl;

                /* XXX - Original; restore if there are problems
                 *                if( !aSegment->IsCW() )
                 *                {
                 *                    cyl.SetParams( aSegment->GetCenter(), aSegment->GetMStart(),
                 *                                   aSegment->GetMEnd() );
            }
            else
            {
            cyl.SetParams( aSegment->GetCenter(), aSegment->GetMEnd(),
            aSegment->GetMStart() );
            }
            */

                cyl.SetParams( aSegment->GetCenter(), aSegment->GetStart(),
                               aSegment->GetEnd() );

                ok = cyl.Instantiate( aModel, aTopZ, aBotZ, aSurface );
            } while( 0 );

            break;

        default:
            do
            {
                IGES_GEOM_WALL wall;
                IGES_POINT p0 = aSegment->GetMStart();
                p0.z = aTopZ;
                IGES_POINT p1 = aSegment->GetMEnd();
                p1.z = aTopZ;
                IGES_POINT p2 = aSegment->GetMEnd();
                p2.z = aBotZ;
                IGES_POINT p3 = aSegment->GetMStart();
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
