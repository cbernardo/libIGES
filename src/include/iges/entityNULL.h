/*
 * file: entityNULL.h
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
 * Description: IGES Entity 0: NULL, Section 4.2, p.65 (93)
 */

#ifndef ENTITY_NULL_H
#define ENTITY_NULL_H

#include "iges_entity.h"

// NOTE:
// It is impossible to reliably determine the entity type
// of a DE entry which has been set to NULL; as such, all
// NULL entities may be safely destroyed upon verification
// of the relationships of entities ('Associate' method
// has been executed). Destroying this essentially unrecoverable
// data will help ensure neater output files. Note that the
// deletion of dangling entities is another matter.
//
// All currently unsupported entities should also be treated
// as a NULL Entity
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + Level
// + View
// + Transformation Matrix
// + Label Display Association
// + Line weight
// + Color number
// + Form number
//


/**
 * Class IGES_ENTITY_NULL
 * represents the NULL Entity (Type 0) and all non-implemented entities.
 */
class IGES_ENTITY_NULL : IGES_ENTITY
{
private:
    int trueEntity;     // actual Entity Type; non-zero in the case of
                        // a currently unsupported entity

protected:

    friend class IGES;
    void setEntityType( int aEntityID );
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    IGES_ENTITY_NULL( IGES* aParent );
    virtual ~IGES_ENTITY_NULL();
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );

    /**
     * Function GetTrueEntityType
     * returns the number representing the true entity type
     * represented by this instance of the NULL Entity; in the
     * case of a true NULL Entity the return value will be zero
     * while for unimplemented entities the return value will be
     * non-zero.
     */
    int GetTrueEntityType(void)
    {
        return trueEntity;
    }

    // Inherited virtual functions
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
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
};

#endif  // ENTITY_NULL_H
