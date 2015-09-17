/*
 * file: entity184.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 184: Solid Assembly, Section 4.48 p.214/242
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

#ifndef ENTITY_184_H
#define ENTITY_184_H

#include <libigesconf.h>
#include <mcad_elements.h>
#include <iges_entity.h>

// NOTE:
// Allowed entities:
//      + Primitive
//      + Boolean Tree
//      + Solid Assembly
//      + Solid Instance
//      + Manifold Solid B-Rep Object
//
// The associated parameter data are:
// + PTR: Index: DE of the entity to be instantiated
//
// Forms:
//  0: All items are one of Primitive, Solid Instance, Boolean Tree, or Solid Assembly
//  1: At least one item is a Manifold BREP Object
//
// Unused DE items:
// + Structure
//


/**
 * Class IGES_ENTITY_184
 * represents a Solid Assembly.
 */
class MCAD_API IGES_ENTITY_184 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );
    // XXX - TO BE IMPLEMENTED

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
    // Inherited virtual functions
    virtual bool SetEntityForm(int aForm);
    virtual bool SetDependency(IGES_STAT_DEPENDS aDependency);
    virtual bool SetEntityUse(IGES_STAT_USE aUseCase);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    // XXX - TO BE IMPLEMENTED

};

#endif  // ENTITY_430_H
