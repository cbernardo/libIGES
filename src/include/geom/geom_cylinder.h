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
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef IGES_GEOM_CYLINDER_H
#define IGES_GEOM_CYLINDER_H

#include <vector>
#include <libigesconf.h>
#include <geom/mcad_elements.h>

class IGES;
class IGES_ENTITY_144;

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

    /**
     * Function Instantiate
     * uses current parameters to create IGES entities which represent
     * a cylindrical arc. If data is returned in 'result' the
     * caller is responsible for deleting the pointer array when it
     * is no longer needed.
     *
     * @param model is the IGES object which will own the entities
     * created.
     * @param top is the height of the top of the cylindrical arc
     * @param bot is the height of the bottom of the cylindrical arc
     * @param result [out] is a list of pointers to the Trimmed
     * Parametric Surfaces which represent the cylindrical arc;
     * if non-NULL on return the caller is responsible for calling
     * delete[] on the pointer.
     * @param nParts will hold the number of surface pointers in
     * 'result'
     * @return true if the cylindrical arc was successfully
     * instantiated; in such a case 'result' will be non-null
     */
    bool Instantiate( IGES* model, double top, double bot,
                      IGES_ENTITY_144**& result, int& nParts, bool aReverse );
};

#endif  // IGES_GEOM_CYLINDER_H
