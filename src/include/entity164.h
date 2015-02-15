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

#include "iges_entity.h"

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

class IGES_ENTITY_164 : public IGES_ENTITY
{
private:
    // XXX - TO BE IMPLEMENTED

public:
    // Inherited virtual functions
    virtual bool AddReference(IGES_ENTITY* aParentEntity);
    virtual bool DelReference(IGES_ENTITY* aParentEntity);
    virtual bool ReadDE(IGES_RECORD* aRecord, std::ifstream& aFile);
    virtual bool ReadPD(std::ifstream& aFile);
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

#endif  // ENTITY_164_H
