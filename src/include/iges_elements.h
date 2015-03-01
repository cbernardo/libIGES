/*
 * file: iges_elements.h
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

#ifndef IGES_ELEMENTS_H
#define IGES_ELEMENTS_H

struct IGES_MATRIX;
struct IGES_POINT
{
    double x;
    double y;
    double z;

    IGES_POINT();
    IGES_POINT( const IGES_POINT& p );
    IGES_POINT( const double x, const double y, const double z );
    IGES_POINT& operator*=( const double scalar );
    IGES_POINT& operator+=( const IGES_POINT& v );
    IGES_POINT  operator+( const IGES_POINT& v );
    IGES_POINT& operator-=( const IGES_POINT& v );
    IGES_POINT  operator-( const IGES_POINT& v );
};

// V1 = V0 * scalar
IGES_POINT operator*( const IGES_POINT& v, const double scalar );
// V1 = scalar * V0
IGES_POINT operator*( const double scalar, const IGES_POINT& v );

struct IGES_MATRIX
{
    double v[3][3];

    IGES_MATRIX();
    IGES_MATRIX( const IGES_MATRIX& m );

    // Matrix *= scalar
    IGES_MATRIX& operator*=( double scalar );
    // MatrixA *= MatrixB
    IGES_MATRIX& operator*=( const IGES_MATRIX& m );
    IGES_MATRIX& operator+=( const IGES_MATRIX& m );
    IGES_MATRIX  operator+( const IGES_MATRIX& m );
    IGES_MATRIX& operator-=( const IGES_MATRIX& m );
    IGES_MATRIX operator-( const IGES_MATRIX& m );
};

// C = MatrixA * MatrixB
IGES_MATRIX operator*( const IGES_MATRIX& m, const IGES_MATRIX& n );
// C = MatrixA * scalar
IGES_MATRIX operator*( const IGES_MATRIX& lhs, double rhs );
// C = scalar * MatrixA
IGES_MATRIX operator*( double scalar, const IGES_MATRIX& m );
// V1 = MatrixA * V0
IGES_POINT operator*( const IGES_MATRIX& m, const IGES_POINT& v );


struct IGES_TRANSFORM
{
    IGES_TRANSFORM();
    IGES_TRANSFORM( const IGES_TRANSFORM& t );
    IGES_TRANSFORM( const IGES_MATRIX& m, const IGES_POINT& v );

    IGES_MATRIX R;
    IGES_POINT  T;

    IGES_TRANSFORM& operator*=(const IGES_TRANSFORM& m);
    IGES_TRANSFORM& operator*=(const double scalar);
};
// scalar * TX
IGES_TRANSFORM operator*( const double scalar, const IGES_TRANSFORM& m );
// TX0 * TX1
IGES_TRANSFORM operator*( const IGES_TRANSFORM& m, const IGES_TRANSFORM& n );
// TX * V (perform a transform + offset)
IGES_POINT operator*( const IGES_TRANSFORM& m, const IGES_POINT& v );

#endif  // IGES_ELEMENTS_H
