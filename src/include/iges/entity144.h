/*
 * file: entity144.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 144: Trimmed Parametric Surface, Section 4.34, p.181 (209+)
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

#ifndef ENTITY_144_H
#define ENTITY_144_H

#include <iges_entity.h>

class IGES_ENTITY_142;

// NOTE:
// The associated parameter data are:
// + PTS: Pointer: surface to be trimmed
// + N1: Int: 0 = self-bounded PTS, 1 = boundary of PTS differs from PTO
// + N2: Number of internal boundaries (cutouts)
// + PTO: Pointer: outer boundary of the surface
// + PTI: Pointer: list of inner boundaries (cutouts)
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_144 : public IGES_ENTITY
{
protected:

    int iPTS;
    int iPTO;
    std::list<int>iPTI;
    // E106-63 (copious data)
    // E108 (plane)
    // E114 (parametric spline surface)
    // E118 (ruled surface)
    // E120 (surface of revolution)
    // E122 (tabulated cylinder)
    // E128 (NURBS surface)
    // E140 (offset surface)
    // E143 (bounded surface)
    // E190 (plane surface)
    // E192 (right circular cylindrical surface)
    // E194 (right circular conical surface)
    // E196 (spherical surface)
    // E198 (toroidal surface)
    IGES_ENTITY* PTS;               // surface entity
    IGES_ENTITY_142* PTO;           // outer curve
    std::list<IGES_ENTITY_142*>PTI; // inner cutouts

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    IGES_ENTITY_144( IGES* aParent );
    ~IGES_ENTITY_144();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    int N1;
    int N2;

    bool GetPTS( IGES_ENTITY** aPtr );
    bool SetPTS( IGES_ENTITY* aPtr );
    bool GetPTO( IGES_ENTITY_142** aPtr );
    bool SetPTO( IGES_ENTITY_142* aPtr );
    bool GetPTIList( std::list<IGES_ENTITY_142*>& aList );
    bool AddPTI( IGES_ENTITY_142* aPtr );
    bool DelPTI( IGES_ENTITY_142* aPtr );
};

#endif  // ENTITY_144_H
