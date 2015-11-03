/*
 * file: entity510.h
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
 * Description: IGES Entity 510: Face, Section 4.150, p.593+ (621+)
 */

#ifndef ENTITY_510_H
#define ENTITY_510_H

#include <libigesconf.h>
#include <core/iges_entity.h>

// NOTE:
// The associated parameter data are:
// + SURF: Int: DE to underlying surface entity; this may be one of Entities:
//      114: Parametric Spline Surface
//      118: (Form 1) Ruled Surface
//      120: Surface of Revolution
//      122: Tabulated Cylinder
//      128: Rational B-Spline Surface
//      140: Offset Surface
//      190: Plane Surface (untested)
//      192: Right Circular Cylindrical Surface (untested)
//      194: Right Circular Conical Surface (untested)
//      196: Spherical Surface (untested)
//      198: Toroidal Surface (untested)
// + N: Int: Number of loops (must be > 0)
// + OF: Bool: Outer Loop Flag (true implies that the first loop is the outer loop)
// + LOOP(n): Int: DE to LOOP entities on the face
//
// Forms:
//  1: Face
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + View
// + Transformation Matrix
// + Line weight
// + Color number
//

class IGES_ENTITY_508;


/**
 * Class IGES_ENTITY_510
 * represents the Face Entity
 */
class IGES_ENTITY_510 : public IGES_ENTITY
{
private:
    // check that the surface type is allowed by the specification
    bool checkSurfType( IGES_ENTITY* aEnt );

protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    ///< DE to loops bounding the face, LOOP(1..N) in the specification
    std::list< int > iloops;
    std::vector< IGES_ENTITY_508* > mloops; //< loops bounding the face
    IGES_ENTITY* msurface;                  //< associated surface entity
    int mDEsurf;                            //< DE to associated surface entity, 'SURF' in the specification
    bool mOuterLoopFlag;                    //< Outer loop flag 'OF' in the specification

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
    IGES_ENTITY_510( IGES* aParent );
    virtual ~IGES_ENTITY_510();

    // Inherited virtual functions
    virtual bool SetTransform( IGES_ENTITY* aTransform );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    // parameters not supported by the specification:
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual bool SetView( IGES_ENTITY* aView );
    virtual bool SetColor( IGES_COLOR aColor );
    virtual bool SetColor( IGES_ENTITY* aColor );
    virtual bool SetLineWeightNum( int aLineWeight );


    /**
     * Function GetBounds
     * returns a pointer to the list of loop entities which
     * bound this face.
     */
    bool GetBounds( size_t& aListSize, IGES_ENTITY_508**& aBoundsList );


    /**
     * Function AddBound
     * adds a Loop Entity to the list of bounds to this face
     * and returns true on success.
     *
     * @param aLoop = loop to add to face boundaries
     */
    bool AddBound( IGES_ENTITY_508* aLoop );


    /**
     * Function SetSurface
     * sets the pointer to the underlying surface of this face
     * and returns true on success.
     *
     * @param aSurface = a pointer to a valid surface (see sec. 4.150 of the specification)
     */
    bool SetSurface( IGES_ENTITY* aSurface );


    /**
     * Function GetSurface
     * retrieves a pointer to the underlying surface entity or NULL
     * if no such entity has been provided.
     */
    IGES_ENTITY* GetSurface( void );


    /**
     * Function SetOuterLoopFlag
     * sets the value of the Outer Loop Flag 'OF'.
     */
    void SetOuterLoopFlag( bool aFlag );


    /**
     * Function GetOuterLoopFlag
     * returns the value of the Outer Loop Flag 'OF'.
     */
    bool GetOuterLoopFlag( void );
};

#endif  // ENTITY_510_H
