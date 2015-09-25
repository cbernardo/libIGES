/*
 * file: entity308.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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

/*
 * Description: IGES Entity 308: Subfigure Definition Entity, Section 4.74, p.377(405+)
 */

#ifndef ENTITY_308_H
#define ENTITY_308_H

#include <libigesconf.h>
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


/**
 * Class IGES_ENTITY_308
 * represents the Subfigure Definition Entity. This entity is
 * commonly used to define a feature within a part, a part
 * within an assembly or subassembly, and a subassembly
 * within an assembly. In the case of SolidWorks, as an
 * example, each occurrence of this entity is treated as
 * a Part or Subassembly.
 */
class IGES_ENTITY_308 : public IGES_ENTITY
{
protected:

    std::list< int > iDE;

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    std::list< IGES_ENTITY* > DE;       //< associated entities
    std::vector< IGES_ENTITY* > m_DE;   // DE arranged for access across DLL

    // public functions for libIGES only
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

    // class-specific functions for libIGES use only

    /**
     * Function getDepthLevel
     * calculates and returns the Depth Level of this entity;
     * this number is required to maintain a file which conforms
     * to the IGES specification.
     */
    int getDepthLevel( void );

public:
    IGES_ENTITY_308( IGES* aParent );
    virtual ~IGES_ENTITY_308();

    // Inherited virtual functions
    virtual void Compact( void );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetVisibility( bool isVisible );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // parameters
    int DEPTH;          //< Depth Level of this instance
    std::string NAME;   //< Name of this Subfigure Definition (Part Name or Subassembly Name)
    int N;              //< Number of entities comprising this Subfigure Definition

    /**
     * Function GetDEList
     * provides the caller with a list of entity pointers which comprise this
     * Subfigure Definition entity and returns true on success.
     *
     * @param aDESize will hold the number of entities returned in aDEList
     * @param aDEList will point to the first entity pointer
     * @return true if this Subfigure Definition entity was not empty
     */
    bool GetDEList( size_t& aDESize, IGES_ENTITY**& aDEList );


    /**
     * Function AddDE
     * adds an entity to the list of entities comprising this Subfigure Definition
     * and returns true on success.
     *
     * @param aPtr = a pointer to the entity to be added
     */
    bool AddDE(IGES_ENTITY *aPtr);

    /**
     * Function DelDE
     * removes a given entity pointer from the list of entities
     * comprising this Subfigure Definition and returns true if
     * the specified entity pointer was found and removed.
     *
     * @param aPtr = pointer of entity to be disassociated
     */
    bool DelDE( IGES_ENTITY* aPtr );
};

#endif  // ENTITY_308_H
