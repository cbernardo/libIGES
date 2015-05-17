/*
 * file: entity504.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 504: Edge, Section 4.148, p.588+ (616+)
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

#ifndef ENTITY_504_H
#define ENTITY_504_H

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

class IGES_ENTITY_502;

struct EDGE_DEIDX
{
    int curv;
    int svp;
    int tvp;
    int sv;
    int tv;

    EDGE_DEIDX()
    {
        curv = 0;
        svp = 0;
        tvp = 0;
        sv = 0;
        tv = 0;
    }
};

struct EDGE_DATA
{
    IGES_ENTITY* curv;
    IGES_ENTITY_502* svp;
    IGES_ENTITY_502* tvp;
    int sv;
    int tv;

    EDGE_DATA()
    {
        curv = NULL;
        svp = NULL;
        tvp = NULL;
        sv = 0;
        tv = 0;
    }
};

class IGES_ENTITY_504 : public IGES_ENTITY
{
protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<EDGE_DEIDX> deItems;  // Data for EDGE, including DE indices
    std::list<EDGE_DATA> edges;
    std::vector<EDGE_DATA> vedges;   // EDGE data (pointers and values)
    std::list< std::pair<IGES_ENTITY_502*, int> > vertices; // counts for vertex references

public:
    IGES_ENTITY_504( IGES* aParent );
    virtual ~IGES_ENTITY_504();

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
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // functions unique to E504
    std::vector<EDGE_DATA>* GetEdges( void );
    void AddEdge( IGES_ENTITY* aCurve, IGES_ENTITY_502* aSVP, int aSV,
                  IGES_ENTITY_502* aTVP, int aTV );
};

#endif  // ENTITY_504_H
