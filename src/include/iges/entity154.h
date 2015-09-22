/*
 * file: entity154.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 154: Right Circular Cylinder, Section 4.39, p.193(221+)
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

#ifndef ENTITY_154_H
#define ENTITY_154_H

#include <libigesconf.h>
#include <mcad_elements.h>
#include <iges_entity.h>

// Note:
// The associated parameter data are:
// + H: Real: Cylinder Height
// + R: Real: Cylinder Radius
// + X1: Real: First face center coordinates
// + Y1: Real:
// + Z1: Real:
// + I1: Real: Unit vector in axis direction
// + J1: Real:
// + K1: Real:
//
// Forms: 0
//
// Unused DE items:
// + Structure
//


/**
 * Class IGES_ENTITY_154
 * represents a Right Circular Cylinder
 */
class IGES_ENTITY_154 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    // public functions for libIGES only
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

public:
    IGES_ENTITY_154( IGES* aParent );
    virtual ~IGES_ENTITY_154();

    // Inherited virtual functions
    virtual void Compact( void );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // parameters
    double H;   //< cylinder height
    double R;   //< cylinder radius
    double X1;  //< X value, first face center coordinate, default 0
    double Y1;  //< Y value, first face center coordinate, default 0
    double Z1;  //< Z value, first face center coordinate, default 0
    double I1;  //< X value, unit vector in axis direction, default 0
    double J1;  //< Y value, unit vector in axis direction, default 0
    double K1;  //< Z value, unit vector in axis direction, default 1
};

#endif  // ENTITY_154_H
