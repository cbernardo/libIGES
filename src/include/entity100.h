/*
 * file: entity100.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 100: Circle, Section 4.3, p.66+ (94+)
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

#ifndef ENTITY_100_H
#define ENTITY_100_H

#include "iges_entity.h"

// NOTE:
// Arcs must lie in the Xt-Yt plane (constant Z in definition space coordinates).
// Arcs are defined by their endpoints and center and are traced in a CCW fashion
// from First Endpoint to Second Endpoint (similar to the IDF specification).
//
// The associated parameter data are:
// + ZT: Real: Zt displacement of the arc from the Xt-Yt plane
// + X1: Real: Arc center, abscissa
// + Y1: Real: Arc center, ordinate
// + X2: Real: Start point, abscissa
// + Y2: Real: Start point, ordinate
// + X3: Real: End point, abscissa
// + Y3: Real: End point, ordinate
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
//

class IGES_ENTITY_100 : public IGES_ENTITY
{
protected:

    // Remove a child entity; this is invoked by a child which is being deleted
    virtual bool removeChild( IGES_ENTITY* aChildEntity );

    // XXX - TO BE IMPLEMENTED

public:
    IGES_ENTITY_100( IGES* aParent );
    ~IGES_ENTITY_100();

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool WriteDE( std::ofstream& aFile );
    virtual bool WritePD( std::ofstream& aFile );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual bool SetLevel( int aLevel );
    virtual bool SetLevel( IGES_ENTITY* aLevel );
    virtual bool SetView( IGES_ENTITY* aView );
    virtual bool SetTransform( IGES_ENTITY* aTransform );
    virtual bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );
    virtual bool SetColor( IGES_COLOR aColor );
    virtual bool SetColor( IGES_ENTITY* aColor );
    virtual bool SetLineWeightNum( int aLineWeight );
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    double zOffset;     // ZT in the IGES spec
    double xCenter;     // X1
    double yCenter;     // Y1
    double xStart;      // X2
    double yStart;      // Y2
    double xEnd;        // X3
    double yEnd;        // Y3

    // XXX - TO BE IMPLEMENTED

};

#endif  // ENTITY_100_H
