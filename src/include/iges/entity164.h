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
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ENTITY_164_H
#define ENTITY_164_H

#include <libigesconf.h>
#include <mcad_elements.h>
#include <iges_curve.h>

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


/**
 * Class IGES_ENTITY_164
 * represents a Solid of Linear Extrusion
 */
class IGES_ENTITY_164 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    IGES_CURVE* PTR;    // closed curve

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
    IGES_ENTITY_164( IGES* aParent );
    virtual ~IGES_ENTITY_164();

    // Inherited virtual functions
    virtual void Compact( void );
    virtual bool SetEntityForm(int aForm);
    virtual bool SetEntityUse(IGES_STAT_USE aUseCase);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    /**
     * Function GetClosedCurve
     * retrieves a pointer to the (planar) closed curve to be extruded
     * and returns true if a closed curve had been specified.
     *
     * @param aCurve = handle to to store the reference to the closed curve
     */
    bool GetClosedCurve( IGES_CURVE** aCurve );

    /**
     * Function SetClosedCurve
     * sets the closed planar curve which is to be extruded and returns
     * true on success.
     *
     * @param aCurve = pointer to the closed planar curve to be extruded
     */
    bool SetClosedCurve( IGES_CURVE* aCurve );

    int iPtr;  //< DE Sequence of the planar curve
    double L;  //< length of extrusion
    double I1; //< X unit vector of direction; default 0
    double J1; //< Y unit vector of direction; default 0
    double K1; //< Z unit vector of direction; default 1
};

#endif  // ENTITY_164_H
