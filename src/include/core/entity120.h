/*
 * file: entity120.h
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
 * Description: IGES Entity 120: Surface of Revolution, Section 4.18, p.116+ (144+)
 */

#ifndef ENTITY_120_H
#define ENTITY_120_H

#include <libigesconf.h>
#include <core/iges_entity.h>

// NOTE:
// The associated parameter data are:
// + L: Pointer: Line entity, the axis of revolution
// + C: Pointer: Curve entity (generatrix)
// + SA: Real: Start Angle, Radians
// + TA: Real: Terminate Angle, Radians
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_CURVE;


/**
 * Class IGES_ENTITY_120
 * represents a Surface of Revolution which consists of a
 * 2D/3D Generatrix curve, an axis of revolution,
 * and a Start and Terminate angle.
 */
class IGES_ENTITY_120 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    IGES_CURVE* L;
    IGES_CURVE* C;

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
    IGES_ENTITY_120( IGES* aParent );
    virtual ~IGES_ENTITY_120();

    // Inherited virtual functions
    virtual bool SetEntityForm( int aForm );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    int iL;         // DE pointer to line entity
    int iC;         // DE pointer to curve entity

    /**
     * Function GetL
     * retrieves a pointer to the axis of revolution and
     * returns true on success.
     *
     * @param aCurve = handle to store pointer to axis of revolution, a line entity (Type 110)
     */
    bool GetL( IGES_CURVE** aCurve );

    /**
     * Function GetAxis
     * retrieves a pointer to the axis of revolution and
     * returns true on success.
     *
     * @param aCurve = handle to store pointer to axis of revolution, a line entity (Type 110)
     */
    bool GetAxis( IGES_CURVE** aCurve );

    /**
     * Function SetL
     * sets the axis of revolution and returns true on success.
     *
     * @param aCurve = pointer to line entity (Type 110) representing the axis of revolution
     */
    bool SetL( IGES_CURVE* aCurve );

    /**
     * Function SetAxis
     * sets the axis of revolution and returns true on success.
     *
     * @param aCurve = pointer to line entity (Type 110) representing the axis of revolution
     */
    bool SetAxis( IGES_CURVE* aCurve );

    /**
     * Function GetC
     * retrieves a pointer to the 2D/3D Generatrix curve and
     * returns true on success.
     *
     * @param aCurve = handle to store pointer to generatrix curve
     */
    bool GetC( IGES_CURVE** aCurve );

    /**
     * Function GetGeneratrix
     * retrieves a pointer to the 2D/3D Generatrix curve and
     * returns true on success.
     *
     * @param aCurve = handle to store pointer to generatrix curve
     */
    bool GetGeneratrix( IGES_CURVE** aCurve );


    /**
     * Function SetC
     * sets the 2D/3D Generatrix curve and returns true on success.
     *
     * @param aCurve = pointer to generatrix curve
     */
    bool SetC( IGES_CURVE* aCurve );

    /**
     * Function SetGeneratrix
     * sets the 2D/3D Generatrix curve and returns true on success.
     *
     * @param aCurve = pointer to generatrix curve
     */
    bool SetGeneratrix( IGES_CURVE* aCurve );

    /**
     * Start Angle (radians)
     */
    union
    {
        double SA;
        double startAngle;
    };

    /**
     * Terminal Angle (radians)
     */
    union
    {
        double TA;
        double endAngle;
    };
};

#endif  // ENTITY_TEMP_H
