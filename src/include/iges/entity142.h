/*
 * file: entity142.h
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
 * Description: IGES Entity 142: Curve on a Parametric Surface, Section 4.32, p.178 (206+)
 */

#ifndef ENTITY_142_H
#define ENTITY_142_H

#include <libigesconf.h>
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


/**
 * Class IGES_ENTITY_142
 * represents a Curve on a Parametric Surface
 */
class IGES_ENTITY_142 : public IGES_ENTITY
{
protected:

    int iSPTR;
    int iBPTR;
    int iCPTR;
    IGES_ENTITY* SPTR;
    IGES_ENTITY* BPTR;
    IGES_ENTITY* CPTR;

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

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
    IGES_ENTITY_142( IGES* aParent );
    virtual ~IGES_ENTITY_142();

    // Inherited virtual functions
    virtual void Compact( void );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    int CRTN;   //< Creation flag for the curve; 0=Unspecified, 1=Projection onto surface,
                //< 2=Intersection of 2 surfaces, 3=Isoparametric curve
    int PREF;   //< Preferred representation in the Sending System, 0=Unspecified,
                //< 1=Curve in Parameter Space (BPTR) is preferred, 2=Model space curve (CPTR)
                //< is preferred, 3=BPTR and CPTR are equally preferred.

    /**
     * Function GetSPTR
     * retrieves a pointer to the underlying surface entity; this surface
     * entity must be the same as the one referenced by the Trimmed Parametric Surface Entity
     * which references this Curve on a Parametric Surface Entity. The function returns
     * true on success.
     *
     * @param aPtr = handle to store pointer to the underlying surface entity
     */
    bool GetSPTR( IGES_ENTITY** aPtr );

    /**
     * Function SetSPTR
     * Sets the underlying surface entity and returns true on success.
     *
     * @param aPtr = pointer to the underlying surface entity
     */
    bool SetSPTR( IGES_ENTITY* aPtr );

    /**
     * Function GetBPTR
     * retrieves a pointer to the Parameter Space Curve on the underlying surface entity
     * and returns true on success (such a curve exists).
     *
     * @param aPtr = handle to store pointer to the Parameter Space Curve
     */
    bool GetBPTR( IGES_ENTITY** aPtr );


    /**
     * Function SetBPTR
     * sets the Parameter Space Curve on the underlying surface entity
     * and returns true on success.
     *
     * @param aPtr = pointer to the Parameter Space Curve
     */
    bool SetBPTR( IGES_ENTITY* aPtr );

    /**
     * Function GetCPTR
     * retrieves a pointer to the Model Space Curve on the underlying surface entity
     * and returns true on success (such a curve exists).
     *
     * @param aPtr = handle to store pointer to the Model Space Curve
     */
    bool GetCPTR( IGES_ENTITY** aPtr );

    /**
     * Function SetCPTR
     * sets the Model Space Curve on the underlying surface entity
     * and returns true on success.
     *
     * @param aPtr = pointer to the Model Space Curve
     */
    bool SetCPTR( IGES_ENTITY* aPtr );
};

#endif  // ENTITY_142_H
