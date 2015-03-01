/*
 * file: entity_template.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Sample entity template based on
 * IGES Entity 124: Transformation Matrix, Section 4.21, p.123+ (151+)
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

#error This is a template only and not suitable for compilation or inclusion

#ifndef ENTITY_TEMP_H
#define ENTITY_TEMP_H

#include "iges_entity.h"

// NOTE:
// The associated parameter data are:
// + R11: Real: Top Row
// + R12: Real:
// + R13: Real:
// + T1:  Real:
// + R21: Real: Second Row
// + R22: Real:
// + R23: Real:
// + T2:  Real:
// + R21: Real: Third Row
// + R22: Real:
// + R23: Real:
// + T2:  Real:
//
// Forms:
//  0: Matrix is Orthonormal and with a Determinant of 1; output will be in a Right-Hand Coordinate System
//  1: Matrix is Orthonormal and with a Determinant of -1; output will be in a Left-Hand Coordinate System
//  10, 11, 12: For Finite Element Analysis only
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + Level
// + View
// + Label Display Association
// + Line weight
// + Color number
//

class IGES_ENTITY_TEMP : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool associate( std::vector<IGES_ENTITY*>* entities );
    virtual bool format( int &index );
    virtual bool rescale( double sf );
    // XXX - TO BE IMPLEMENTED

public:
    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // XXX - TO BE IMPLEMENTED

};

#endif  // ENTITY_TEMP_H
