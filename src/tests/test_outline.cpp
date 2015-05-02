/*
 * Description:
 *  This is a test suite for the IGES_GEOM_OUTLINE class
 */

#include <iostream>
#include <cmath>
#include <iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <geom_segment.h>
#include <geom_outline.h>

using namespace std;

int main()
{
    IGES_GEOM_SEGMENT* seg1 = new IGES_GEOM_SEGMENT;
    IGES_GEOM_SEGMENT seg2;

    IGES_POINT c1[3];   // parameters for Circle 1
    IGES_POINT c2[3];   // parameters for Circle 2

    // radius: 2, c(0,0)
    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 2.0;
    c1[1].y = 0.0;
    c1[2].x = 2.0;
    c1[2].y = 0.0;

    // radius: 1, c(-2,0)
    c2[0].x = -2.0;
    c2[0].y = 0.0;
    c2[1].x = -1.0;
    c2[1].y = 0.0;
    c2[2].x = -1.0;
    c2[2].y = 0.0;

    seg1->SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    IGES_GEOM_OUTLINE otln;
    bool error = false;

    if( !otln.AddSegment( seg1, error ) )
    {
        cout << "* [FAIL]: could not add segment to outline\n";
        return -1;
    }

    if( !otln.IsClosed() )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    if( !otln.SubOutline( &seg2, error ) )
    {
        cout << "* [FAIL]: could not subtract an outline, error: " << error << "\n";
        return -1;
    }

    // radius: 1, c(+2,0)
    c2[0].x = 2.0;
    c2[0].y = 0.0;
    c2[1].x = 3.0;
    c2[1].y = 0.0;
    c2[2].x = 3.0;
    c2[2].y = 0.0;
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !otln.AddCutout( &seg2, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
        return -1;
    }

    // radius: 1, c(0,+2)
    c2[0].x = 0.0;
    c2[0].y = 2.0;
    c2[1].x = 1.0;
    c2[1].y = 2.0;
    c2[2].x = 1.0;
    c2[2].y = 2.0;
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !otln.AddCutout( &seg2, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
        return -1;
    }

    // radius: 1, c(0,-2)
    c2[0].x = 0.0;
    c2[0].y = -2.0;
    c2[1].x = 1.0;
    c2[1].y = -2.0;
    c2[2].x = 1.0;
    c2[2].y = -2.0;
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !otln.AddCutout( &seg2, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
        return -1;
    }

    // radius: 0.2, c(-1,0)
    c2[0].x = -1.0;
    c2[0].y = 0.0;
    c2[1].x = -0.8;
    c2[1].y = 0.0;
    c2[2].x = -0.8;
    c2[2].y = 0.0;
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !otln.AddCutout( &seg2, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
        return -1;
    }

    // radius: 0.2, c(-1.75,0.968246)
    c2[0].x = -1.75;
    c2[0].y = 0.968246;
    c2[1].x = -1.55;
    c2[1].y = 0.968246;
    c2[2].x = -1.55;
    c2[2].y = 0.968246;
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !otln.AddCutout( &seg2, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
        return -1;
    }

    IGES_GEOM_SEGMENT* seg3 = new IGES_GEOM_SEGMENT;
    // radius: 0.5, c(0,0)
    c2[0].x = 0.0;
    c2[0].y = 0.0;
    c2[1].x = 0.5;
    c2[1].y = 0.0;
    c2[2].x = 0.5;
    c2[2].y = 0.0;
    seg3->SetParams( c2[0], c2[1], c2[2], false );

    if( !otln.AddCutout( seg3, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
        return -1;
    }

    IGES model;
    std::vector<IGES_ENTITY_144*> res;

    if( !otln.GetVerticalSurface( &model, error, res, 1.5, -1.5 ) )
    {
        cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
        return -1;
    }

    model.Write( "test_oln.igs", true );

    return 0;
}
