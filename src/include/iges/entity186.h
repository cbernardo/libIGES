/*
 * file: entity186.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 186: Manifold Solid B-REP Object (MSBO)
 *              Section 4.49, p.216+ (244+)
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

#ifndef ENTITY_186_H
#define ENTITY_186_H

#include <libigesconf.h>
#include <iges_entity.h>

// NOTE:
// The associated parameter data are:
// + SHELL: INT: DE of the shell
// + SOF: BOOL: Orientation flag of shell, 1 = agrees with underlying faces
// + N: INT: number of void shells or 0
// + VOID(n): INT: DE to void shell
// + VOF(n): BOOL: orientation flag of void shell
// Form: 0
//
// Unused DE items:
// + Structure

class IGES_ENTITY_514;

/**
 * Class IGES_ENTITY_186
 * represents a Manifold Solid B-Rep Object (MSBO)
 */
class IGES_ENTITY_186 : public IGES_ENTITY
{
protected:
    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    int mDEshell;               //< DE of the shell
    IGES_ENTITY_514* mshell;    //< the primary shell of this MSBO
    bool mSOF;  //< shell orientation flag
    std::list<std::pair<int, bool> > ivoids;    //< DE of voids and their Orientation Flag
    std::list<std::pair<IGES_ENTITY_514*, bool> > mvoids;   //< voids and their Orientation Flag

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
    MCAD_API IGES_ENTITY_186( IGES* aParent );
    virtual MCAD_API ~IGES_ENTITY_186();

    // Inherited virtual functions
    virtual MCAD_API bool SetEntityForm( int aForm );

    // functions unique to E186
    // XXX - TO BE IMPLEMENTED
};

#endif  // ENTITY_186_H
