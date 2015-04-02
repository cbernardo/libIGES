/*
 * file: entity122.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 122: Tabulated Cylinder, Section 4.19, p.119(147+)
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

#ifndef ENTITY_122_H
#define ENTITY_122_H

#include <iges_elements.h>
#include <iges_curve.h>

// Note:
// The associated parameter data are:
// + DE: Int: pointer to DE entry of directrix
// + LX: Real: coordinates of terminate point of generatrix
// + LY: Real
// + LZ: Real
//
// Forms: 0
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_122 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    IGES_CURVE* DE;
    int iDE;

public:
    IGES_ENTITY_122( IGES* aParent );
    virtual ~IGES_ENTITY_122();

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference(IGES_ENTITY* aParentEntity);
    virtual bool DelReference(IGES_ENTITY* aParentEntity);
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm(int aForm);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    // parameters
    double LX;
    double LY;
    double LZ;

    bool GetDE( IGES_CURVE** aPtr );
    bool SetDE( IGES_CURVE* aPtr );
};

#endif  // ENTITY_122_H
