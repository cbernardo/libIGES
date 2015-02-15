/*
 * file: entity110.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 110: Line, Section 4.13, p.96+ (124+)
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

#ifndef ENTITY_110_H
#define ENTITY_110_H

#include "iges_entity.h"

// NOTE:
// The associated parameter data are:
// + X1: Real: Start point
// + Y1: Real: Start point
// + Z1: Real: Start point
// + X2: Real: End point
// + Y2: Real: End point
// + Z2: Real: End point
//
// Forms:
//  0: line from Xt(1),Yt(1),Zt(1) to Xt(2),Yt(2),Zt(2)
//  1: semi-bounded (has origin at P1 but no terminus)
//  2: unbounded (infinite line for drawing)
//  The difference in the forms lies in the imposed limitations
//  on the parameterization variable (0..1, 0..+inf, -inf..+inf)
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_110 : public IGES_ENTITY
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

#endif  // ENTITY_110_H
