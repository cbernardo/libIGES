/*
 * Description:
 *  This is a test suite for the intersection algorithms of
 * the segment object.
 */

#include <iostream>
#include <cmath>
#include <iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <geom_segment.h>

using namespace std;

void print_flag( IGES_INTERSECT_FLAG flag )
{
    switch( flag )
    {
        case IGES_IFLAG_NONE:
            cout << "[flag: none]";
            break;

        case IGES_IFLAG_TANGENT:
            cout << "[flag: tangent]";
            break;

        case IGES_IFLAG_EDGE:
            cout << "[flag: edge]";
            break;

        case IGES_IFLAG_INSIDE:
            cout << "[flag: inside]";
            break;

        case IGES_IFLAG_ENCIRCLES:
            cout << "[flag: encircles]";
            break;

        case IGES_IFLAG_IDENT:
            cout << "[flag: identical]";
            break;

        default:
            cout << "[invalid flag value]";
            break;
    }

    return;
}

// test intersectiong circles
void testCircles( void );
// test intersecting circles and line segments
void testCircleSeg( void );

int main()
{
    //testCircles();
    testCircleSeg();

    return 0;
}

void testCircles( void )
{
    IGES_GEOM_SEGMENT seg1;
    IGES_GEOM_SEGMENT seg2;

    IGES_POINT c1[3];   // parameters for Circle 1
    IGES_POINT c2[3];   // parameters for Circle 1

    cout << "* Test: tangent circles\n";

    // radius: 1, c(0,0)
    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 1.0;
    c1[1].y = 0.0;
    c1[2].x = 1.0;
    c1[2].y = 0.0;

    // radius: 1, c(2,0)
    c2[0].x = 2.0;
    c2[0].y = 0.0;
    c2[1].x = 1.0;
    c2[1].y = 0.0;
    c2[2].x = 1.0;
    c2[2].y = 0.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expect invalid geometry: tangent
    IGES_INTERSECT_FLAG flag;
    std::list<IGES_POINT> ilist;

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [OK]: [expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ilist.clear();
    }

    cout << "* Test: C1 encircled by C2\n";

    // radius: 0.5, c(0.5,0)
    c2[0].x = 0.5;
    c2[0].y = 0.0;
    c2[1].x = 1.0;
    c2[1].y = 0.0;
    c2[2].x = 1.0;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [OK]: [expected failure: encircles] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with 'encircles'\n";
        ilist.clear();
    }

    cout << "* Test: C2 inside C1\n";

    // radius: 1.5, c(0,0)
    c2[0].x = 0.0;
    c2[0].y = 0.0;
    c2[1].x = 1.5;
    c2[1].y = 0.0;
    c2[2].x = 1.5;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [OK]: [expected failure: inside] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with 'inside'\n";
        ilist.clear();
    }

    cout << "* Test: no intersection\n";

    // radius: 1, c(3,0)
    c2[0].x = 3.0;
    c2[0].y = 0.0;
    c2[1].x = 4.0;
    c2[1].y = 0.0;
    c2[2].x = 4.0;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [OK]: [expected failure: none] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with 'none' (no intersection)\n";
        ilist.clear();
    }

    cout << "* Test: intersect at (0, 1), (0, -1)\n";

    // radius: 3, c(sqrt(8),0)
    c2[0].x = sqrt(8.0);
    c2[0].y = 0.0;
    c2[1].x = sqrt(8.0) + 3.0;
    c2[1].y = 0.0;
    c2[2].x = c2[1].x;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success --";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [OK]: found intersections at:\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    return;
}


// test intersecting circles and line segments
void testCircleSeg( void )
{
    IGES_GEOM_SEGMENT seg1;
    IGES_GEOM_SEGMENT seg2;

    IGES_POINT c1[3];   // parameters for Circle 1
    IGES_POINT l1[2];   // parameters for Line 1

    cout << "* Test: tangent to circle (C1, L1)\n";

    // radius: 95.6, c(0,0)
    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 95.6;
    c1[1].y = 0.0;
    c1[2].x = 95.6;
    c1[2].y = 0.0;

    // 45 degree tangent:
    l1[0].x = 95.6 * cos( M_PI * 0.25 );
    l1[0].y = 95.6 * sin( M_PI * 0.25 );
    l1[1].x = l1[0].x + 10.0;
    l1[1].y = l1[0].y - 10.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( l1[0], l1[1] );

    // expect invalid geometry: tangent
    IGES_INTERSECT_FLAG flag;
    std::list<IGES_POINT> ilist;

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [OK]: [expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
    }

    cout << "* Test: tangent to circle (L1, C1)\n";
    l1[0].x -= 5.0;
    l1[0].y += 5.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, ilist, flag ) )
    {
        cout << "  [OK]: [expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ilist.clear();
    }

    cout << "* Test: 1 point on circle\n";
    l1[0].x += 5.0;
    l1[0].y -= 5.0;
    l1[1].x = l1[0].x + 5.0;
    l1[1].y = l1[0].y + 5.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( ilist.size() != 1 )
        {
            cout << "  [FAIL]: expected single point, got " << ilist.size() << "\n";
        }
        else
        {
            cout << "  [OK]: expected single point (67.5994, 67.5994)\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: 2 points on circle\n";
    l1[0].x = -l1[1].x;
    l1[0].y = -l1[1].y;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( ilist.size() != 2 )
        {
            cout << "  [FAIL]: expected 2 points, got " << ilist.size() << "\n";
        }
        else
        {
            cout << "  [OK]: expected 2 points (67.5994, 67.5994), (-67.5994, -67.5994)\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    return;
}
