/*
 * file: mcad_elements.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Basic geometry elements and their operators
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


#include <cstring>
#include <geom/mcad_elements.h>

MCAD_POINT::MCAD_POINT()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
    return;
}


MCAD_POINT::MCAD_POINT( const MCAD_POINT& p )
{
    x = p.x;
    y = p.y;
    z = p.z;
    return;
}


MCAD_POINT::MCAD_POINT( const double x, const double y, const double z )
{
    MCAD_POINT::x = x;
    MCAD_POINT::y = y;
    MCAD_POINT::z = z;
    return;
}


MCAD_POINT& MCAD_POINT::operator*=( const double scalar )
{
    *this = *this * scalar;
    return *this;
}


MCAD_POINT& MCAD_POINT::operator+=( const MCAD_POINT& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}


MCAD_POINT const MCAD_POINT::operator+( const MCAD_POINT& v )
{
    MCAD_POINT p( *this );
    p.x += v.x;
    p.y += v.y;
    p.z += v.z;

    return p;
}


// NOTE: p1 -= p2 seems to produce a different rounding
// error than p1 = p1 - p2 and the error is large
// enough to cause nearness tests to fail.
MCAD_POINT& MCAD_POINT::operator-=( const MCAD_POINT& v )
{
    x = x - v.x;
    y = y - v.y;
    z = z - v.z;

    return *this;
}

MCAD_POINT operator*( const MCAD_POINT& v, const double scalar )
{
    MCAD_POINT pt;
    pt.x = v.x * scalar;
    pt.y = v.y * scalar;
    pt.z = v.z * scalar;
    return pt;
}


MCAD_POINT operator*( const double scalar, const MCAD_POINT& v )
{
    return v * scalar;
}


// pX = p0 - p1
MCAD_POINT operator-( const MCAD_POINT& p0, const MCAD_POINT& p1 )
{
    MCAD_POINT px = p0;
    px -= p1;
    return px;
}


MCAD_MATRIX::MCAD_MATRIX()
{
    memset( v, 0, sizeof(v) );

    // the identity transform
    for( int i = 0; i < 3; ++i )
            v[i][i] = 1.0;

    return;
}


MCAD_MATRIX::MCAD_MATRIX( const MCAD_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] = m.v[i][j];
    }
}


MCAD_MATRIX& MCAD_MATRIX::operator*=( double scalar )
{
    *this = *this * scalar;
    return *this;
}

MCAD_MATRIX& MCAD_MATRIX::operator*=( const MCAD_MATRIX& m )
{
    *this = *this * m;
    return *this;
}


MCAD_MATRIX& MCAD_MATRIX::operator+=( const MCAD_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] += m.v[i][j];
    }

    return *this;
}


MCAD_MATRIX  MCAD_MATRIX::operator+( const MCAD_MATRIX& m )
{
    MCAD_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = v[i][j] + m.v[i][j];
    }

    return tmp;
}


MCAD_MATRIX& MCAD_MATRIX::operator-=( const MCAD_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] -= m.v[i][j];
    }

    return *this;
}


MCAD_MATRIX MCAD_MATRIX::operator-( const MCAD_MATRIX& m )
{
    MCAD_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = v[i][j] - m.v[i][j];
    }

    return tmp;
}


MCAD_POINT operator*( const MCAD_MATRIX& m, const MCAD_POINT& v )
{
    MCAD_POINT pt;
    pt.x = m.v[0][0] * v.x + m.v[0][1] * v.y + m.v[0][2] * v.z;
    pt.y = m.v[1][0] * v.x + m.v[1][1] * v.y + m.v[1][2] * v.z;
    pt.z = m.v[2][0] * v.x + m.v[2][1] * v.y + m.v[2][2] * v.z;

    return pt;
}


MCAD_MATRIX operator*( const MCAD_MATRIX& m, const MCAD_MATRIX& n )
{
    MCAD_MATRIX tmp;

    // First row
    tmp.v[0][0] = m.v[0][0] * n.v[0][0] + m.v[0][1] * n.v[1][0]
    + m.v[0][2] * n.v[2][0];

    tmp.v[0][1] = m.v[0][0] * n.v[0][1] + m.v[0][1] * n.v[1][1]
    + m.v[0][2] * n.v[2][1];

    tmp.v[0][2] = m.v[0][0] * n.v[0][2] + m.v[0][1] * n.v[1][2]
    + m.v[0][2] * n.v[2][2];

    // Second row
    tmp.v[1][0] = m.v[1][0] * n.v[0][0] + m.v[1][1] * n.v[1][0]
    + m.v[1][2] * n.v[2][0];

    tmp.v[1][1] = m.v[1][0] * n.v[0][1] + m.v[1][1] * n.v[1][1]
    + m.v[1][2] * n.v[2][1];

    tmp.v[1][2] = m.v[1][0] * n.v[0][2] + m.v[1][1] * n.v[1][2]
    + m.v[1][2] * n.v[2][2];

    // Third row
    tmp.v[2][0] = m.v[2][0] * n.v[0][0] + m.v[2][1] * n.v[1][0]
    + m.v[2][2] * n.v[2][0];

    tmp.v[2][1] = m.v[2][0] * n.v[0][1] + m.v[2][1] * n.v[1][1]
    + m.v[2][2] * n.v[2][1];

    tmp.v[2][2] = m.v[2][0] * n.v[0][2] + m.v[2][1] * n.v[1][2]
    + m.v[2][2] * n.v[2][2];

    return tmp;
}


MCAD_MATRIX operator*( const MCAD_MATRIX& m, double scalar )
{
    MCAD_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = m.v[i][j] * scalar;
    }

    return tmp;
}


MCAD_MATRIX operator*( double scalar, const MCAD_MATRIX& m )
{
    return m * scalar;
}


MCAD_TRANSFORM::MCAD_TRANSFORM()
{
    return;
}

MCAD_TRANSFORM::MCAD_TRANSFORM( const MCAD_TRANSFORM& t )
{
    R = t.R;
    T = t.T;
    return;
}


MCAD_TRANSFORM::MCAD_TRANSFORM( const MCAD_MATRIX& m, const MCAD_POINT& v )
{
    R = m;
    T = v;
    return;
}


MCAD_TRANSFORM& MCAD_TRANSFORM::operator*=(const MCAD_TRANSFORM& m)
{
    T = R * m.T + T;
    R = R * m.R;
    return *this;
}


MCAD_TRANSFORM& MCAD_TRANSFORM::operator*=(const double scalar)
{
    R *= scalar;
    T *= scalar;
    return *this;
}

// scalar * TX
MCAD_TRANSFORM operator*( const double scalar, const MCAD_TRANSFORM& m )
{
    MCAD_TRANSFORM v( m );
    v *= scalar;
    return v;
}


// TX0 * TX1
MCAD_TRANSFORM operator*( const MCAD_TRANSFORM& m, const MCAD_TRANSFORM& n )
{
    MCAD_TRANSFORM v( m );
    v *= n;

    return v;
}


// TX * V (perform a transform + offset)
MCAD_POINT operator*( const MCAD_TRANSFORM& m, const MCAD_POINT& v  )
{
    MCAD_POINT p = (m.R * v) + m.T;

    return p;
}
