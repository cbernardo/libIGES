/*
 * file: iges_elements.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Basic geometry elements and their operators
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


#include <cstring>
#include <iges_elements.h>

IGES_POINT::IGES_POINT()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
    return;
}


IGES_POINT::IGES_POINT( const IGES_POINT& p )
{
    x = p.x;
    y = p.y;
    z = p.z;
    return;
}


IGES_POINT::IGES_POINT( const double x, const double y, const double z )
{
    IGES_POINT::x = x;
    IGES_POINT::y = y;
    IGES_POINT::z = z;
    return;
}


IGES_POINT& IGES_POINT::operator*=( const double scalar )
{
    *this = *this * scalar;
    return *this;
}


IGES_POINT& IGES_POINT::operator+=( const IGES_POINT& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}


IGES_POINT  IGES_POINT::operator+( const IGES_POINT& v )
{
    IGES_POINT p( *this );
    p.x += v.x;
    p.y += v.y;
    p.z += v.z;

    return p;
}


// NOTE: p1 -= p2 seems to produce a different rounding
// error than p1 = p1 - p2 and the error is large
// enough to cause nearness tests to fail.
IGES_POINT& IGES_POINT::operator-=( const IGES_POINT& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}


IGES_POINT  IGES_POINT::operator-( const IGES_POINT& v )
{
    IGES_POINT p;
    p.x -= v.x;
    p.y -= v.y;
    p.z -= v.z;
    return p;
}


IGES_POINT operator*( const IGES_POINT& v, const double scalar )
{
    IGES_POINT pt;
    pt.x = v.x * scalar;
    pt.y = v.y * scalar;
    pt.z = v.z * scalar;
    return pt;
}


IGES_POINT operator*( const double scalar, const IGES_POINT& v )
{
    return v * scalar;
}


IGES_MATRIX::IGES_MATRIX()
{
    memset( v, 0, sizeof(v) );

    // the identity transform
    for( int i = 0; i < 3; ++i )
            v[i][i] = 1.0;

    return;
}


IGES_MATRIX::IGES_MATRIX( const IGES_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] = m.v[i][j];
    }
}


IGES_MATRIX& IGES_MATRIX::operator*=( double scalar )
{
    *this = *this * scalar;
    return *this;
}

IGES_MATRIX& IGES_MATRIX::operator*=( const IGES_MATRIX& m )
{
    *this = *this * m;
    return *this;
}


IGES_MATRIX& IGES_MATRIX::operator+=( const IGES_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] += m.v[i][j];
    }

    return *this;
}


IGES_MATRIX  IGES_MATRIX::operator+( const IGES_MATRIX& m )
{
    IGES_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = v[i][j] + m.v[i][j];
    }

    return tmp;
}


IGES_MATRIX& IGES_MATRIX::operator-=( const IGES_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] -= m.v[i][j];
    }

    return *this;
}


IGES_MATRIX IGES_MATRIX::operator-( const IGES_MATRIX& m )
{
    IGES_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = v[i][j] - m.v[i][j];
    }

    return tmp;
}


IGES_POINT operator*( const IGES_MATRIX& m, const IGES_POINT& v )
{
    IGES_POINT pt;
    pt.x = m.v[0][0] * v.x + m.v[0][1] * v.y + m.v[0][2] * v.z;
    pt.y = m.v[1][0] * v.x + m.v[1][1] * v.y + m.v[1][2] * v.z;
    pt.z = m.v[2][0] * v.x + m.v[2][1] * v.y + m.v[2][2] * v.z;

    return pt;
}


IGES_MATRIX operator*( const IGES_MATRIX& m, const IGES_MATRIX& n )
{
    IGES_MATRIX tmp;

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


IGES_MATRIX operator*( const IGES_MATRIX& m, double scalar )
{
    IGES_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = m.v[i][j] * scalar;
    }

    return tmp;
}


IGES_MATRIX operator*( double scalar, const IGES_MATRIX& m )
{
    return m * scalar;
}


IGES_TRANSFORM::IGES_TRANSFORM()
{
    return;
}

IGES_TRANSFORM::IGES_TRANSFORM( const IGES_TRANSFORM& t )
{
    R = t.R;
    T = t.T;
    return;
}


IGES_TRANSFORM::IGES_TRANSFORM( const IGES_MATRIX& m, const IGES_POINT& v )
{
    R = m;
    T = v;
    return;
}


IGES_TRANSFORM& IGES_TRANSFORM::operator*=(const IGES_TRANSFORM& m)
{
    T = R * m.T + T;
    R = R * m.R;
    return *this;
}


IGES_TRANSFORM& IGES_TRANSFORM::operator*=(const double scalar)
{
    R *= scalar;
    T *= scalar;
    return *this;
}

// scalar * TX
IGES_TRANSFORM operator*( const double scalar, const IGES_TRANSFORM& m )
{
    IGES_TRANSFORM v( m );
    v *= scalar;
    return v;
}


// TX0 * TX1
IGES_TRANSFORM operator*( const IGES_TRANSFORM& m, const IGES_TRANSFORM& n )
{
    IGES_TRANSFORM v( m );
    v *= n;

    return v;
}


// TX * V (perform a transform + offset)
IGES_POINT operator*( const IGES_TRANSFORM& m, const IGES_POINT& v  )
{
    IGES_POINT p = (m.R * v) + m.T;

    return p;
}
