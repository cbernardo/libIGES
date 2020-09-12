/*
 * file: entity108.h
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
 * Description: IGES Entity 108: Plane Entity, Section 4.12, p.92+ (120+)
 */

#ifndef ENTITY_108_H
#define ENTITY_108_H

#include <libigesconf.h>
#include <core/iges_curve.h>

// NOTE:
// Planes are parameterized in definition space coordinates as A*Xt + B*Yt + C*Zt = D.
// A plane may be unbounded (Type 0), bounded (Type 1); or (deprecated) a bounded plane
// representing a hole (Type -1).
// Display symbols are supported by parameters 6 to 9.
//
// The associated parameter data are:
// + A   : Real    : Xt coefficient of the plane
// + B   : Real    : Yt coefficient of the plane
// + C   : Real    : Zt coefficient of the plane
// + D   : Real    : coefficient of the plane
// + PTR : Pointer : always zero
// + X   : Real    : Xt coordinate of display symbol
// + Y   : Real    : Yt coordinate of display symbol
// + Z   : Real    : Zt coordinate of display symbol
// + SIZE: Real    : size parameter for display symbol
//
// Forms:
//  -1 : (deprecated) bounded plane representing a hole
//   0 : unbounded plane
//   1 : bounded plane
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + Line Weight Number
//
// Special notes:
// If the plane is used as a clipping plane then Entity use Flag shall be of type Annotation (01)
//


/**
 * Class IGES_ENTITY_108
 * represents a plane as A*Xt + B*Yt + C*Zt = D.
 */
class IGES_ENTITY_108 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    // Type 108 parameters
    IGES_CURVE* PTR;    //< closed curve referenced by iPtr
    int iPtr;           //< DE Sequence of the planar curve

public:
    // public functions for internal libIGES use
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

public:
    IGES_ENTITY_108( IGES* aParent );
    virtual ~IGES_ENTITY_108();

    // Inherited virtual functions
    virtual bool SetEntityForm( int aForm );
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // other virtuals to be overridden
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual bool SetLineWeightNum( int aLineWeight );

    // Type 108 functions
    /**
     * Function GetClosedCurve
     * retrieves a pointer to the (planar) closed bounding curve.
     *
     * @param aCurve = handle to to store the reference to the closed curve
     */
    bool GetClosedCurve( IGES_CURVE** aCurve );

    /**
     * Function SetClosedCurve
     * sets the closed planar bounding curve and returns true on success.
     *
     * @param aCurve = pointer to the closed planar curve to be extruded
     */
    bool SetClosedCurve( IGES_CURVE* aCurve );

    // Type 108 parameters
    double  A;          //< Plane coefficient A*Xt
    double  B;          //< Plane coefficient B*Yt
    double  C;          //< Plane coefficient C*Zt
    double  D;          //< Plane coefficient D

    double  X;          //< X coordinate of display symbol
    double  Y;          //< X coordinate of display symbol
    double  Z;          //< X coordinate of display symbol
    double  Size;       //< X size parameter of display symbol
};

#endif  // ENTITY_108_H
