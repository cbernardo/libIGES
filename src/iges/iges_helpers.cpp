/*
 * file: iges_helpers.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: miscellaneous useful functions
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

#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>

#include <error_macros.h>
#include <iges_helpers.h>


using namespace std;


// return true if the 2 points match to within 'minRes'
// or best effort if the numbers are big
bool PointMatches( IGES_POINT p1, IGES_POINT p2, double minRes )
{
    // determine if we can in fact meet the minRes specification
    double dN = abs( p1.x*p1.x + p1.y*p1.y + p1.z*p1.z );
    double dV = abs( p2.x*p2.x + p2.y*p2.y + p2.z*p2.z );

    double rN = dN / minRes;
    double rV = dV / minRes;

    // if minRes cannot be guaranteed in the calculation
    // then create a generous new boundary which should
    // ideally minimize mischaracterizations
    if( dN > 1e15 || dV > 1e15 )
    {
        rN = 1.0 / sqrt(max( dN, dV ));
        p1 *= rN;
        p2 *= rN;
        minRes *= rN;
    }

    minRes *= 3.0001 * minRes;

    if( minRes < 3.0e-30 )
        minRes = 3.0e-30;

    dN = p2.x - p1.x;
    dV = p2.y - p1.y;
    rN = p2.z - p1.z;
    rV = dN*dN + dV*dV + rN*rN;

    if( rV > minRes )
        return false;

    return true;
}


bool CheckNormal( double& X, double &Y, double& Z )
{
    double dN = X*X + Y*Y + Z*Z;

    if( dN < 1e-6 )
    {
        ERRMSG << "\n + [INFO] bad vector (cannot be normalized)\n";
        return false;
    }

    double dV = dN - 1.0;

    if( dV < -1e-6 || dV > 1e-6 )
    {
        ERRMSG << "\n + [INFO] renormalizing unit vector\n";
        dV = sqrt(dN);
        X /= dV;
        Y /= dV;
        Z /= dV;
    }

    return true;
}


void print_transform( const IGES_TRANSFORM* T )
{
    cout << setprecision( 3 );
    cout << "R1: " << T->R.v[0][0] << ", " << T->R.v[0][1] << ", " << T->R.v[0][2] << ",  T.x = " << T->T.x << "\n";
    cout << "R2: " << T->R.v[1][0] << ", " << T->R.v[1][1] << ", " << T->R.v[1][2] << ",  T.y = " << T->T.y << "\n";
    cout << "R3: " << T->R.v[2][0] << ", " << T->R.v[2][1] << ", " << T->R.v[2][2] << ",  T.z = " << T->T.z << "\n";
    return;
}


void print_matrix( const IGES_MATRIX* m )
{
    cout << setprecision( 3 );
    cout << "R1: " << m->v[0][0] << ", " << m->v[0][1] << ", " << m->v[0][2] << "\n";
    cout << "R2: " << m->v[1][0] << ", " << m->v[1][1] << ", " << m->v[1][2] << "\n";
    cout << "R3: " << m->v[2][0] << ", " << m->v[2][1] << ", " << m->v[2][2] << "\n";
    return;
}


void print_vec( const IGES_POINT* p )
{
    cout << setprecision( 3 );
    cout << "V: " << p->x << ", " << p->y << ", " << p->z << "\n";
    return;
}


// calculate the normal given points p0, p1, p2
bool CalcNormal( const IGES_POINT* p0, const IGES_POINT* p1, const IGES_POINT* p2, IGES_POINT* pn )
{
    if( !p0 || !p1 || !p2 || !pn )
    {
        ERRMSG << "\n + [ERROR] NULL pointer passed as an argument\n";
        return false;
    }

    IGES_POINT tp0( *p0 );
    IGES_POINT tp1( *p1 );
    IGES_POINT tp2( *p2 );

    IGES_POINT t0 = tp1 - tp0;
    IGES_POINT t1 = tp2 - tp0;

    pn->x = t0.y * t1.z - t0.z * t1.y;
    pn->y = t0.z * t1.x - t0.x * t1.z;
    pn->z = t0.x * t1.y - t0.y * t1.x;

    return CheckNormal( pn->x, pn->y, pn->z );
}
