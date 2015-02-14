/*
 * file: entityNULL.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 0: NULL, Section 4.2, p.65 (93)
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

#ifndef ENTITY_NULL_H
#define ENTITY_NULL_H

#include "iges_entity.h"

// NOTE:
// It is impossible to reliably determine the entity type
// of a DE entry which has been set to NULL; as such, all
// NULL entities may be safely destroyed upon verification
// of the relationships of entities ('association' method
// has been executed). Destroying this essentially unrecoverable
// data will help ensure neater output files. Note that the
// deletion of dangling entities is another matter.
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + Level
// + View
// + Transformation Matrix
// + Label Display Association
// + Line weight
// + Color number
// + Form number
//

class IGES_ENTITY_NULL : IGES_ENTITY
{
private:
    // XXX - TO BE IMPLEMENTED

public:
    // XXX - TO BE IMPLEMENTED

}

#endif  // ENTITY_NULL_H
