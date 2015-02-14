/*
 * file: entity430.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 430: Solid Instance Entity, Section 4.146 p.584/612+
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

#ifndef ENTITY_430_H
#define ENTITY_430_H

#include "iges_entity.h"

// NOTE:
// Allowed entities:
//      + Primitive
//      + Boolean Tree
//      + Solid Assembly
//      + Solid Instance
//      + Manifold Solid B-Rep Object
//
// Entity Use Flag: 02
//
// The associated parameter data are:
// + N: Integer: Number of paired items in list
// + PTR(1) .. PTR(N): DE
// + PTRM(1) .. PTRM(N): Transform Matrix for DE(x)
//
// Forms:
//  0: The referenced solid is a Primitive, Solid Instance, Boolean Tree, or Solid Assembly
//  1: The referenced solid is a Manifold BREP Object
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_430 : IGES_ENTITY
{
private:
    // XXX - TO BE IMPLEMENTED

public:
    // XXX - TO BE IMPLEMENTED

}

#endif  // ENTITY_430_H
