/*
 * file: test_outline.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description:
 *  This is a test suite for the IGES_GEOM_PCB class which
 * performs outline manipulations (add, subtract outlines)
 * and creates vertical walls corresponding to the resulting
 * outline.
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
#include <dll_iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <dll_mcad_segment.h>
#include <dll_iges_geom_pcb.h>

using namespace std;

// take one large circle and subtract a series of smaller circles from it.
int test_arcs( void );
// take a square and subtract a series of circles from it.
int test_lines( void );
// take a square and add a series of circles to it
int test_addr( void );
// test overlap operations of generic outlines
//   + subs: set to true to test subtraction, false for addition
//   + primeA: set to true to test operations on Outline A (Circle),
//             false for Outline B (square).
int test_otln( bool subs, bool primeA );

int main()
{
    if( 1 )
    {
        if( test_arcs() )
        {
            cout << "[FAIL]: test_arcs() encountered problems\n";
            return -1;
        }
    }

    if( 1 )
    {
        if( test_lines() )
        {
            cout << "[FAIL]: test_lines() encountered problems\n";
            return -1;
        }
    }

    if( 1 )
    {
        if( test_addr() )
        {
            cout << "[FAIL]: test_addr() encountered problems\n";
            return -1;
        }
    }

    if( 1 )
    {
        if( 1 )
        {
            if( test_otln( false, true ) )
            {
                cout << "[FAIL]: test_otln() encountered problems adding to Outline A\n";
                return -1;
            }
        }

        if( 1 )
        {
            if( test_otln( false, false ) )
            {
                cout << "[FAIL]: test_otln() encountered problems adding to Outline B\n";
                return -1;
            }
        }

        if( 1 )
        {
            if( test_otln( true, true ) )
            {
                cout << "[FAIL]: test_otln() encountered problems subtracting from Outline A\n";
                return -1;
            }
        }

        if( 1 )
        {
            if( test_otln( true, false ) )
            {
                cout << "[FAIL]: test_otln() encountered problems subtracting from Outline B\n";
                return -1;
            }
        }

    }

    cout << "[OK]: All tests passed\n";
    return 0;
}


int test_arcs( void ) {
    DLL_MCAD_SEGMENT seg1(true);
    DLL_MCAD_SEGMENT seg2(true);

    MCAD_POINT c1[3];   // parameters for Circle 1
    MCAD_POINT c2[3];   // parameters for Circle 2

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

    seg1.SetParams(c1[0], c1[1], c1[2], false);
    seg2.SetParams(c2[0], c2[1], c2[2], false);

    DLL_IGES_GEOM_PCB otln( true );
    bool error = false;

    if( !otln.AddSegment(seg1, error) )
    {
        cout << "* [FAIL]: could not add segment to outline, error: " << error << "\n";
        return -1;
    }

    if( seg1.IsValid() ) {
        cout << "* [FAIL]: segment seg1 should not be valid\n";
        return -1;
    }

    bool ret = false;

    if( !otln.IsClosed( ret ) || !ret )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    if( !otln.SubOutline( seg2, error ) )
    {
        cout << "* [FAIL]: could not subtract an outline, error: " << error << "\n";
        return -1;
    }

    if( seg2.IsValid() ) {
        cout << "* [FAIL]: segment seg2 should not be valid\n";
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( seg2, true, error ) )
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( seg2, true, error ) )
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( seg2, true, error ) )
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.SubOutline( seg2, error ) )
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( seg2, true, error ) )
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.AddCutout( seg2, true, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    if( 1 )
    {
        // ensure that we apply a cutout which concides with endpoints on the outline
        DLL_MCAD_SEGMENT s0( true );
        c2[0].x = 0.0;
        c2[0].y = 0.0;
        c2[1].x = 2.0;
        c2[1].y = 0.0;
        c2[2].x = 2.0;
        c2[2].y = 0.0;
        s0.SetParams( c2[0], c2[1], c2[2], false );

        DLL_MCAD_SEGMENT s1( true );
        c2[0].x = 0.0;
        c2[0].y = -2.0;
        c2[1].x = 1.0;
        c2[1].y = -2.0;
        c2[2].x = 1.0;
        c2[2].y = -2.0;
        s1.SetParams( c2[0], c2[1], c2[2], false );

        MCAD_POINT* iList = NULL;
        int nItems = 0;
        MCAD_INTERSECT_FLAG flag;
        s0.GetIntersections( s1.GetRawPtr(), iList, nItems, flag );

        c2[0].x = 0.0;
        c2[0].y = -1.2;
        s1.SetParams( c2[0], iList[0], iList[0], false );
        delete [] iList;

        if( !otln.SubOutline( s1, error ) )
        {
            cout << "* [FAIL]: could not add a cutout which intersects endpoints, error: " << error << "\n";
            return -1;
        }
    }

    if( 1 )
    {
        DLL_MCAD_SEGMENT seg3( true );
        // radius: 0.5, c(0,0)
        c2[0].x = 0.0;
        c2[0].y = 0.0;
        c2[1].x = 0.5;
        c2[1].y = 0.0;
        c2[2].x = 0.5;
        c2[2].y = 0.0;
        seg3.SetParams( c2[0], c2[1], c2[2], false );

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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.SubOutline( seg2, error ) )
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
        seg2.NewSegment();
        seg2.SetParams( c2[0], c2[1], c2[2], false );

        if( !otln.SubOutline( seg2, error ) )
        {
            cout << "* [FAIL]: could not add a cutout, error: " << error << "\n";
            return -1;
        }
    }

    if( !otln.IsContiguous( ret ) || !ret )
    {
        cout << "* [FAIL]: outline was not contiguous\n";
        return -1;
    }

    DLL_IGES model;
    IGES_ENTITY_144** res = NULL;
    int nSurfs = 0;

    if( !otln.GetVerticalSurface( model.GetRawPtr(), error, res, nSurfs, 1.5, -1.5 ) )
    {
        cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
        return -1;
    }

    delete [] res;

    model.Write( "test_oln_arcs.igs", true );
    return 0;
}


int test_lines( void )
{
    DLL_MCAD_SEGMENT sides[4] = { DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ),
                                  DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ) };
    MCAD_POINT v[4];

    v[0].x = 10.0;
    v[0].y = 10.0;
    v[1].x = -10.0;
    v[1].y = 10.0;
    v[2].x = -10.0;
    v[2].y = -10.0;
    v[3].x = 10.0;
    v[3].y = -10.0;

    sides[0].SetParams(v[0], v[1]);
    sides[1].SetParams(v[1], v[2]);
    sides[2].SetParams(v[2], v[3]);
    sides[3].SetParams(v[3], v[0]);

    DLL_IGES_GEOM_PCB otln( true );
    bool error = false;

    if( !otln.AddSegment( sides[0], error )
        || !otln.AddSegment( sides[1], error )
        || !otln.AddSegment( sides[2], error )
        || !otln.AddSegment( sides[3], error ) )
    {
        cout << "* [FAIL]: could not add segment to outline\n";
        return -1;
    }

    bool ret = false;

    if( !otln.IsClosed( ret ) || !ret )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    MCAD_POINT c1[2];   // parameters for circles

    if( 1 )
    {
        // nibble out 8 bits
        DLL_MCAD_SEGMENT circ( true );

        // radius: 0.5, c(10,10)
        c1[0].x = 10.0;
        c1[0].y = 10.0;
        c1[1].x = 10.5;
        c1[1].y = 10.0;

        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 1.0, c(0,10)
        c1[0].x = 0.0;
        c1[0].y = 10.0;
        c1[1].x = 1.0;
        c1[1].y = 10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 1.5, c(-10,10)
        c1[0].x = -10.0;
        c1[0].y = 10.0;
        c1[1].x = -8.5;
        c1[1].y = 10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 2.0, c(-10,0)
        c1[0].x = -10.0;
        c1[0].y = 0.0;
        c1[1].x = -8;
        c1[1].y = 0.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 2.5, c(-10,-10)
        c1[0].x = -10.0;
        c1[0].y = -10.0;
        c1[1].x = -7.5;
        c1[1].y = -10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 3.0, c(0,-10)
        c1[0].x = 0.0;
        c1[0].y = -10.0;
        c1[1].x = 3.0;
        c1[1].y = -10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 3.5, c(10,-10)
        c1[0].x = 10.0;
        c1[0].y = -10.0;
        c1[1].x = 13.5;
        c1[1].y = -10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }

        // radius: 4.0, c(10,0)
        c1[0].x = 10.0;
        c1[0].y = 0.0;
        c1[1].x = 14.0;
        c1[1].y = 0.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add a cutout\n";
            return -1;
        }
    }

    DLL_MCAD_SEGMENT hole( true );

    // radius: 4.5, c(0,0)
    c1[0].x = 0.0;
    c1[0].y = 0.0;
    c1[1].x = 4.5;
    c1[1].y = 0.0;

    hole.SetParams( c1[0], c1[1], c1[1], false );

    if( !otln.AddCutout( hole, true, error ) )
    {
        cout << "* [FAIL]: could not add a cutout\n";
        return -1;
    }

    if( !otln.IsContiguous( ret ) || !ret )
    {
        cout << "* [FAIL]: outline was not contiguous\n";
        return -1;
    }

    DLL_IGES model;
    IGES_ENTITY_144** res = NULL;
    int nSurfs = 0;

    if( !otln.GetVerticalSurface( model.GetRawPtr(), error, res, nSurfs, 0.8, -0.8 ) )
    {
        cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
        return -1;
    }

    delete [] res;
    model.Write( "test_oln_lines.igs", true );
    return 0;
}


int test_addr( void )
{
    DLL_MCAD_SEGMENT sides[4] = { DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ),
                                  DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ) };
    MCAD_POINT v[4];

    if( 0 )
    {
        v[0].x = 10.0;
        v[0].y = 10.0;
        v[1].x = -10.0;
        v[1].y = 10.0;
        v[2].x = -10.0;
        v[2].y = -10.0;
        v[3].x = 10.0;
        v[3].y = -10.0;
    }
    else
    {
        v[0].x = 10.0;
        v[0].y = 10.0;
        v[1].x = 10.0;
        v[1].y = -10.0;
        v[2].x = -10.0;
        v[2].y = -10.0;
        v[3].x = -10.0;
        v[3].y = 10.0;
    }

    sides[0].SetParams(v[0], v[1]);
    sides[1].SetParams(v[1], v[2]);
    sides[2].SetParams(v[2], v[3]);
    sides[3].SetParams(v[3], v[0]);

    DLL_IGES_GEOM_PCB otln( true );
    bool error = false;

    if( !otln.AddSegment( sides[0], error )
        || !otln.AddSegment( sides[1], error )
        || !otln.AddSegment( sides[2], error )
        || !otln.AddSegment( sides[3], error ) )
    {
        cout << "* [FAIL]: could not add segment to outline\n";
        return -1;
    }

    bool ret = false;

    if( !otln.IsClosed( ret ) || !ret )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    MCAD_POINT c1[2];   // parameters for circles
    DLL_MCAD_SEGMENT circ( false );

    if( 1 )
    {
        // add an equally wide circle to the top part

        // radius: 10, c(0,10)
        c1[0].x = 0.0;
        c1[0].y = 10.0;
        c1[1].x = 10.0;
        c1[1].y = 10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.AddOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add an outline\n";
            return -1;
        }
    }

    if( 1 )
    {
        // add an equally wide circle to the LHS but protruding slightly;
        // the case of an equally wide circle which is slightly sunken
        // is prohibited by the 2-point intersection constraint so we
        // do not attempt that case.
        // radius: 10.0, c(-12,0)
        c1[0].x = -12.0;
        c1[0].y = 0.0;
        c1[1].x = -2.0;
        c1[1].y = 0.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.AddOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add an outline\n";
            return -1;
        }
    }
    else
    {
        // cut out a circle tangent to TOP and BOT
        // radius: 10, c(-10,0)
        c1[0].x = -10.0;
        c1[0].y = 0.0;
        c1[1].x = 0.0;
        c1[1].y = 0.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.SubOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add an outline\n";
            return -1;
        }
    }

    if( 1 )
    {
        // add a circle to the BR corner
        // radius: 5, c(10,-10)
        c1[0].x = 10.0;
        c1[0].y = -10.0;
        c1[1].x = 15.0;
        c1[1].y = -10.0;

        circ.NewSegment();
        circ.SetParams( c1[0], c1[1], c1[1], false );

        if( !otln.AddOutline( circ, error ) )
        {
            cout << "* [FAIL]: could not add an outline\n";
            return -1;
        }
    }

    if( !otln.IsContiguous( ret ) || !ret )
    {
        cout << "* [FAIL]: outline was not contiguous\n";
        return -1;
    }

    DLL_IGES model;
    IGES_ENTITY_144** res = NULL;
    int nSurfs = 0;

    if( !otln.GetVerticalSurface( model.GetRawPtr(), error, res, nSurfs, 0.8, -0.8 ) )
    {
        cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
        return -1;
    }

    delete [] res;
    model.Write( "test_oln_addr.igs", true );
    return 0;
}


int test_otln( bool subs, bool primeA )
{
    DLL_MCAD_SEGMENT sides[4] = { DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ),
                                  DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ) };
    MCAD_POINT v[4];

    v[0].x = 10.0;
    v[0].y = 10.0;
    v[1].x = -10.0;
    v[1].y = 10.0;
    v[2].x = -10.0;
    v[2].y = -10.0;
    v[3].x = 10.0;
    v[3].y = -10.0;

    sides[0].SetParams(v[0], v[1]);
    sides[1].SetParams(v[1], v[2]);
    sides[2].SetParams(v[2], v[3]);
    sides[3].SetParams(v[3], v[0]);

    DLL_IGES_GEOM_PCB otlnB( true );
    bool error = false;

    if( !otlnB.AddSegment( sides[0], error )
        || !otlnB.AddSegment( sides[1], error )
        || !otlnB.AddSegment( sides[2], error )
        || !otlnB.AddSegment( sides[3], error ) )
    {
        cout << "* [FAIL]: could not add segment to outline\n";
        return -1;
    }

    bool ret = false;

    if( !otlnB.IsClosed( ret ) || !ret )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    MCAD_POINT c1[2];   // parameters for circles
    DLL_MCAD_SEGMENT circ[6] = { DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ),
                                 DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ),
                                 DLL_MCAD_SEGMENT( true ), DLL_MCAD_SEGMENT( true ) };
    DLL_IGES_GEOM_PCB otln[6] = { DLL_IGES_GEOM_PCB( true ), DLL_IGES_GEOM_PCB( true ),
                                  DLL_IGES_GEOM_PCB( true ), DLL_IGES_GEOM_PCB( true ),
                                  DLL_IGES_GEOM_PCB( true ), DLL_IGES_GEOM_PCB( true ) };

    // create the various circular outlines

    // radius: 10, c(0,10)
    c1[0].x = 0.0;
    c1[0].y = 10.0;
    c1[1].x = 10.0;
    c1[1].y = 10.0;
    circ[0].SetParams( c1[0], c1[1], c1[1], false );
    otln[0].AddSegment( circ[0], error );

    // radius: 3, c(10,10)
    c1[0].x = 10.0;
    c1[0].y = 10.0;
    c1[1].x = 13.0;
    c1[1].y = 10.0;
    circ[1].SetParams( c1[0], c1[1], c1[1], false );
    otln[1].AddSegment( circ[1], error );

    // radius: 3, c(-10,10)
    c1[0].x = -10.0;
    c1[0].y = 10.0;
    c1[1].x = -7.0;
    c1[1].y = 10.0;
    circ[2].SetParams( c1[0], c1[1], c1[1], false );
    otln[2].AddSegment( circ[2], error );

    // radius: 3, c(10,-10)
    c1[0].x = 10.0;
    c1[0].y = -10.0;
    c1[1].x = 13.0;
    c1[1].y = -10.0;
    circ[3].SetParams( c1[0], c1[1], c1[1], false );
    otln[3].AddSegment( circ[3], error );

    // radius: 3, c(-10,-10)
    c1[0].x = -10.0;
    c1[0].y = -10.0;
    c1[1].x = -7.0;
    c1[1].y = -10.0;
    circ[4].SetParams( c1[0], c1[1], c1[1], false );
    otln[4].AddSegment( circ[4], error );

    // radius: 3, c(0,-10)
    c1[0].x = 0.0;
    c1[0].y = -10.0;
    c1[1].x = 3.0;
    c1[1].y = -10.0;
    circ[5].SetParams( c1[0], c1[1], c1[1], false );
    otln[5].AddSegment( circ[5], error );

    if( !otln[0].IsClosed( ret ) || !ret )
    {
        cout << "* [FAIL]: outline is not closed\n";
        return -1;
    }

    if( primeA )
    {
        if( !subs )
        {
            // add outline B to A
            if( !otln[0].AddOutline( otlnB, error ) )
            {
                cout << "* [FAIL]: could not add an outline\n";
                return -1;
            }

            // add all other outlines to A
            for( int i = 1; i < 6; ++i )
            {
                if( !otln[0].AddOutline( otln[i], error ) )
                {
                    cout << "* [FAIL]: could not add outline " << i << "\n";
                    return -1;
                }
            }
        }
        else
        {
            // subtract outline B from A
            if( !otln[0].SubOutline( otlnB, error ) )
            {
                cout << "* [FAIL]: could not subtract an outline\n";
                return -1;
            }

            // subtract next 2 outlines from A
            for( int i = 1; i < 3; ++i )
            {
                if( !otln[0].SubOutline( otln[i], error ) )
                {
                    cout << "* [FAIL]: could not subtract outline " << i << "\n";
                    return -1;
                }
            }
        }

        if( !otln[0].IsContiguous( ret ) || !ret )
        {
            cout << "* [FAIL]: outline was not contiguous\n";
            return -1;
        }

        DLL_IGES model;
        IGES_ENTITY_144** res = NULL;
        int nSurfs = 0;

        if( !otln[0].GetVerticalSurface( model.GetRawPtr(), error, res, nSurfs, 0.8, -0.8 ) )
        {
            cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
            return -1;
        }

        delete [] res;

        if( subs )
            model.Write( "test_olnA_sub.igs", true );
        else
            model.Write( "test_olnA_add.igs", true );
    }
    else
    {
        if( !subs )
        {
            // add outline A to B
            if( !otlnB.AddOutline( otln[0], error ) )
            {
                cout << "* [FAIL]: could not add an outline\n";
                return -1;
            }

            // add all other outlines to B
            for( int i = 1; i < 6; ++i )
            {
                if( !otlnB.AddOutline( otln[i], error ) )
                {
                    cout << "* [FAIL]: could not add outline " << i << "\n";
                    return -1;
                }
            }

        }
        else
        {
            // subtract outline A from B
            if( !otlnB.SubOutline( otln[0], error ) )
            {
                cout << "* [FAIL]: could not subtract an outline\n";
                return -1;
            }

            // subtract all other outlines from B
            for( int i = 1; i < 6; ++i )
            {
                if( !otlnB.SubOutline( otln[i], error ) )
                {
                    cout << "* [FAIL]: could not subtract outline " << i << "\n";
                    return -1;
                }
            }

        }

        if( !otlnB.IsContiguous( ret ) || !ret )
        {
            cout << "* [FAIL]: outline was not contiguous\n";
            return -1;
        }

        DLL_IGES model;
        IGES_ENTITY_144** res = NULL;
        int nSurfs = 0;

        if( !otlnB.GetVerticalSurface( model.GetRawPtr(), error, res, nSurfs, 0.8, -0.8 ) )
        {
            cout << "* [FAIL]: could not create vertical structures, error: " << error << "\n";
            return -1;
        }

        delete [] res;

        if( subs )
            model.Write( "test_olnB_sub.igs", true );
        else
            model.Write( "test_olnB_add.igs", true );
    }

    return 0;
}
