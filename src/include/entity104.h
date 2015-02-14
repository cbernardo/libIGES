/*
 * file: entity104.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 104: Conic Arc, Section 4.5, p.74+ (102+)
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ENTITY_104_H
#define ENTITY_104_H

#include "iges_entity.h"

// NOTE:
//
// A Conic Arc, like a Circle, is constrained to a plane parallel to Xt-Yt.
// The curve is a subset of the general equation:
// AX^2 + BXY + CY^2 + DX + EY + F = 0
// The axis of the conic must be parallel to Xt or Yt; any other orientation
// is achieved via the Transformation Matrix. Degenerate forms such as
// Point and Line are prohibited. Form 0, allowed in previous versions of
// the specification, is deprecated.
//
// The associated parameter data are:
// + A: Real: Coefficient A
// + B: Real: Coefficient A
// + C: Real: Coefficient A
// + D: Real: Coefficient A
// + E: Real: Coefficient A
// + F: Real: Coefficient A
// + ZT: Real: Zt displacement of the arc from the Xt-Yt plane
// + X1: Real: Start point, abscissa
// + Y1: Real: Start point, ordinate
// + X2: Real: End point, abscissa
// + Y2: Real: End point, ordinate
//
// Forms: (note: Form 0 does not exist)
//  1: ellipse
//  2: hyperbola
//  3: parabola
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_104 : IGES_ENTITY
{
private:
    // XXX - TO BE IMPLEMENTED

public:
    // XXX - TO BE IMPLEMENTED

}

#endif  // ENTITY_104_H
