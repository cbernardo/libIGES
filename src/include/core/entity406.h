/*
 * file: entity406.h
 *
 * Copyright 2017, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 406: Property Entity, Section 4.98, p.476 (504)
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
 * Notes:
 * 1. When Status Flag Number components are not supported any call to
 * change the component is ignored and the method shall return true;
 * optionally a warning message may be printed.
 * 2. Any call to change values of unsupported Directory Entry parameters
 * will fail and the method shall return false; a [BUG] message is
 * required in these instances.
 * 3. Curve entities shall have a GetStartPoint() and GetEndPoint()
 * method which reports the first and last point of the curve; this is
 * required to check the validity of a Composite Curve.
 * 4. Curve entities shall implement:
 *     GetNSegments( void ) : number of segments in a compound item
 * 4. Curve entities must also support an Interpolation Function:
 *   bool F( Point&, nSegment, var ) where nSegments = number of segments and
 *   var = 0..1. 'Point' and 'Connect Point' entities shall report
 *   nSegment = 0 and users shall not call an interpolator.
 */


#ifndef ENTITY_406_H
#define ENTITY_406_H

#include <core/iges_entity.h>

// NOTE:
// The associated parameter data are:
// + 1: Integer:  Number of Properties
// + 2: Variable: V(1) Property #1
// + ...
// + N+1: Variable: V(N) Property #N
//
// Forms: (* = unimplemented, # = implemented, x =  unimplemented untested entity)
//  *1: Definition Levels
//  *2: Region Restriction
//  *3: Level Function
//  *5: Line Widening
//  *6: Drilled Hole
//  *7: Reference Designator
//  *8: Pin Number
//  *9: Part Number
//  *10: Hierarchy
//  *11: Tabular Data
//  *12: External Reference File List
//  *13: Nominal Size
//  *14: Flow Line Specification
//  #15: Name (p513 / 541) :: Param 1 = 1, Param 2 = HString
//  *16: Drawing Size
//  *17: Drawing Units
//  *18: Intercharacter Spacing
//  x20: Highlight (p519 / 547)
//  x21: Pick
//  x22: Uniform Rectangular Grid
//  x23: Associativity Group Type
//  x24: Level to LEP Layer Map
//  x25: LEP Artwork Stackup
//  x26: LEP Drilled Hole
//  x27: Generic Data
//  x28: Dimension Units
//  x29: Dimension Tolerance
//  x30: Dimension Display Data
//  x31: Basic Dimension
//  x32: Drawing Sheet Approval
//  x33: Drawing Sheet ID
//  x34: Underscore
//  x35: Overscore
//  x36: Closure
//  x37: Signal Bus Width
//  x38: URL Anchor
//  x39: Planarity
//  x40: Continuity
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + View
// + Transformation Matrix
// + Label Display Association
// + Line weight
// + Color number
//

// Notes on Form Data Access:
//
// Form 15:
// GetData(): returns a (void *)&std::string
// SetData(): assumes a (void *)char[]  -- NOT (void *)std::string.
//

class IGES_ENTITY_406 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    // entity specific functions
    IGES_ENTITY_406( IGES* aParent );
    virtual ~IGES_ENTITY_406();
    const void *GetData();  // retrieve the entity data as a void pointer to be
                            // cast to a type appropriate to the Form Number
    bool  SetData(const void *Data);

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
    // Inherited virtual functions
    virtual void Compact( void );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual bool SetView( IGES_ENTITY* aView );
    virtual bool SetTransform( IGES_ENTITY* aTransform );
    virtual bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );
    virtual bool SetLineWeightNum( int aLineWeight );
    virtual bool SetColor( IGES_COLOR aColor );
    virtual bool SetColor( IGES_ENTITY* aColor );

private:
    void *data; // pointer to form-specific data
};

#endif  // ENTITY_406_H
