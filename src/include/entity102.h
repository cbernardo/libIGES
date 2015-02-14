/*
 * file: entity102.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 102: Composite Curve, Section 4.4, p.69+ (97+)
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

#ifndef ENTITY_102_H
#define ENTITY_102_H

#include "iges_entity.h"

// NOTE:
//
// A Composite Curve is an ordered list of entities including Point,
// Connect Point, and generic parameterized curves (except for Composite Curve itself).
// The order of listing of member entities is the order in which they appear in
// the parameter list. Other acceptable entities include:
// + Line
// + Circle
// + Spline
// + Conic
//
// + Subordinate entities (items in the list) must have a physical dependence
//   relationship with this entity. The first point of this entity is the first
//   point of the first list entity and the last point of this entity is the
//   last point of the last list entity. Within the list, the last point of
//   entity N must be the same as the first point of entity N+1.
//
// + Note restrictions on the use of the Point and Connect Point Entities, p.69 (97)
//
// + When HIERARCHY is Global Defer, the following entries are ignored and may
//   be defaulted:
//   + Line Font Pattern
//   + Line Weight
//   + Color Number
//   + Level
//   + View
//   + Blank Status
//
// The associated parameter data are:
// + N: Int: Number of member entities
// + DE(1): Int: Index to DE of 1st member entity
// + DE(N): Int: Index to DE of last member entity
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_102 : IGES_ENTITY
{
private:
    // XXX - TO BE IMPLEMENTED

public:
    // XXX - TO BE IMPLEMENTED

}

#endif  // ENTITY_102_H
