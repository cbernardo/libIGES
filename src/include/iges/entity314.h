/*
 * file: entity314.h
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
 * Description: IGES Entity 314: Color, Section 4.77, p.386 (414)
 */

#ifndef ENTITY314_H
#define ENTITY314_H

#include <libigesconf.h>
#include <iges_entity.h>

// NOTE:
// The associated parameter data are:
// + red: Real: Red, 0.0 .. 100.0
// + grn: Real: Green
// + blu: Real: Blue
// + cname: STR: Color name (optional)
//
// Forms: 0 only
//
// Unused DE items:
// + Structure
// + Line Font Pattern
// + Level
// + View
// + Transform
// + Label Display Association
// + Line weight
//
// Constraints:
// COLOR DE item must be 1..8 to represent the closest predefined shade


/**
 * Class IGES_ENTITY_314
 * represents the Color Definition Entity; this entity can
 * be used to define colors which are not predefined by the
 * IGES specification. Note that to conform to the specification
 * the user must also invoke the function SetColor() to assign
 * a fixed color definition which most closely matches this
 * color.
 */
class IGES_ENTITY_314 : public IGES_ENTITY
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
    MCAD_API IGES_ENTITY_314( IGES* aParent );
    virtual MCAD_API ~IGES_ENTITY_314();

    // Inherited virtual functions
    virtual MCAD_API bool SetEntityForm( int aForm );
    virtual MCAD_API bool SetDependency( IGES_STAT_DEPENDS aDependency );
    virtual MCAD_API bool SetEntityUse( IGES_STAT_USE aUseCase );
    virtual MCAD_API bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // other virtuals to be overridden
    virtual MCAD_API bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual MCAD_API bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual MCAD_API bool SetLevel( int aLevel );
    virtual MCAD_API bool SetLevel( IGES_ENTITY* aLevel );
    virtual MCAD_API bool SetView( IGES_ENTITY* aView );
    virtual MCAD_API bool SetTransform( IGES_ENTITY* aTransform );
    virtual MCAD_API bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );
    virtual MCAD_API bool SetColor( IGES_COLOR aColor );
    virtual MCAD_API bool SetColor( IGES_ENTITY* aColor );
    virtual MCAD_API bool SetVisibility(bool isVisible);
    virtual MCAD_API bool SetLineWeightNum( int aLineWeight );

    // parameters
    MCAD_API union
    {
        double red;     //< RED value (0..100); known in the specification as CC1
        double CC1;
    };

    MCAD_API union
    {
        double green;   //< GREEN value (0..100); known in the specification as CC2
        double CC2;
    };

    MCAD_API union
    {
        double blue;    //< BLUE value (0..100); known in the specification as CC3
        double CC3;
    };

    MCAD_API std::string cname;  // optional name describing the color; known in specification as CNAME
    MCAD_API std::string& CNAME;
};

#endif  // ENTITY314_H
