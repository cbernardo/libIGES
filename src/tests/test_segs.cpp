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
#include <mcad_segment.h>

using namespace std;

void print_flag( MCAD_INTERSECT_FLAG flag )
{
    switch( flag )
    {
        case MCAD_IFLAG_NONE:
            cout << "[flag: none]";
            break;

        case MCAD_IFLAG_ENDPOINT:
            cout << "[flag: endpoint]";
            break;

        case MCAD_IFLAG_TANGENT:
            cout << "[flag: tangent]";
            break;

        case MCAD_IFLAG_EDGE:
            cout << "[flag: edge]";
            break;

        case MCAD_IFLAG_INSIDE:
            cout << "[flag: inside]";
            break;

        case MCAD_IFLAG_ENCIRCLES:
            cout << "[flag: encircles]";
            break;

        case MCAD_IFLAG_OUTSIDE:
            cout << "[flag: outside]";
            break;

        case MCAD_IFLAG_IDENT:
            cout << "[flag: identical]";
            break;

        case MCAD_IFLAG_MULTIEDGE:
            cout << "[flag: multiedge]";
            break;

        default:
            cout << "[unknown flag value: " << flag << "]";
            break;
    }

    return;
}

// test intersecting circles
void testCircles( void );
// test intersecting circles and line segments
void testCircleSeg( void );
// test arcs and line segments
void testArcSeg( void );
// test arcs on arcs
void testArcs( void );
// print "  [OK]:" or "  [FAIL]:" depending on whether or not the flags match
void checkFlags( MCAD_INTERSECT_FLAG f1, MCAD_INTERSECT_FLAG f2 )
{
    if( f1 != f2 )
        cout << "  [FAIL]: ";
    else
        cout << "  [OK]: ";
}

int main()
{
    testCircles();
    testCircleSeg();
    testArcSeg();
    testArcs();

    return 0;
}

void testCircles( void )
{
    MCAD_SEGMENT seg1;
    MCAD_SEGMENT seg2;

    IGES_POINT c1[3];   // parameters for Circle 1
    IGES_POINT c2[3];   // parameters for Circle 2

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
    MCAD_INTERSECT_FLAG flag;
    std::list<IGES_POINT> ilist;

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
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
        checkFlags( flag, MCAD_IFLAG_ENCIRCLES );
        cout << "[expected failure: encircles] ";
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
        checkFlags( flag, MCAD_IFLAG_INSIDE );
        cout << "[expected failure: inside] ";
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
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "[expected failure: none] ";
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
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "found intersections at:\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    return;
}


// test intersecting circles and line segments
void testCircleSeg( void )
{
    MCAD_SEGMENT seg1;
    MCAD_SEGMENT seg2;

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
    MCAD_INTERSECT_FLAG flag;
    std::list<IGES_POINT> ilist;

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ilist.clear();
    }

    cout << "* Test: tangent to circle (L1, C1)\n";
    l1[0].x -= 5.0;
    l1[0].y += 5.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
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
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << " expected single endpoint (67.5994, 67.5994) ";
            print_flag( flag );
            cout << "\n  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
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
            checkFlags( flag, MCAD_IFLAG_NONE );
            cout << "expected 2 points (67.5994, 67.5994), (-67.5994, -67.5994)\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    return;
}


// test arcs and line segments
void testArcSeg( void )
{
    MCAD_SEGMENT seg1;
    MCAD_SEGMENT seg2;

    IGES_POINT c1[3];   // parameters for Arc 1
    IGES_POINT l1[2];   // parameters for Line 1

    cout << "* Test: vertical tangent to arc (A1, L1)\n";

    // radius: 95.6, c(0,0)
    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 0;
    c1[1].y = -1.0;
    c1[2].x = 0;
    c1[2].y = 1.0;

    // vertical tangent:
    l1[0].x = 1.0;
    l1[0].y = -1.0;
    l1[1].x = 1.0;
    l1[1].y = 1.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( l1[0], l1[1] );

    // expect invalid geometry: tangent
    MCAD_INTERSECT_FLAG flag;
    std::list<IGES_POINT> ilist;

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ilist.clear();
    }

    cout << "* Test: horizontal tangent to arc (A1, L1)\n";

    // horizontal tangent:
    l1[0].x = -1.0;
    l1[0].y = 1.0;
    l1[1].x = 1.0;
    l1[1].y = 1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ilist.clear();
    }

    cout << "* Test: tangent not on arc (A1, L1)\n";

    l1[0].x = -1.0;
    l1[0].y = -1.0;
    l1[1].x = -1.0;
    l1[1].y = 1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "[expected failure: none] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure flag 'none'\n";
        ilist.clear();
    }

    cout << "* Test: endpoint arc (A1, L1), single point\n";

    l1[0].x = 0.0;
    l1[0].y = 1.0;
    l1[1].x = 0.0;
    l1[1].y = 2.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( ilist.size() != 1 )
        {
            cout << "  [FAIL]: expected 1 point, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: endpoint arc (A1, L1), 2 points\n";

    l1[0].x = 0.0;
    l1[0].y = 1.0;
    l1[1].x = 0.0;
    l1[1].y = -1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( ilist.size() != 2 )
        {
            cout << "  [FAIL]: expected 2 points, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: endpoint arc (A1, L1), 2 points (one is not on an arc endpoint)\n";

    l1[0].x = 0.0;
    l1[0].y = 1.0;
    l1[1].x = 1.0;
    l1[1].y = 0.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( ilist.size() != 2 )
        {
            cout << "  [FAIL]: expected 2 points, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: segment intersects arc at 2 endpoints of arc (but not endpoints of segment)\n";

    l1[0].x = 0.0;
    l1[0].y = -2.0;
    l1[1].x = 0.0;
    l1[1].y = 2.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( ilist.size() != 2 )
        {
            cout << "  [FAIL]: expected 2 points, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: segment intersects arc at 1 endpoint of arc (but not endpoints of segment)\n";

    l1[0].x = .0;
    l1[0].y = 0.0;
    l1[1].x = 0.0;
    l1[1].y = 2.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( ilist.size() != 1 )
        {
            cout << "  [FAIL]: expected 1 point, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: segment intersects arc at 2 endpoints of segment (but not endpoints of arc)\n";

    l1[0].x = cos( M_PI * 0.25 );
    l1[0].y = -l1[0].x;
    l1[1].x = l1[0].x;
    l1[1].y = l1[0].x;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( ilist.size() != 2 )
        {
            cout << "  [FAIL]: expected 2 points, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: segment intersects arc at 2 points; none are endpoints\n";

    l1[0].y = -1.0;
    l1[1].y = 1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with flag 'none'\n";
    }
    else
    {
        if( ilist.size() != 2 )
        {
            cout << "  [FAIL]: expected 2 points, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_NONE );
            cout << "[expected flag: none] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
            cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        }
        ilist.clear();
    }

    cout << "* Test: segment intersects arc at 1 endpoint of the segment\n";

    l1[0].y = 0.0;
    l1[1].y = l1[1].x;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: expected success with flag 'endpoint'\n";
    }
    else
    {
        if( ilist.size() != 1 )
        {
            cout << "  [FAIL]: expected 1 point, got " << ilist.size() << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        }
        ilist.clear();
    }

    return;
}


// test arcs on arcs
void testArcs( void )
{
    MCAD_SEGMENT seg1;
    MCAD_SEGMENT seg2;

    IGES_POINT c1[3];   // parameters for Arc 1
    IGES_POINT c2[3];   // parameters for Arc 2

    cout << "* Test: tangent intersecting arcs\n";

    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 0.0;
    c1[1].y = -1.0;
    c1[2].x = 0.0;
    c1[2].y = 1.0;

    c2[0].x = 2.0;
    c2[0].y = 0.0;
    c2[1].x = 2.0;
    c2[1].y = 1.0;
    c2[2].x = 2.0;
    c2[2].y = -1.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expect invalid geometry: tangent
    MCAD_INTERSECT_FLAG flag;
    std::list<IGES_POINT> ilist;

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ilist.clear();
    }

    cout << "* Test: tangent non-intersecting arcs\n";

    c2[0].x = 2.0;
    c2[0].y = 0.0;
    c2[1].x = 2.0;
    c2[1].y = -1.0;
    c2[2].x = 2.0;
    c2[2].y = 1.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: no intersection. no flags set
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "[expected failure: none] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with flag 'none' (no intersection)\n";
        ilist.clear();
    }

    cout << "* Test: C1 == C2\n";

    seg2.SetParams( c1[0], c1[1], c1[2], false );

    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: edge] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_EDGE );
        cout << "[expected flag: edge] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    cout << "* Test: partially overlapping arcs\n";

    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = -1.0;
    c1[1].y = 0.0;
    c1[2].x = 0.0;
    c1[2].y = 1.0;

    c2[0].x = 0.0;
    c2[0].y = 0.0;
    c2[1].x = 0.0;
    c2[1].y = -1.0;
    c2[2].x = 1.0;
    c2[2].y = 0.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: intersection on an edge
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: edge] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_EDGE );
        cout << "[expected flag: edge] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    cout << "* Test: non-overlapping arcs with same radius and coincident edges, r1 = r2\n";

    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 0.0;
    c1[1].y = 1.0;
    c1[2].x = 0.0;
    c1[2].y = -1.0;

    c2[0].x = 0.0;
    c2[0].y = 0.0;
    c2[1].x = 0.0;
    c2[1].y = -1.0;
    c2[2].x = 0.0;
    c2[2].y = 1.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: fail with no flags set
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: endpoint] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_ENDPOINT );
        cout << "[expected success with flag: endpoint] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    cout << "* Test: non-overlapping arcs, first arc is outside second arc, r1 > r2\n";
    cout << "  and second arc is inside first arc\n";

    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 2.0;
    c1[1].y = 3.0;
    c1[2].x = 3.0;
    c1[2].y = 2.0;

    c2[0].x = 2.0;
    c2[0].y = 2.0;
    c2[1].x = 2.0;
    c2[1].y = 3.0;
    c2[2].x = 3.0;
    c2[2].y = 2.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: success with flag OUTSIDE
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: outside] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_OUTSIDE );
        cout << "[expected success with flag: outside] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    cout << "* Test: non-overlapping arcs, first arc is inside second arc, r1 < r2\n";
    seg1.SetParams( c2[0], c2[1], c2[2], false );
    seg2.SetParams( c1[0], c1[1], c1[2], false );

    // expected result: success with flag INSIDE
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: inside] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_INSIDE );
        cout << "[expected success with flag: inside] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    cout << "* Test: non-overlapping arcs, first arc is outside second arc, r1 > r2\n";

    c2[0].x = 3.0;
    c2[0].y = 3.0;
    c2[1].x = 3.0;
    c2[1].y = 2.0;
    c2[2].x = 2.0;
    c2[2].y = 3.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: success with flag OUTSIDE
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: outside] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_OUTSIDE );
        cout << "[expected success with flag: outside] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

    cout << "* Test: non-overlapping arcs, first arc is outside second arc, r1 < r2\n";

    seg1.SetParams( c2[0], c2[1], c2[2], false );
    seg2.SetParams( c1[0], c1[1], c1[2], false );

    // expected result: success with flag OUTSIDE
    if( !seg1.GetIntersections( seg2, ilist, flag ) )
    {
        cout << "  [FAIL]: [expected success with flag: outside] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_OUTSIDE );
        cout << "[expected success with flag: outside] ";
        print_flag( flag );
        cout << "\n";
        cout << "  p1: " << ilist.front().x << ", " << ilist.front().y << "\n";
        cout << "  p2: " << ilist.back().x << ", " << ilist.back().y << "\n";
        ilist.clear();
    }

#warning TO BE IMPLEMENTED: MULTI-EDGE TEST
cout << "* Test: multiple-overlap arcs\n";
cout << "  [FAIL]: TEST NOT IMPLEMENTED\n";

    return;
}
