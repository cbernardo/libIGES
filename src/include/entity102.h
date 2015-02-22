/*
 * file: entity102.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 102: Composite Curve, Section 4.4, p.69+ (97+)
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

#ifndef ENTITY_102_H
#define ENTITY_102_H

#include "iges_entity.h"

// NOTE:
//
// A Composite Curve is an ordered list of entities including Point,
// Connect Point, and generic parameterized curves (except for Composite Curve itself).
// The order of listing of member entities is the order in which they appear in
// the parameter list. Other acceptable entities include:
// + Line
// + Circle
// + Spline
// + Conic
//
// + Subordinate entities (items in the list) must have a physical dependence
//   relationship with this entity. The first point of this entity is the first
//   point of the first list entity and the last point of this entity is the
//   last point of the last list entity. Within the list, the last point of
//   entity N must be the same as the first point of entity N+1.
//
// + Note restrictions on the use of the Point and Connect Point Entities, p.69 (97)
//
// + When HIERARCHY is Global Defer, the following entries are ignored and may
//   be defaulted:
//   + Line Font Pattern
//   + Line Weight
//   + Color Number
//   + Level
//   + View
//   + Blank Status
//
// The associated parameter data are:
// + N: Int: Number of member entities
// + DE(1): Int: Index to DE of 1st member entity
// + DE(N): Int: Index to DE of last member entity
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_102 : public IGES_ENTITY
{
protected:

    // Remove a child entity; this is invoked by a child which is being deleted
    virtual bool removeChild( IGES_ENTITY* aChildEntity ) = 0;

    // XXX - TO BE IMPLEMENTED

public:
    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference(IGES_ENTITY* aParentEntity);
    virtual bool DelReference(IGES_ENTITY* aParentEntity);
    virtual bool ReadDE(IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar);
    virtual bool ReadPD(std::ifstream& aFile, int& aSequenceVar);
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

#endif  // ENTITY_102_H
