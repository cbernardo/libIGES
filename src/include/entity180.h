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

#include "iges_entity.h"

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

class IGES_ENTITY_180 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );
    // XXX - TO BE IMPLEMENTED

public:
    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference(IGES_ENTITY* aParentEntity);
    virtual bool DelReference(IGES_ENTITY* aParentEntity);
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool WriteDE(std::ofstream& aFile);
    virtual bool WritePD(std::ofstream& aFile);
    virtual bool SetEntityForm(int aForm);
    virtual bool SetLineFontPattern(IGES_LINEFONT_PATTERN aPattern);
    virtual bool SetLineFontPattern(IGES_ENTITY* aPattern);
    virtual bool SetLevel(int aLevel);
    virtual bool SetLevel(IGES_ENTITY* aLevel);
    virtual bool SetView(IGES_ENTITY* aView);
    virtual bool SetTransform(IGES_ENTITY* aTransform);
    virtual bool SetLabelAssoc(IGES_ENTITY* aLabelAssoc);
    virtual bool SetColor(IGES_COLOR aColor);
    virtual bool SetColor(IGES_ENTITY* aColor);
    virtual bool SetLineWeightNum(int aLineWeight);
    virtual bool SetDependency(IGES_STAT_DEPENDS aDependency);
    virtual bool SetEntityUse(IGES_STAT_USE aUseCase);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    // XXX - TO BE IMPLEMENTED

};

#endif  // ENTITY_180_H
