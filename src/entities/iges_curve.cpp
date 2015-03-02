/*
 * file: iges_curve.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Base entity of all IGES Entity classes.
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

// Note: This base class must never be instantiated.

#include <iomanip>
#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <all_entities.h>
#include <iges_io.h>


IGES_CURVE::IGES_CURVE(IGES* aParent) : IGES_ENTITY( aParent )
{
    return;
}   // IGES_CURVE::IGES_CURVE(IGES*)


IGES_CURVE::~IGES_CURVE()
{
    return;
}
