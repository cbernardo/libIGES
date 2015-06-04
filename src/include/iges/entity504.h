/*
 * file: entity504.h
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
 * Description: IGES Entity 504: Edge, Section 4.148, p.588+ (616+)
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


/**
 * Struct EDGE_DEIDX
 * stores information on referenced entities to be read in from an IGES file
 */
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


/**
 * Struct EDGE_DATA
 * stores information on instantiated entities representing
 * part of an Edge entity
 */
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


/**
 * Class IGES_ENTITY_504
 * represents the Edge Entity
 */
class IGES_ENTITY_504 : public IGES_ENTITY
{
private:
    /// add a parent reference to a curve and ensure that it is not a duplicate
    bool addCurve( IGES_ENTITY* aCurve );

    /// add a parent reference to a Vertex List and maintain a reference count
    bool addVertexList( IGES_ENTITY_502* aVertexList );

    /// decrement a Vertex List's reference count and delete references if appropriate
    bool delVertexList( IGES_ENTITY_502* aVertexList, bool aFlagAll );

protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<EDGE_DEIDX> deItems;  //< Data for EDGE, including DE indices
    std::list<EDGE_DATA> edges;     //< Data for entities references by this Edge

    ///< EDGE data (pointers and values) which may be passed to users for convenience
    std::vector<EDGE_DATA> vedges;

    std::list< std::pair<IGES_ENTITY_502*, int> > vertices; //< counts for vertex references

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
    // parameters not supported by the specification:
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    virtual bool SetView( IGES_ENTITY* aView );
    virtual bool SetColor( IGES_COLOR aColor );
    virtual bool SetColor( IGES_ENTITY* aColor );
    virtual bool SetLineWeightNum( int aLineWeight );

    // functions unique to E504

    /**
     * Function GetEdges
     * returns a vector containing Edge data for convenient access by users
     */
    std::vector<EDGE_DATA>* GetEdges( void );


    /**
     * Function AddEdge
     * adds information to represent a section of this Edge entity
     * and returns true on success.
     *
     * @param aCurve = pointer to a valid curve entity (see sec. 4.148 of the specification)
     * @param aSVP = pointer to a Vertex List entity which describes a section of this edge
     * @param aSV = index into the vertex list @param aSVP to specify the Start Point of this edge segment
     * @param aTVP = pointer to a Vertex List entity which describes a section of this edge
     * @param aTV = index into the vertex list @param aTVP to specify the End Point of this edge segment
     */
    bool AddEdge( IGES_ENTITY* aCurve, IGES_ENTITY_502* aSVP, int aSV,
                  IGES_ENTITY_502* aTVP, int aTV );
};

#endif  // ENTITY_504_H
