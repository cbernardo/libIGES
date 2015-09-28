/*
 * file: entity408.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * This file is part of libIGES.
 *
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 * Description: IGES Entity 4-8: Subfigure Instance, Section 4.137, p.557(585+)
 */

#ifndef ENTITY_408_H
#define ENTITY_408_H

#include <libigesconf.h>
#include <mcad_elements.h>
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

/**
 * Class IGES_ENTITY_408
 * represents the Singular Subfigure Instance Entity; this
 * entity is commonly used to instantiate a feature or part
 * within a part or subassembly.
 */
class IGES_ENTITY_408 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    IGES_ENTITY_308* DE;    // Pointer to the Subfigure Definition Entity to be instantiated
    int iDE;                // Directory Entry index to the Subfigure Definition Entity

public:
    // public functions for libIGES only
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

public:
    IGES_ENTITY_408( IGES* aParent );
    virtual ~IGES_ENTITY_408();

    // Inherited virtual functions
    virtual void Compact( void );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // parameters
    double X;   //< X translation relative to type space of a referring entity
    double Y;   //< Y translation relative to type space of a referring entity
    double Z;   //< Z translation relative to type space of a referring entity
    double S;   //< scale factor relative to type space of a referring entity


    /**
     * Function GetDE
     * retrieves a pointer to the associated Subfigure Definition Entity
     * and returns true on success; false is returned if there is no
     * associated Subfigure Definition Entity.
     *
     * @param aPtr = handle to store pointer to associated Subfigure Definition Entity
     */
    bool GetDE( IGES_ENTITY_308*& aPtr );


    /**
     * Function SetDE
     * sets the associated Subfigure Definition Entity and returns true on
     * success.
     *
     * @param aPtr = pointer to the Subfigure Definition Entity to associate
     */
    bool SetDE( IGES_ENTITY_308* aPtr );

    /**
     * Function GetDepthLevel
     * returns the nesting level of this entity; this function is used to
     * establish correct Depth Level values as per the IGES specification.
     */
    int getDepthLevel( void );
};

#endif  // ENTITY_408_H
