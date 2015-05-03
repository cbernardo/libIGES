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

int test_arcs( void );
int test_lines( void );

int main()
{
    if( 0 )
    {
        if( test_arcs() )
        {
            cout << "[FAIL]: test_arcs() encountered problems\n";
            return -1;
        }
    }

    if( test_lines() )
    {
        cout << "[FAIL]: test_lines() encountered problems\n";
        return -1;
    }

    cout << "[OK]: All tests passed\n";
    return 0;
}


int test_arcs( void )
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

    if( 1 )
    {
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
    }

    if( 1 )
    {
        // radius: 0.2, c(-1,0)
        c2[0].x = -1.0;
        c2[0].y = 0.0;
        c2[1].x = -0.8;
        c2[1].y = 0.0;
        c2[2].x = -0.8;
        c2[2].y = 0.0;
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.SubOutline( &seg2, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    if( 1 )
    {
        // radius: 0.5, c(-1.75,0.968246)
        c2[0].x = -1.75;
        c2[0].y = 0.968246;
        c2[1].x = -1.25;
        c2[1].y = 0.968246;
        c2[2].x = -1.25;
        c2[2].y = 0.968246;
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( &seg2, true, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    if( 1 )
    {
        // radius: 1, c(2*cos(45), 2*sin(45))
        c2[0].x = 2.0 * cos(M_PI / 4.0);
        c2[0].y = 2.0 * sin(M_PI / 4.0);
        c2[1].x = c2[0].x + 1.0;
        c2[1].y = c2[0].y;
        c2[2].x = c2[1].x;
        c2[2].y = c2[0].y;
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( &seg2, true, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    if( 1 )
    {
        // ensure that we apply a cutout which concides with endpoints on the outline
        IGES_GEOM_SEGMENT s0;
        c2[0].x = 0.0;
        c2[0].y = 0.0;
        c2[1].x = 2.0;
        c2[1].y = 0.0;
        c2[2].x = 2.0;
        c2[2].y = 0.0;
        s0.SetParams( c2[0], c2[1], c2[2], false );

        IGES_GEOM_SEGMENT s1;
        c2[0].x = 0.0;
        c2[0].y = -2.0;
        c2[1].x = 1.0;
        c2[1].y = -2.0;
        c2[2].x = 1.0;
        c2[2].y = -2.0;
        s1.SetParams( c2[0], c2[1], c2[2], false );

        std::list<IGES_POINT> iList;
        IGES_INTERSECT_FLAG flag;
        s0.GetIntersections( s1, iList, flag );

        c2[0].x = 0.0;
        c2[0].y = -1.2;
        s1.SetParams( c2[0], iList.front(), iList.front(), false );

        if( !otln.SubOutline( &s1, error ) )
        {
            cout << "* [FAIL]: could not add a cutout which intersects endpoints, error: " << error << "\n";
            return -1;
        }

    }

    if( 1 )
    {
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
    }

    if( 1 )
    {
        // radius: 0.2, c(0, 0.5)
        c2[0].x = 0.0;
        c2[0].y = 0.5;
        c2[1].x = 0.2;
        c2[1].y = 0.5;
        c2[2].x = 0.2;
        c2[2].y = 0.5;
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.SubOutline( &seg2, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    if( 1 )
    {
        // radius: 0.3, c(0.5*cos(45),-0.5*sin(45))
        c2[0].x = 0.5 * cos( M_PI/4.0 );
        c2[0].y = -c2[0].x;
        c2[1].x = c2[0].x + 0.3;
        c2[1].y = c2[0].y;
        c2[2].x = c2[1].x;
        c2[2].y = c2[1].y;
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.SubOutline( &seg2, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    IGES model;
    std::vector<IGES_ENTITY_144*> res;

    if( !otln.GetVerticalSurface( &model, error, res, 1.5, -1.5 ) )
    {
        cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
        return -1;
    }

    model.Write( "test_oln_arcs.igs", true );
    return 0;
}


int test_lines( void )
{
    IGES_GEOM_SEGMENT* sides[4];
    IGES_POINT v[4];

    v[0].x = 10.0;
    v[0].y = 10.0;
    v[1].x = -10.0;
    v[1].y = 10.0;
    v[2].x = -10.0;
    v[2].y = -10.0;
    v[3].x = 10.0;
    v[3].y = -10.0;

    for( int i = 0; i < 4; ++i )
        sides[i] = new IGES_GEOM_SEGMENT;

    sides[0]->SetParams(v[0], v[1]);
    sides[1]->SetParams(v[1], v[2]);
    sides[2]->SetParams(v[2], v[3]);
    sides[3]->SetParams(v[3], v[0]);

    IGES_GEOM_OUTLINE otln;
    bool error = false;

    if( !otln.AddSegment( sides[0], error )
        || !otln.AddSegment( sides[1], error )
        || !otln.AddSegment( sides[2], error )
        || !otln.AddSegment( sides[3], error ) )
    {
        cout << "* [FAIL]: could not add segment to outline\n";
        return -1;
    }

    if( !otln.IsClosed() )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    if( 1 )
    {
        // nibble out 8 bits
        IGES_GEOM_SEGMENT circ;
        IGES_POINT c1[2];   // parameters for circles

        // radius: 0.5, c(10,10)
        c1[0].x = 10.0;
        c1[0].y = 10.0;
        c1[1].x = 10.5;
        c1[1].y = 10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 1.0, c(0,10)
        c1[0].x = 0.0;
        c1[0].y = 10.0;
        c1[1].x = 1.0;
        c1[1].y = 10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 1.5, c(-10,10)
        c1[0].x = -10.0;
        c1[0].y = 10.0;
        c1[1].x = -8.5;
        c1[1].y = 10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 2.0, c(-10,0)
        c1[0].x = -10.0;
        c1[0].y = 0.0;
        c1[1].x = -8;
        c1[1].y = 0.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 2.5, c(-10,-10)
        c1[0].x = -10.0;
        c1[0].y = -10.0;
        c1[1].x = -7.5;
        c1[1].y = -10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 3.0, c(0,-10)
        c1[0].x = 0.0;
        c1[0].y = -10.0;
        c1[1].x = 3.0;
        c1[1].y = -10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        /*
        // radius: 3.5, c(10,-10)
        c1[0].x = 10.0;
        c1[0].y = -10.0;
        c1[1].x = 13.5;
        c1[1].y = -10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 4.0, c(10,0)
        c1[0].x = 10.0;
        c1[0].y = 0.0;
        c1[1].x = 14.0;
        c1[1].y = 0.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( &circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }
        */

    }

    IGES model;
    std::vector<IGES_ENTITY_144*> res;

    if( !otln.GetVerticalSurface( &model, error, res, 0.8, -0.8 ) )
    {
        cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
        return -1;
    }

    model.Write( "test_oln_lines.igs", true );
    return 0;
}
