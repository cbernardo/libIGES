/*
 * file: geom_wall.h
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

#ifndef IGES_GEOM_WALL_H
#define IGES_GEOM_WALL_H

#include <sisl.h>
#include <all_entities.h>

class IGES;

class IGES_GEOM_WALL
{
private:
    SISLSurf* plane;        // NURBS representation of the plane
    SISLCurve* side[4];     // Representation of the 4 bounding edges
    MCAD_POINT vertex[4];   // vertices as specified by the user

    void init( void );
    void clear( void );

public:
    IGES_GEOM_WALL();
    ~IGES_GEOM_WALL();

    bool SetParams( MCAD_POINT p0, MCAD_POINT p1, MCAD_POINT p2, MCAD_POINT p3 );
    IGES_ENTITY_144* Instantiate( IGES* model );
};

#endif  // IGES_GEOM_WALL_H
