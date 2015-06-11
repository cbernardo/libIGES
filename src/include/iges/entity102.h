/*
 * file: entity102.h
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
 * Description: IGES Entity 102: Composite Curve, Section 4.4, p.69+ (97+)
 */

#ifndef ENTITY_102_H
#define ENTITY_102_H

#include <libigesconf.h>
#include <iges_curve.h>

// NOTE:
//
// A Composite Curve is an ordered list of entities including Point,
// Connect Point, and generic parameterized curves (except for Composite Curve itself).
// The order of listing of member entities is the order in which they appear in
// the parameter list. Acceptable entities include:
// * 100 ENT_CIRCULAR_ARC
// * 104 ENT_CONIC_ARC
// * 110 ENT_LINE
// * 112 ENT_PARAM_SPLINE_CURVE
// * 116 *ENT_POINT
// * 126 ENT_NURBS_CURVE
// * 132 *ENT_CONNECT_POINT
// * 106 ENT_COPIOUS_DATA FORMS: (Due to complexity, postpone any implementation of this)
// *     Note: not necessarily all forms are supported; this needs to be checked carefully
// *        1, 2, 3
// *        11, 12, 13
// *        63
// * 130 ENT_OFFSET_CURVE (Due to complexity, postpone this one)
//
// + Subordinate entities (items in the list) must have a physical dependence
//   relationship with this entity. The first point of this entity is the first
//   point of the first list entity and the last point of this entity is the
//   last point of the last list entity. Within the list, the last point of
//   entity N must be the same as the first point of entity N+1.
//
// + Note restrictions on the use of the Point and Connect Point Entities, p.69 (97)
//
// + When HIERARCHY is Global Defer, the following entries are ignored and may
//   be defaulted:
//   + Line Font Pattern
//   + Line Weight
//   + Color Number
//   + Level
//   + View
//   + Blank Status
//
// The associated parameter data are:
// + N: Int: Number of member entities
// + DE(1): Int: Index to DE of 1st member entity
// + DE(N): Int: Index to DE of last member entity
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//


/**
 * Class IGES_ENTITY_102
 * represents a composite curve
 */
class MCAD_API IGES_ENTITY_102 : public IGES_CURVE
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<int> iCurves;
    std::list<IGES_CURVE*> curves;

public:
    IGES_ENTITY_102( IGES* aParent );
    virtual ~IGES_ENTITY_102();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

    // method for adding items to this compound curve
    bool AddSegment( IGES_CURVE* aSegment );

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE(IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar);
    virtual bool ReadPD(std::ifstream& aFile, int& aSequenceVar);
    virtual bool SetEntityForm(int aForm);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    virtual bool IsClosed( void );
    virtual int GetNCurves( void );
    virtual IGES_CURVE* GetCurve( int index );
    virtual bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    virtual bool GetEndPoint( MCAD_POINT& pt, bool xform = true );
    virtual int GetNSegments( void );
    virtual bool Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform = true );
};

#endif  // ENTITY_102_H
