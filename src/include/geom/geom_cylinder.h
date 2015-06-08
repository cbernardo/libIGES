/*
 * file: geom_cylinder.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: object to aid in creating a vertical cylindrical
 * surface within IGES.
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

#ifndef IGES_GEOM_CYLINDER_H
#define IGES_GEOM_CYLINDER_H

#include <vector>
#include <libigesconf.h>
#include <all_entities.h>

class IGES;

class MCAD_API IGES_GEOM_CYLINDER
{
private:
    MCAD_POINT arcs[5];     // center, start .. end for series of sub-arcs;
                            // the end point of one sub-arc is the start
                            // of the next
    int narcs;              // number of arc segments to represent the surface
    double radius;
    double angles[6];       // start/end angles for arc1, arc2, arc3

    void init( void );
    void clear( void );

public:
    IGES_GEOM_CYLINDER();
    ~IGES_GEOM_CYLINDER();

    bool SetParams( MCAD_POINT center, MCAD_POINT start, MCAD_POINT end );
    bool Instantiate( IGES* model, double top, double bot,
                      std::vector<IGES_ENTITY_144*>& result );
};

#endif  // IGES_GEOM_CYLINDER_H
