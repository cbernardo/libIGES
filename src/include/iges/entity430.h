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
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ENTITY_430_H
#define ENTITY_430_H

#include <iges_elements.h>
#include <iges_entity.h>

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

class IGES_ENTITY_430 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );
    // XXX - TO BE IMPLEMENTED

public:
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );
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

#endif  // ENTITY_430_H
