/*
 * file: entity164.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 164: Solid of Linear Extrusion, Section 4.44, p.204(232+)
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

#ifndef ENTITY_164_H
#define ENTITY_164_H

#include "iges_entity.h"

// NOTE:
// The associated parameter data are:
// + PTR: Index: DE of closed planar curve
// + L: Real: Length of extrusion along positive vector direction
// + I1: Real: Unit vector in extrusion direction
// + J1: Real:
// + K1: Real:
//
// Forms: 0
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_164 : IGES_ENTITY
{
private:
    // XXX - TO BE IMPLEMENTED

public:
    // XXX - TO BE IMPLEMENTED

}

#endif  // ENTITY_164_H
