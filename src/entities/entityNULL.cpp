/*
 * file: entityNULL.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 0: NULL, Section 4.2, p.65 (93)
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

#include <error_macros.h>
#include <iges.h>
#include <entityNULL.h>

IGES_ENTITY_NULL::IGES_ENTITY_NULL(IGES* aParent) : IGES_ENTITY(aParent)
{
    // XXX - TO BE IMPLEMENTED
    return;
}


IGES_ENTITY_NULL::~IGES_ENTITY_NULL()
{
    // XXX - TO BE IMPLEMENTED
    return;
}


bool IGES_ENTITY_NULL::AddReference(IGES_ENTITY* aParentEntity)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::DelReference(IGES_ENTITY* aParentEntity)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::ReadDE(IGES_RECORD* aRecord, std::ifstream& aFile)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::ReadPD(std::ifstream& aFile)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::WriteDE(std::ofstream& aFile)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::WritePD(std::ofstream& aFile)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetEntityForm(int aForm)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetLineFontPattern(IGES_LINEFONT_PATTERN aPattern)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetLineFontPattern(IGES_ENTITY* aPattern)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetLevel(int aLevel)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetLevel(IGES_ENTITY* aLevel)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetView(IGES_ENTITY* aView)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetTransform(IGES_ENTITY* aTransform)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetLabelAssoc(IGES_ENTITY* aLabelAssoc)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetColor(IGES_COLOR aColor)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetColor(IGES_ENTITY* aColor)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetLineWeightNum(int aLineWeight)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetDependency(IGES_STAT_DEPENDS aDependency)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetEntityUse(IGES_STAT_USE aUseCase)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_NULL::SetHierarchy(IGES_STAT_HIER aHierarchy)
{
    // XXX - TO BE IMPLEMENTED
    return false;
}
