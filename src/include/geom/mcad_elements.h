/*
 * file: mcad_elements.h
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

#ifndef MCAD_ELEMENTS_H
#define MCAD_ELEMENTS_H

#include <libigesconf.h>

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
