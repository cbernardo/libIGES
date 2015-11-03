/*
 * file: mcad_elements.h
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

#ifndef MCAD_ELEMENTS_H
#define MCAD_ELEMENTS_H

#include <libigesconf.h>

#ifdef USE_SISL
    enum MCAD_SEGTYPE
    {
        MCAD_SEGTYPE_NONE = 0,
        MCAD_SEGTYPE_LINE = 1,
        MCAD_SEGTYPE_ARC = 2,
        MCAD_SEGTYPE_CIRCLE = 4
    };

    // flag used for geometry intersection information
    // Note that many of the cases in which we flag invalid geometry may in fact
    // not be invalid geometry but the geom_* code is intended to help in an ECAD
    // application rather than provide full MCAD support. This decision keeps this
    // code simpler while forcing the ECAD designer to put more thought into the
    // manufacturing design of the board.
    enum MCAD_INTERSECT_FLAG
    {
        MCAD_IFLAG_NONE = 0,    // no special conditions to report
        MCAD_IFLAG_ENDPOINT,    // intersection is at the endpoint of a segment
        MCAD_IFLAG_TANGENT,     // intersection is at a tangent (invalid geometry)
        MCAD_IFLAG_EDGE,        // intersection is along an edge; result contains
        // start and end point of the edge. Initially the code
        // shall enforce simple geometry so an EDGE flag
        // shall be treated as invalid geometry.
            MCAD_IFLAG_INSIDE,      // this circle is inside the given circle (invalid geometry)
        // or this arc is inside the given arc.
            MCAD_IFLAG_ENCIRCLES,   // this circle envelopes the given circle (invalid geometry)
        MCAD_IFLAG_OUTSIDE,     // this arc is outside the given arc
        MCAD_IFLAG_IDENT,       // 2 circles are identical
        MCAD_IFLAG_MULTIEDGE    // arcs overlap on 2 edges (invalid geometry)
    };
#endif

struct MCAD_MATRIX;
struct MCAD_API MCAD_POINT
{
    double x;
    double y;
    double z;

    MCAD_POINT();
    MCAD_POINT( const MCAD_POINT& p );
    MCAD_POINT( const double x, const double y, const double z );
    MCAD_POINT& operator*=( const double scalar );
    MCAD_POINT& operator+=( const MCAD_POINT& v );
    MCAD_POINT  const operator+( const MCAD_POINT& v );
    MCAD_POINT& operator-=( const MCAD_POINT& v );
};

// V1 = V0 * scalar
MCAD_API MCAD_POINT operator*(const MCAD_POINT& v, const double scalar);
// V1 = scalar * V0
MCAD_API MCAD_POINT operator*(const double scalar, const MCAD_POINT& v);
// pX = p0 - p1
MCAD_API MCAD_POINT operator-(const MCAD_POINT& p0, const MCAD_POINT& p1);

struct MCAD_API MCAD_MATRIX
{
    double v[3][3];

    MCAD_MATRIX();
    MCAD_MATRIX( const MCAD_MATRIX& m );

    // Matrix *= scalar
    MCAD_MATRIX& operator*=( double scalar );
    // MatrixA *= MatrixB
    MCAD_MATRIX& operator*=( const MCAD_MATRIX& m );
    MCAD_MATRIX& operator+=( const MCAD_MATRIX& m );
    MCAD_MATRIX  operator+( const MCAD_MATRIX& m );
    MCAD_MATRIX& operator-=( const MCAD_MATRIX& m );
    MCAD_MATRIX operator-( const MCAD_MATRIX& m );
};

// C = MatrixA * MatrixB
MCAD_API MCAD_MATRIX operator*(const MCAD_MATRIX& m, const MCAD_MATRIX& n);
// C = MatrixA * scalar
MCAD_API MCAD_MATRIX operator*(const MCAD_MATRIX& lhs, double rhs);
// C = scalar * MatrixA
MCAD_API MCAD_MATRIX operator*(double scalar, const MCAD_MATRIX& m);
// V1 = MatrixA * V0
MCAD_API MCAD_POINT operator*(const MCAD_MATRIX& m, const MCAD_POINT& v);


struct MCAD_API MCAD_TRANSFORM
{
    MCAD_TRANSFORM();
    MCAD_TRANSFORM( const MCAD_TRANSFORM& t );
    MCAD_TRANSFORM( const MCAD_MATRIX& m, const MCAD_POINT& v );

    MCAD_MATRIX R;
    MCAD_POINT  T;

    MCAD_TRANSFORM& operator*=(const MCAD_TRANSFORM& m);
    MCAD_TRANSFORM& operator*=(const double scalar);
};
// scalar * TX
MCAD_API MCAD_TRANSFORM operator*(const double scalar, const MCAD_TRANSFORM& m);
// TX0 * TX1
MCAD_API MCAD_TRANSFORM operator*(const MCAD_TRANSFORM& m, const MCAD_TRANSFORM& n);
// TX * V (perform a transform + offset)
MCAD_API MCAD_POINT operator*(const MCAD_TRANSFORM& m, const MCAD_POINT& v);

#endif  // MCAD_ELEMENTS_H
