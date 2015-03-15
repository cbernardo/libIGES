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

// NOTE:
// The associated parameter data are:
// + CRTN: Int: how the curve was created:
//              0: unspecified
//              1: projection of a curve on a surface
//              2: intersection of 2 surfaces
//              3: Isoparametric curve
// + SPTR: Pointer: surface on which the curve lies
// + BPTR: Pointer: definition of the curve in the parametric space of SPTR
// + CPTR: Pointer: curve B on surface S (alternate represenation)
// + PREF: Int: Indicates preferred representation of the Sending System:
//          0: unspecified
//          1: BPTR on SPTR calculation
//          2: alternate representation CPTR
//          3: CPTR and BPTR+SPTR are equally preferred
//
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
    IGES_ENTITY* PTS;   // surface entity
    IGES_ENTITY* PTO;   // outer curve
    std::list<IGES_ENTITY*>PTI; // inner cutouts

    friend class IGES;
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    IGES_ENTITY_144( IGES* aParent );
    ~IGES_ENTITY_144();

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity );
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
    bool GetPTO( IGES_ENTITY** aPtr );
    bool SetPTO( IGES_ENTITY* aPtr );
    bool GetPTIList( std::list<IGES_ENTITY*>& aList );
    bool AddPTI( IGES_ENTITY* aPtr );
    bool DelPTI( IGES_ENTITY* aPtr );
};

#endif  // ENTITY_144_H
