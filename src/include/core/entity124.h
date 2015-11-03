/*
 * file: entity124.h
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
 * Description: IGES Entity 124: Transformation Matrix, Section 4.21, p.123+ (151+)
 */

#ifndef ENTITY_124_H
#define ENTITY_124_H

#include <libigesconf.h>
#include <geom/mcad_elements.h>
#include <core/iges_entity.h>

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
// Since transforms may be nested we should provide the following operators for access:
// Set/GetChildTransform(): Sets/gets the child transform in the DE
// Set/GetTopTransform(): Sets/gets this entity's transform data
// GetTransformMatrix(): Gets the overall transform matrix
// Transform(X,Y,Z): Performs a transform on the given point
//
// Note that GetTransformMatrix() produces the matrix by combining the
// Top matrix with the child's GetTransformMatrix(). This ensures correct
// application of all subordinate transforms.
//


/**
 * Class IGES_ENTITY_124
 * represents a Transformation which consists of a
 * 3x3 Transformation Matrix and a 3x1 Offset Matrix.
 */
class IGES_ENTITY_124 : public IGES_ENTITY
{
protected:

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
    MCAD_TRANSFORM T;   //< Transformation matrix data for this entity

    IGES_ENTITY_124( IGES* aParent );
    virtual ~IGES_ENTITY_124();

    // Inherited virtual functions
    virtual bool SetEntityForm(int aForm);
    virtual bool SetVisibility(bool isVisible);
    virtual bool SetDependency(IGES_STAT_DEPENDS aDependency);
    virtual bool SetEntityUse(IGES_STAT_USE aUseCase);
    virtual bool SetHierarchy(IGES_STAT_HIER aHierarchy);

    // items to be overridden; these items are not supported in this entity
    // + Line Font Pattern
    // + Level
    // + View
    // + Label Display Association
    // + Line weight
    // + Color number
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual bool SetLevel( int aLevel );
    virtual bool SetLevel( IGES_ENTITY* aLevel );
    virtual bool SetView( IGES_ENTITY* aView );
    virtual bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );
    virtual bool SetColor( IGES_COLOR aColor );
    virtual bool SetColor( IGES_ENTITY* aColor );
    virtual bool SetLineWeightNum( int aLineWeight );

    /**
     * Function GetTransformMatrix
     * returns the overall transformation matrix for this entity
     * which is equal to the local transform data multiplied by the
     * overall transformation matrix of the referenced transform
     * entity if any.
     */
    MCAD_TRANSFORM GetTransformMatrix( void );
};

#endif  // ENTITY_124_H
