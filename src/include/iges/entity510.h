/*
 * file: entity510.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 510: Face, Section 4.150, p.593+ (621+)
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

#ifndef ENTITY_510_H
#define ENTITY_510_H

#include <iges_entity.h>

// NOTE:
// The associated parameter data are:
// + N: Int: number of edge tuples
// + CURV(n): Int: DE to curve entity (100, 102, 104, 106/(11,12,63), 110, 112, 126, 130)
// + SVP(n): Int: DE of Vertex Entity (E502-1) for start vertex
// + SV(n): Int: List Index of Vertex in SVP(n) for start vertex
// + TVP(n): Int: DE of Vertex Entity (E502-1) for terminate vertex
// + TV(n): Int: List Index of Vertex in TVP(n) for terminate vertex
//
// Forms:
//  1: Vertex List
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

class IGES_ENTITY_510 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<int> iloops;              // DE to loops bounding the face
    std::list<IGES_ENTITY_508*> mloops; // loops bounding the face
    IGES_ENTITY* msurface;
    bool mOuterLoopFlag;                // Outer loop flag 'OF'

public:
    IGES_ENTITY_510( IGES* aParent );
    virtual ~IGES_ENTITY_510();

    // Inherited virtual functions
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
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

    // functions unique to E510
    const std::list<IGES_ENTITY_508*>* GetBounds( void );
    bool AddBound( IGES_ENTITY_508* aLoop );
    bool SetSurface( IGES_ENTITY* aSurface );
    IGES_ENTITY* GetSurface( void );

    void SetOuterLoopFlag( bool aFlag );
    bool GetOuterLoopFlag( void );
};

#endif  // ENTITY_510_H
