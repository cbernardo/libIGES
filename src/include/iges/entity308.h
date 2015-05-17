/*
 * file: entity308.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 308: Subfigure Definition Entity, Section 4.74, p.377(405+)
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

#ifndef ENTITY_308_H
#define ENTITY_308_H

#include <iges_entity.h>

// NOTE:
// The associated parameter data are:
// + DEPTH: Int: depth of subfigure nesting; top level subfigures have the higher number
//               and no subfigure may contain a subfigure of equal or higher depth level.
// + NAME: HString: name of the subfigure
// + N: Int: number of entities in the subfigure
// + DE(N): Integer: list of associated entities
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_308: public IGES_ENTITY
{
protected:

    std::list<int>iDE;
    std::list<IGES_ENTITY*>DE;  // associated entities

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    IGES_ENTITY_308( IGES* aParent );
    ~IGES_ENTITY_308();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetVisibility( bool isVisible );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // class-specific functions
    int GetDepthLevel( void );

    // parameters
    int DEPTH;
    std::string NAME;
    int N;

    bool GetDEList( std::list<IGES_ENTITY*>& aList );
    bool AddDE( IGES_ENTITY* aPtr );
    bool DelDE( IGES_ENTITY* aPtr );
};

#endif  // ENTITY_308_H
