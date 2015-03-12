/*
 * file: iges_helpers.h
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

#ifndef IGES_HELPERS_H
#define IGES_HELPERS_H

#include <string>
#include <iges_base.h>
#include <iges_elements.h>

// return true if the 2 points match to within 'minRes'
bool PointMatches( IGES_POINT p1, IGES_POINT p2, double minRes );

// check and renormalize a vector; return false if vector is invalid
bool CheckNormal( double& X, double &Y, double& Z );

// print out an IGES transform
class IGES_TRANSFORM;
void print_transform( const IGES_TRANSFORM* T );

// print out an IGES matrix
class IGES_MATRIX;
void print_matrix( const IGES_MATRIX* m );

// print out an IGES point (decimal places limited to 3)
class IGES_POINT;
void print_vec( const IGES_POINT* p );

#endif  // IGES_HELPERS_H
