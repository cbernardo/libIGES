/*
 * file: geom_wall.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: object to aid in the creation of a rectangular
 * surface within IGES
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

#include <geom_wall.h>


IGES_GEOM_WALL::IGES_GEOM_WALL()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


IGES_GEOM_WALL::IGES_GEOM_WALL( IGES_POINT p0, IGES_POINT p1, IGES_POINT p2, IGES_POINT p3 )
{
    // XXX - TO BE IMPLEMENTED
    return;
}


IGES_GEOM_WALL::~IGES_GEOM_WALL()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


void IGES_GEOM_WALL::init( void )
{
    vnorm = IGES_POINT( 0.0, 0.0, 1.0 );
    plane = NULL;
    side0 = NULL;
    side1 = NULL;
    side2 = NULL;
    side3 = NULL;

    return;
}


bool IGES_GEOM_WALL::SetParams( IGES_POINT p0, IGES_POINT p1, IGES_POINT p2, IGES_POINT p3 )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_GEOM_WALL::Instantiate( IGES* model )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}
