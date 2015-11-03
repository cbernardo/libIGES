/*
 * file: entity180.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 180: Boolean Tree, Section 4.46, p.209(237+)
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

#ifndef ENTITY_180_H
#define ENTITY_180_H

#include <libigesconf.h>
#include <geom/mcad_elements.h>
#include <core/iges_entity.h>

// NOTE:
// The boolean operators are in Postfix Notation. Examples:
// + DE1, DE2, DIFFERENCE,
// + DE1, DE2, DIFFERENCE, DE3, DIFFERENCE, DE4, DIFFERENCE (A board with holes can be built this way)
// Since the operators are BOOLEAN, if M is the number of Operands, the number of Operators must be M - 1.
// The specification implies a stack of operands and operators; it should be possible to parse these into
// a list of some sort.
//
// The associated parameter data are:
// + N: Number: Number of items (DE pointers + Operators)
// + LIST: List of DEs (negative) and Operators (1:Union, 2:Intersection, 3:Difference)
//
// Forms:
//  0: All operands are Primitives, Solid Instances, or other Boolean Trees
//  1: At least one operand is a Manifold BREP Object
//
// Unused DE items:
// + Structure
//
// Access functions:
// Use Push(pointer/integer) and Pop() to manipulate the list; provide an iterator.
// The Push(pointer) function must check the type of data pushed and change the Form
// to 1 if necessary.
//


/**
 * Class IGES_ENTITY_180
 * represents a Boolean Solid Operation Tree.
 */
class IGES_ENTITY_180 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<BTREE_NODE*> nodes;
    bool typeOK( int aTypeNum );

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
    IGES_ENTITY_180( IGES* aParent );
    virtual ~IGES_ENTITY_180();

    // Inherited virtual functions
    virtual bool SetEntityForm(int aForm);
    virtual bool SetDependency(IGES_STAT_DEPENDS aDependency);
    virtual bool SetEntityUse(IGES_STAT_USE aUseCase);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    /**
     * Function AddOp
     * adds the given operator to the list of entities in the boolean tree
     *
     * @param op = Boolean Operation
     */
    bool AddOp( BTREE_OPERATOR op );

    /**
     * Function AddArg
     * adds the given argument to the list of entities in the boolean tree
     *
     * @param aOperand = operand for the Boolean Tree
     */
    bool AddArg( IGES_ENTITY* aOperand );

    /**
     * Function ClearNodes
     * erases the current Boolean Tree operator/operand list
     */
    void ClearNodes( void );

    /**
     * Function GetNNodes
     * returns the number of nodes (operators and operands) in the BTREE list
     */
    int GetNNodes( void );

    /**
     * Function GetNodes
     * returns a pointer to the internal list of BTREE operators and operands
     */
    BTREE_NODE* GetNode( int aIndex );
};

#endif  // ENTITY_180_H
