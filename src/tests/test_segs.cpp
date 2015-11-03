/*
 * file: test_segs.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description:
 *  This is a test suite for the intersection algorithms of
 * the segment object. Various cases are explored and the
 * results tested against the expected result.
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

#include <iostream>
#include <cmath>
#include <api/dll_iges.h>
#include <geom/geom_wall.h>
#include <geom/geom_cylinder.h>
#include <api/dll_mcad_segment.h>

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
void testCircles( int& nTests, int& nFails );
// test intersecting circles and line segments
void testCircleSeg( int& nTests, int& nFails );
// test arcs and line segments
void testArcSeg( int& nTests, int& nFails );
// test arcs on arcs
void testArcs( int& nTests, int& nFails );
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
    int nTests = 0;
    int nFails = 0;

    testCircles( nTests, nFails );
    testCircleSeg( nTests, nFails );
    testArcSeg( nTests, nFails );
    testArcs( nTests, nFails );

    cout << "\n** SUMMARY: " << nFails << " failures in " << nTests << " tests\n\n";

    return 0;
}

void testCircles( int& nTests, int& nFails )
{
    DLL_MCAD_SEGMENT seg1( true );
    DLL_MCAD_SEGMENT seg2( true );

    MCAD_POINT c1[3];   // parameters for Circle 1
    MCAD_POINT c2[3];   // parameters for Circle 2

    cout << "* Test: tangent circles\n";
    ++nTests;

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
    MCAD_POINT* iList = NULL;
    int nIntersects = 0;

    if( !seg1.GetIntersections( seg2.GetRawPtr(), iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with tangent flag\n";
        ++nFails;

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: C1 encircled by C2\n";
    ++nTests;

    // radius: 0.5, c(0.5,0)
    c2[0].x = 0.5;
    c2[0].y = 0.0;
    c2[1].x = 1.0;
    c2[1].y = 0.0;
    c2[2].x = 1.0;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_ENCIRCLES );
        cout << "[expected failure: encircles] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with 'encircles'\n";
        ++nFails;

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: C2 inside C1\n";
    ++nTests;

    // radius: 1.5, c(0,0)
    c2[0].x = 0.0;
    c2[0].y = 0.0;
    c2[1].x = 1.5;
    c2[1].y = 0.0;
    c2[2].x = 1.5;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_INSIDE );
        cout << "[expected failure: inside] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with 'inside'\n";
        ++nFails;

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: no intersection\n";
    ++nTests;

    // radius: 1, c(3,0)
    c2[0].x = 3.0;
    c2[0].y = 0.0;
    c2[1].x = 4.0;
    c2[1].y = 0.0;
    c2[2].x = 4.0;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "[expected failure: none] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        cout << "  [FAIL]: expected failure with 'none' (no intersection)\n";
        ++nFails;

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: intersect at (0, 1), (0, -1)\n";
    ++nTests;

    // radius: 3, c(sqrt(8),0)
    c2[0].x = sqrt(8.0);
    c2[0].y = 0.0;
    c2[1].x = sqrt(8.0) + 3.0;
    c2[1].y = 0.0;
    c2[2].x = c2[1].x;
    c2[2].y = 0.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success --";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "found intersections at:\n";
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    return;
}


// test intersecting circles and line segments
void testCircleSeg(  int& nTests, int& nFails )
{
    DLL_MCAD_SEGMENT seg1( true );
    DLL_MCAD_SEGMENT seg2( true );

    MCAD_POINT c1[3];   // parameters for Circle 1
    MCAD_POINT l1[2];   // parameters for Line 1

    cout << "* Test: tangent to circle (C1, L1)\n";
    ++nTests;

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
    MCAD_POINT* iList = NULL;
    int nIntersects = 0;

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected single point, got " << nIntersects << " ";
            print_flag( flag );
            cout << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_TANGENT );
            cout << " expected single endpoint (67.5994, 67.5994) with tangent flag ";
            print_flag( flag );
            cout << "\n  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: tangent to circle (L1, C1)\n";
    ++nTests;

    l1[0].x -= 5.0;
    l1[0].y += 5.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected single point, got " << nIntersects << " ";
            print_flag( flag );
            cout << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_TANGENT );
            cout << " expected single endpoint (67.5994, 67.5994) with tangent flag ";
            print_flag( flag );
            cout << "\n  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: 1 point on circle\n";
    ++nTests;

    l1[0].x += 5.0;
    l1[0].y -= 5.0;
    l1[1].x = l1[0].x + 5.0;
    l1[1].y = l1[0].y + 5.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected single point, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << " expected single endpoint (67.5994, 67.5994) ";
            print_flag( flag );
            cout << "\n  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: 2 points on circle\n";
    ++nTests;

    l1[0].x = -l1[1].x;
    l1[0].y = -l1[1].y;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg2.GetIntersections( seg1, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( nIntersects != 2 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 2 points, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_NONE );
            cout << "expected 2 points (67.5994, 67.5994), (-67.5994, -67.5994)\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
            cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    return;
}


// test arcs and line segments
void testArcSeg( int& nTests, int& nFails )
{
    DLL_MCAD_SEGMENT seg1( true );
    DLL_MCAD_SEGMENT seg2( true );

    MCAD_POINT c1[3];   // parameters for Arc 1
    MCAD_POINT l1[2];   // parameters for Line 1

    cout << "* Test: vertical tangent to arc (A1, L1)\n";
    ++nTests;

    // radius: 1, c(0,0)
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
    MCAD_POINT* iList = NULL;
    int nIntersects = 0;

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected single point, got " << nIntersects << " ";
            print_flag( flag );
            cout << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_TANGENT );
            cout << " expected single point (67.5994, 67.5994) with tangent flag ";
            print_flag( flag );
            cout << "\n  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: horizontal tangent to arc (A1, L1)\n";
    ++nTests;

    // horizontal tangent:
    l1[0].x = -1.0;
    l1[0].y = 1.0;
    l1[1].x = 1.0;
    l1[1].y = 1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success -- ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected single point, got " << nIntersects << " ";
            print_flag( flag );
            cout << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_TANGENT );
            cout << " expected single point (67.5994, 67.5994) with tangent flag ";
            print_flag( flag );
            cout << "\n  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: tangent not on arc (A1, L1)\n";
    ++nTests;

    l1[0].x = -1.0;
    l1[0].y = -1.0;
    l1[1].x = -1.0;
    l1[1].y = 1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "[expected failure: none] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        ++nFails;
        cout << "  [FAIL]: expected failure flag 'none'\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: endpoint arc (A1, L1), single point\n";
    ++nTests;

    l1[0].x = 0.0;
    l1[0].y = 1.0;
    l1[1].x = 0.0;
    l1[1].y = 2.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 1 point, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: endpoint arc (A1, L1), 2 points\n";
    ++nTests;

    l1[0].x = 0.0;
    l1[0].y = 1.0;
    l1[1].x = 0.0;
    l1[1].y = -1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( nIntersects != 2 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 2 points, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
            cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: endpoint arc (A1, L1), 2 points (one is not on an arc endpoint)\n";
    ++nTests;

    l1[0].x = 0.0;
    l1[0].y = 1.0;
    l1[1].x = 1.0;
    l1[1].y = 0.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( nIntersects != 2 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 2 points, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
            cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: segment intersects arc at 2 endpoints of arc (but not endpoints of segment)\n";
    ++nTests;

    l1[0].x = 0.0;
    l1[0].y = -2.0;
    l1[1].x = 0.0;
    l1[1].y = 2.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( nIntersects != 2 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 2 points, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
            cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: segment intersects arc at 1 endpoint of arc (but not endpoints of segment)\n";
    ++nTests;

    l1[0].x = .0;
    l1[0].y = 0.0;
    l1[1].x = 0.0;
    l1[1].y = 2.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        cout << "  [FAIL]: expected success with endpoint flag\n";
        ++nFails;
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 1 point, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: segment intersects arc at 2 endpoints of segment (but not endpoints of arc)\n";
    ++nTests;

    l1[0].x = cos( M_PI * 0.25 );
    l1[0].y = -l1[0].x;
    l1[1].x = l1[0].x;
    l1[1].y = l1[0].x;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with endpoint flag\n";
    }
    else
    {
        if( nIntersects != 2 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 2 points, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
            cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: segment intersects arc at 2 points; none are endpoints\n";
    ++nTests;

    l1[0].y = -1.0;
    l1[1].y = 1.0;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with flag 'none'\n";
    }
    else
    {
        if( nIntersects != 2 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 2 points, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_NONE );
            cout << "[expected flag: none] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
            cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: segment intersects arc at 1 endpoint of the segment\n";
    ++nTests;

    l1[0].y = 0.0;
    l1[1].y = l1[1].x;
    seg2.SetParams( l1[0], l1[1] );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
        cout << "  [FAIL]: expected success with flag 'endpoint'\n";
    }
    else
    {
        if( nIntersects != 1 )
        {
            ++nFails;
            cout << "  [FAIL]: expected 1 point, got " << nIntersects << "\n";
        }
        else
        {
            checkFlags( flag, MCAD_IFLAG_ENDPOINT );
            cout << "[expected flag: endpoint] ";
            print_flag( flag );
            cout << "\n";
            cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        }

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    return;
}


// test arcs on arcs
void testArcs( int& nTests, int& nFails )
{
    DLL_MCAD_SEGMENT seg1( true );
    DLL_MCAD_SEGMENT seg2( true );

    MCAD_POINT c1[3];   // parameters for Arc 1
    MCAD_POINT c2[3];   // parameters for Arc 2

    cout << "* Test: tangent intersecting arcs\n";
    ++nTests;

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
    MCAD_POINT* iList = NULL;
    int nIntersects = 0;

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_TANGENT );
        cout << "[expected failure: tangent] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        ++nFails;
        cout << "  [FAIL]: expected failure with tangent flag\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: tangent non-intersecting arcs\n";
    ++nTests;

    c2[0].x = 2.0;
    c2[0].y = 0.0;
    c2[1].x = 2.0;
    c2[1].y = -1.0;
    c2[2].x = 2.0;
    c2[2].y = 1.0;

    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: no intersection. no flags set
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        checkFlags( flag, MCAD_IFLAG_NONE );
        cout << "[expected failure: none] ";
        print_flag( flag );
        cout << "\n";
    }
    else
    {
        ++nFails;
        cout << "  [FAIL]: expected failure with flag 'none' (no intersection)\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: C1 == C2\n";
    ++nTests;

    seg2.SetParams( c1[0], c1[1], c1[2], false );

    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: partially overlapping arcs\n";
    ++nTests;

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
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: non-overlapping arcs with same radius and coincident edges, r1 = r2\n";
    ++nTests;

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
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: non-overlapping arcs, first arc is outside second arc, r1 > r2\n";
    cout << "  and second arc is inside first arc\n";
    ++nTests;

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
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: non-overlapping arcs, first arc is inside second arc, r1 < r2\n";
    ++nTests;

    seg1.SetParams( c2[0], c2[1], c2[2], false );
    seg2.SetParams( c1[0], c1[1], c1[2], false );

    // expected result: success with flag INSIDE
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[1].y << "\n";
        cout << "  p2: " << iList[0].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: non-overlapping arcs, first arc is outside second arc, r1 > r2\n";
    ++nTests;

    c2[0].x = 3.0;
    c2[0].y = 3.0;
    c2[1].x = 3.0;
    c2[1].y = 2.0;
    c2[2].x = 2.0;
    c2[2].y = 3.0;

    seg1.SetParams( c1[0], c1[1], c1[2], false );
    seg2.SetParams( c2[0], c2[1], c2[2], false );

    // expected result: success with flag OUTSIDE
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    cout << "* Test: non-overlapping arcs, first arc is outside second arc, r1 < r2\n";
    ++nTests;

    seg1.SetParams( c2[0], c2[1], c2[2], false );
    seg2.SetParams( c1[0], c1[1], c1[2], false );

    // expected result: success with flag OUTSIDE
    if( !seg1.GetIntersections( seg2, iList, nIntersects, flag ) )
    {
        ++nFails;
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
        cout << "  p1: " << iList[0].x << ", " << iList[0].y << "\n";
        cout << "  p2: " << iList[1].x << ", " << iList[1].y << "\n";

        delete [] iList;
        iList = NULL;
        nIntersects = 0;
    }

    // XXX -
    // WARNING: TO BE IMPLEMENTED
    cout << "* Test: multiple-overlap arcs\n";
    cout << "  [FAIL]: TEST NOT IMPLEMENTED\n";
    ++nTests;
    ++nFails;

    return;
}
