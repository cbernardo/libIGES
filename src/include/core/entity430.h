/*
 * file: entity430.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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

/*
 * Description: IGES Entity 430: Solid Instance Entity, Section 4.146 p.584/612+
 */

#ifndef ENTITY_430_H
#define ENTITY_430_H

#include <libigesconf.h>
#include <geom/mcad_elements.h>
#include <core/iges_entity.h>

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


/**
 * Class IGES_ENTITY_430
 * represents the Solid Instance Entity; this entity represents a single instance of
 * a feature, part, or subassembly. Note: This class is currently unimplemented;
 * implementation is contingent upon encountering IGES files which make use of this
 * entity.
 */
class IGES_ENTITY_430 : public IGES_ENTITY
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
    virtual void Compact( void );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // XXX - TO BE IMPLEMENTED

};

#endif  // ENTITY_430_H
