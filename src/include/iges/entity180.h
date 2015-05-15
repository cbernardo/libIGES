/*
 * file: entity180.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 180: Boolean Tree, Section 4.46, p.209(237+)
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

#ifndef ENTITY_180_H
#define ENTITY_180_H

#include <iges_elements.h>
#include <iges_entity.h>

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

enum BTREE_OPERATOR
{
    OP_START = 1,
    OP_UNION = 1,
    OP_INTERSECT,
    OP_DIFFERENCE,
    OP_END
};

struct BTREE_NODE
{
    bool op;    // true if this is an operation; false if an entity pointer
    int  val;   // Operation code (1,2,3) or DE Sequence of entity
    IGES_ENTITY* pEnt;  // pointer to entity (Primitive, Solid Instance, BTree, or manifold solid BREP

    BTREE_NODE()
    {
        op = false;
        val = 0;
        pEnt = NULL;
    }
};


class IGES_ENTITY_180 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<BTREE_NODE*> nodes;
    bool typeOK( int aTypeNum );

public:
    IGES_ENTITY_180( IGES* aParent );
    virtual ~IGES_ENTITY_180();
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );

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

    bool AddOp( BTREE_OPERATOR op );
    bool AddArg( IGES_ENTITY* aOperand );
    void ClearNodes( void );
    int GetNNodes( void );
    std::list<BTREE_NODE*>* GetNodes( void );
};

#endif  // ENTITY_180_H
