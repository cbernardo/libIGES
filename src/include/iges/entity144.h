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

#include <libigesconf.h>
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


/**
 * Class IGES_ENTITY_144
 * represents a Trimmed Parametric Surface; this is the
 * primary entity type used to describe surfaces of a solid model.
 */
class MCAD_API IGES_ENTITY_144 : public IGES_ENTITY
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
    // public functions for libIGES only
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

public:
    IGES_ENTITY_144( IGES* aParent );
    ~IGES_ENTITY_144();

    // Inherited virtual functions
    virtual bool SetEntityForm( int aForm );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    int N1; //< 0 if the outer boundary is the boundary of the underlying untrimmed surface
    int N2; //< Number of simple closed curves representing the inner boundary of the surface (0 if no inner boundaries)

    /**
     * Function GetPTS
     * retrieves a pointer to the underlying untrimmed surface entity and
     * returns true on success.
     *
     * @param aPtr = handle to store pointer to underlying untrimmed surface
     */
    bool GetPTS( IGES_ENTITY** aPtr );

    /**
     * Function SetPTS
     * sets the underlying untrimmed surface entity and returns true on success.
     *
     * @param aPtr = pointer to underlying untrimmed surface
     */
    bool SetPTS( IGES_ENTITY* aPtr );


    /**
     * Function GetPTO
     * retrieves a pointer to the Curve on a Parametric Surface Entity
     * representing the outer boundary of the surface and returns true
     * on success. The returned pointer may be NULL if the surface is
     * untrimmed.
     *
     * @param aPtr = handle to store pointer to outer boundary curve
     */
    bool GetPTO( IGES_ENTITY_142** aPtr );

    /**
     * Function SetPTO
     * sets the Curve on a Parametric Surface Entity representing the
     * outer boundary of the surface and returns true on success.
     *
     * @param aPtr = pointer to outer boundary curve or NULL for an untrimmed surface
     */
    bool SetPTO( IGES_ENTITY_142* aPtr );

    /**
     * Function GetPTIList
     * retrieves the pointers to internal boundary curves
     * and returns true on success.
     *
     * @param aList = list to store pointers
     */
    bool GetPTIList( std::list<IGES_ENTITY_142*>& aList );

    /**
     * Function AddPTI
     * adds a Curve on a Parametric Surface Entity to the
     * list of internal boundaries for the surface and returns
     * true on success.
     *
     * @param aPtr = pointer to the inner boundary curve
     */
    bool AddPTI( IGES_ENTITY_142* aPtr );


    /**
     * Function DelPTI
     * removes  a Curve on a Parametric Surface Entity from the
     * list of internal boundaries for the surface and returns
     * true if the curve was found and removed.
     *
     * @param aPtr = pointer to the inner boundary curve to be removed
     */
    bool DelPTI( IGES_ENTITY_142* aPtr );
};

#endif  // ENTITY_144_H
