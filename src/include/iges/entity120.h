/*
 * file: entity110.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 120: Surface of Revolution, Section 4.18, p.116+ (144+)
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

#ifndef ENTITY_120_H
#define ENTITY_120_H

#include <iges_entity.h>

// NOTE:
// The associated parameter data are:
// + L: Pointer: Line entity, the axis of revolution
// + C: Pointer: Curve entity (generatrix)
// + SA: Real: Start Angle, Radians
// + TA: Real: Terminate Angle, Radians
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_CURVE;

class IGES_ENTITY_120 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    IGES_CURVE* L;
    IGES_CURVE* C;

public:
    IGES_ENTITY_120( IGES* aParent );
    virtual ~IGES_ENTITY_120();
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    int iL;         // DE pointer to line entity
    int iC;         // DE pointer to curve entity

    bool GetL( IGES_CURVE** aCurve );
    bool GetAxis( IGES_CURVE** aCurve );
    bool SetL( IGES_CURVE* aCurve );
    bool SetAxis( IGES_CURVE* aCurve );

    bool GetC( IGES_CURVE** aCurve );
    bool GetGeneratrix( IGES_CURVE** aCurve );
    bool SetC( IGES_CURVE* aCurve );
    bool SetGeneratrix( IGES_CURVE* aCurve );

    union
    {
        double SA;
        double startAngle;
    };

    union
    {
        double TA;
        double endAngle;
    };
};

#endif  // ENTITY_TEMP_H
