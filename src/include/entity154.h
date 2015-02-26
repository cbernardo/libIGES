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

#include "iges_entity.h"

// NOTE:
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

class IGES_ENTITY_154 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );
    virtual bool format( int &index );
    virtual bool rescale( double sf );
    // XXX - TO BE IMPLEMENTED

public:
    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference(IGES_ENTITY* aParentEntity);
    virtual bool DelReference(IGES_ENTITY* aParentEntity);
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm(int aForm);
    virtual bool SetDependency(IGES_STAT_DEPENDS aDependency);
    virtual bool SetEntityUse(IGES_STAT_USE aUseCase);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    // XXX - TO BE IMPLEMENTED

};

#endif  // ENTITY_154_H
