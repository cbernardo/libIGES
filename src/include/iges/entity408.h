/*
 * file: entity408.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 4-8: Subfigure Instance, Section 4.137, p.557(585+)
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

#ifndef ENTITY_408_H
#define ENTITY_408_H

#include <iges_elements.h>
#include <iges_entity.h>

class IGES_ENTITY_308;

// Note:
// The associated parameter data are:
// + DE: Int: pointer to Entity308 (Subfigure Definition)
// + X: Real: offset
// + Y: Real
// + Z: Real
// + S: Real: scale factor, DEFAULT = 1.0
//
// Forms: 0
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_408 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    IGES_ENTITY_308* DE;
    int iDE;

public:
    IGES_ENTITY_408( IGES* aParent );
    virtual ~IGES_ENTITY_408();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

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
    double X;
    double Y;
    double Z;
    double S;

    bool GetDE( IGES_ENTITY_308** aPtr );
    bool SetDE( IGES_ENTITY_308* aPtr );
    int GetDepthLevel( void );
};

#endif  // ENTITY_408_H
