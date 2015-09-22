/*
 * file: entity504.cpp
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

#include <error_macros.h>
#include <sstream>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity502.h>
#include <entity504.h>
#include "../include/iges/entity504.h"

using namespace std;


IGES_ENTITY_504::IGES_ENTITY_504( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 504;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY;         // required by specification
    use = STAT_USE_GEOMETRY;
    hierarchy = STAT_HIER_NO_SUB;   // required by specification

    return;
}


IGES_ENTITY_504::~IGES_ENTITY_504()
{
    deItems.clear();
    vedges.clear();

    std::list<EDGE_DATA>::iterator sE = edges.begin();
    std::list<EDGE_DATA>::iterator eE = edges.end();

    while( sE != eE )
    {
        if( sE->curv )
        {
            ERRMSG << "\nXXX + [INFO] deleting ref to curve entity " << sE->curv << "\n";
            sE->curv->delReference(this);
        }

        ++sE;
    }

    edges.clear();

    list< pair<IGES_ENTITY_502*, int> >::iterator sV = vertices.begin();
    list< pair<IGES_ENTITY_502*, int> >::iterator eV = vertices.end();

    while( sV != eV )
    {
        sV->first->delReference(this);
        ++sV;
    }

    vertices.clear();
    return;
}


void IGES_ENTITY_504::Compact( void )
{
    vedges.clear();
    return;
}


bool IGES_ENTITY_504::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        deItems.clear();
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    if( deItems.empty() )
        return true;

    IGES_ENTITY_502* lp0;
    IGES_ENTITY_502* lp1;
    IGES_ENTITY*     cp;

    list<EDGE_DEIDX>::iterator sI = deItems.begin();
    list<EDGE_DEIDX>::iterator eI = deItems.end();
    int nI = (int)entities->size();
    int lI;

    while( sI != eI )
    {
        lI = sI->curv >> 1;

        if( 0 > lI || nI <= lI )
        {
            ERRMSG << "\n + [CORRUPT FILE] curve index exceeds number of entities in DE ";
            cerr << sequenceNumber << "\n";
            deItems.clear();
            return false;
        }

        cp = (*entities)[lI];

        lI = sI->svp >> 1;

        if( 0 > lI || nI <= lI )
        {
            ERRMSG << "\n + [CORRUPT FILE] SVP index exceeds number of entities in DE";
            cerr << sequenceNumber << "\n";
            deItems.clear();
            return false;
        }

        lp0 = (IGES_ENTITY_502*)((*entities)[lI]);

        lI = sI->tvp >> 1;

        if( 0 > lI || nI <= lI )
        {
            ERRMSG << "\n + [CORRUPT FILE] TVP index exceeds number of entities in DE";
            cerr << sequenceNumber << "\n";
            deItems.clear();
            return false;
        }

        lp1 = (IGES_ENTITY_502*)((*entities)[lI]);

        if( !AddEdge( cp, lp0, sI->sv, lp1, sI->tv ) )
        {
            ERRMSG << "\n + [INFO] could not add edge reference\n";
            deItems.clear();
            return false;
        }

        ++sI;
    }

    deItems.clear();
    return true;
}


bool IGES_ENTITY_504::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    parameterData = index;

    if( !parent )
    {
        ERRMSG << "\n + [INFO] method invoked with no parent IGES object\n";
        return false;
    }

    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << edges.size() << pd;
    string fStr = ostr.str();
    string tStr;

    list<EDGE_DATA>::iterator sV = edges.begin();
    list<EDGE_DATA>::iterator eV = --edges.end();
    int acc = 0;

    while( sV != eV )
    {
        if( !sV->curv || !sV->svp || !sV->tvp )
        {
            ERRMSG << "\n + [BUG] null pointer in Edge structure\n";
            pdout.clear();
            return false;
        }

        ostr.str("");
        ostr << sV->curv->getDESequence() << pd;
        ostr << sV->svp->getDESequence() << pd;
        ostr << sV->sv << pd;
        ostr << sV->tvp->getDESequence() << pd;
        ostr << sV->tv << pd;
        tStr = ostr.str();

        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        ++acc;
        ++sV;
    }

    if( !sV->curv || !sV->svp || !sV->tvp )
    {
        ERRMSG << "\n + [BUG] null pointer in Edge structure\n";
        pdout.clear();
        return false;
    }

    // note: 2 sets of OPTIONAL parameters may exist at the end of
    // any PD; see p.32/60+ for details; if optional parameters
    // need to be written then we should use 'pd' rather than 'rd'
    // in this call to FormatPDREal()
    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    ostr.str("");
    ostr << sV->curv->getDESequence() << pd;
    ostr << sV->svp->getDESequence() << pd;
    ostr << sV->sv << pd;
    ostr << sV->tvp->getDESequence() << pd;
    ostr << sV->tv << idelim;
    tStr = ostr.str();

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !extras.empty() && !formatExtraParams( fStr, index, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] could not format optional parameters\n";
        pdout.clear();
        iExtras.clear();
        return false;
    }

    if( !formatComments( index ) )
    {
        ERRMSG << "\n + [INFO] could not format comments\n";
        pdout.clear();
        return false;
    }

    paramLineCount = index - parameterData;

    return true;
}


bool IGES_ENTITY_504::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_504::unlink(IGES_ENTITY *aChildEntity)
{
    if(IGES_ENTITY::unlink(aChildEntity) )
        return true;

    int eType = aChildEntity->GetEntityType();
    list<EDGE_DATA>::iterator sE = edges.begin();
    list<EDGE_DATA>::iterator eE = edges.end();

    if( 502 == eType )
    {
        if( delVertexList( (IGES_ENTITY_502*)aChildEntity, true ) )
        {
            // we must disassociate all curves referencing the vertex list
            while( sE != eE )
            {
                if( aChildEntity == sE->svp )
                {
                    sE->curv->delReference(this);

                    if( sE->tvp != sE->svp )
                        delVertexList( sE->tvp, false );

                    sE = edges.erase( sE );
                    continue;
                }
                else if( aChildEntity == sE->tvp )
                {
                    sE->curv->delReference(this);

                    if( sE->tvp != sE->svp )
                        delVertexList( sE->svp, false );

                    sE = edges.erase( sE );
                    continue;
                }

                ++sE;
            }

            return true;
        }

        ERRMSG << "\n + [INFO] unlink() invoked on an unowned Vertex List entity\n";
        return false;
    }

    // check if this is a curve entity
    while( sE != eE )
    {
        if( aChildEntity == sE->curv )
        {
            delVertexList( sE->svp, false );
            delVertexList( sE->tvp, false );
            edges.erase( sE );
            return true;
        }

        ++sE;
    }

    ERRMSG << "\n + [INFO] unlink() invoked on an unowned entity\n";
    return false;
}


bool IGES_ENTITY_504::isOrphaned( void )
{
    if( refs.empty() || edges.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_504::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    isDuplicate = false;

    // check for circular refs
    if( this == aParentEntity )
    {
        ERRMSG << "\n + [BUG] self-reference requested\n";
        return false;
    }

    list< pair<IGES_ENTITY_502*, int> >::iterator sV = vertices.begin();
    list< pair<IGES_ENTITY_502*, int> >::iterator eV = vertices.end();

    while( sV != eV )
    {
        if( this == (IGES_ENTITY*)sV->first )
        {
            ERRMSG << "\n + [BUG] circular reference with vertex list requested\n";
            return false;
        }

        ++sV;
    }

    list<EDGE_DATA>::iterator sE = edges.begin();
    list<EDGE_DATA>::iterator eE = edges.end();

    while( sE != eE )
    {
        if( aParentEntity == sE->curv )
        {
            ERRMSG << "\n + [BUG] circular reference with curve entity requested\n";
            return false;
        }

        ++sE;
    }

    bool ok = IGES_ENTITY::addReference(aParentEntity, isDuplicate);

    if( ok )
        return true;

    ERRMSG << "\n + [INFO] could not add parent reference\n";
    return false;
}


bool IGES_ENTITY_504::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_504::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_NO_SUB;   // required
    depends = STAT_DEP_PHY;         // required
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    lineWeightNum = 0;              // N.A.
    colorNum = 0;                   // N.A.

    if( form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Edge List\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_504::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Edge Entity\n";
        pdout.clear();
        return false;
    }

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

	idx = (int)pdout.find(pd);

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    int nV;

    if( !ParseInt( pdout, idx, nV, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of edges in the list\n";
        pdout.clear();
        return false;
    }

    if( nV < 1 )
    {
        ERRMSG << "\n + [INFO] invalid number of edges: " << nV << "\n";
        pdout.clear();
        return false;
    }

    EDGE_DEIDX deidx;
    int* ip[5] = { &deidx.curv, &deidx.svp, &deidx.sv, &deidx.tvp, &deidx.tv };

    for( int i = 0; i < nV; ++i )
    {
        for( int j = 0; j < 5; ++j )
        {
            if( !ParseInt( pdout, idx, *ip[j], eor, pd, rd ) )
            {
                ERRMSG << "\n + [BAD FILE] no datum for edge " << i << "\n";
                pdout.clear();
                return false;
            }
        }

        deItems.push_back( deidx );
    }

    if( !eor && !readExtraParams( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read optional pointers\n";
        pdout.clear();
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
        pdout.clear();
        return false;
    }

    pdout.clear();
    // note: no need to attempt any scaling

    return true;
}


bool IGES_ENTITY_504::SetEntityForm( int aForm )
{
    if( aForm == 1 )
        return true;

    ERRMSG << "\n + [BUG] Edge Entity only supports Form 1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_504::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Edge Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_504::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Edge Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_504::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    if( STAT_HIER_NO_SUB == aHierarchy )
        return true;

    ERRMSG << "\n + [BUG] Edge Entity only supports hierarchy = STAT_HIER_NO_SUB\n";
    return true;
}


MCAD_API bool
IGES_ENTITY_504::GetEdges( size_t aListSize, EDGE_DATA const*& aEdgeList )
{
    if( edges.empty() )
    {
        vedges.clear();
        aListSize = 0;
        aEdgeList = NULL;
        return false;
    }

    if( vedges.empty() || vedges.size() != edges.size() )
    {
        vedges.clear();
        std::list<EDGE_DATA>::iterator sV = edges.begin();
        std::list<EDGE_DATA>::iterator eV = edges.end();

        while( sV != eV )
        {
            vedges.push_back( *sV );
            ++sV;
        }
    }

    aListSize = 0;
    aEdgeList = &vedges[0];
    return false;
}


bool IGES_ENTITY_504::AddEdge( IGES_ENTITY* aCurve,
                               IGES_ENTITY_502* aSVP, int aSV,
                               IGES_ENTITY_502* aTVP, int aTV )
{
    if( !addCurve( aCurve ) )
    {
        ERRMSG << "\n + [INFO] could not add curve to entity list\n";
        return false;
    }

    if( !addVertexList( aSVP ) )
    {
        aCurve->delReference(this);
        ERRMSG << "\n + [INFO] could not add Vertex List aSVP to entity list\n";
        return false;
    }

    if( !addVertexList( aTVP ) )
    {
        aCurve->delReference(this);
        delVertexList( aSVP, false );
        ERRMSG << "\n + [INFO] could not add Vertex List aTVP to entity list\n";
        return false;
    }

    if( aSV < 1 || aSV > (int)aSVP->GetNVertices() )
    {
        aCurve->delReference(this);
        delVertexList( aSVP, false );
        delVertexList( aTVP, false );
        ERRMSG << "\n + [BUG] aSVP index (" << aSV << ") exceeds list size (";
        cerr << aSVP->GetNVertices() << ")\n";
        return false;
    }

    if( aTV < 1 || aTV > (int)aTVP->GetNVertices() )
    {
        aCurve->delReference(this);
        delVertexList( aSVP, false );
        delVertexList( aTVP, false );
        ERRMSG << "\n + [BUG] aTVP index (" << aTV << ") exceeds list size (";
        cerr << aTVP->GetNVertices() << ")\n";
        return false;
    }

    EDGE_DATA nc;
    nc.curv = aCurve;
    nc.svp = aSVP;
    nc.tvp = aTVP;
    nc.sv = aSV;
    nc.tv = aTV;

    edges.push_back( nc );
    return true;
}


// add a parent reference to a curve and ensure that it is not a duplicate
bool IGES_ENTITY_504::addCurve( IGES_ENTITY* aCurve )
{
    if( !aCurve )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for curve entity\n";
        return false;
    }

    int eType = aCurve->GetEntityType();

    switch( eType )
    {
        case 100:
        case 102:
        case 104:
        case 110:
        case 112:
        case 126:
        case 130:
            break;

        case 106:
            do
            {
                int fn = aCurve->GetEntityForm();

                if( 11 != fn && 12 != fn && 63 != fn )
                {
                    ERRMSG << "\n + [BUG] invalid Type 106 form number (" << fn;
                    cerr << "); only forms 11, 12, and 63 are accepted\n";
                    return false;
                }

            } while( 0 );

            break;

        default:
            ERRMSG << "\n + [BUG] invalid entity specified (Type " << eType << ")\n";
            return false;
            break;
    }

    bool dup = false;

    if( !aCurve->addReference(this, dup) )
    {
        ERRMSG << "\n + [INFO] could not add parent reference to curve\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG] duplicate curve entity\n";
        return false;
    }

    return true;
}


// add a parent reference to a Vertex List and maintain a reference count
bool IGES_ENTITY_504::addVertexList( IGES_ENTITY_502* aVertexList )
{
    if( !aVertexList )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for vertex list entity\n";
        return false;
    }

    list< pair<IGES_ENTITY_502*, int> >::iterator sV = vertices.begin();
    list< pair<IGES_ENTITY_502*, int> >::iterator eV = vertices.end();

    while( sV != eV )
    {
        if( sV->first == aVertexList )
        {
            ++sV->second;
            return true;
        }

        ++sV;
    }

    bool dup = false;

    if( !aVertexList->addReference(this, dup) )
    {
        ERRMSG << "\n + [INFO] could not add parent reference to vertex list\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG] internal vertex list is inconsistent\n";
        return false;
    }

    vertices.push_back( pair<IGES_ENTITY_502*, int>(aVertexList, 1) );
    return true;
}


// decrement a Vertex List's reference count and delete references if appropriate
bool IGES_ENTITY_504::delVertexList( IGES_ENTITY_502* aVertexList, bool aFlagAll )
{
    if( !aVertexList )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for vertex list entity\n";
        return false;
    }

    list< pair<IGES_ENTITY_502*, int> >::iterator sV = vertices.begin();
    list< pair<IGES_ENTITY_502*, int> >::iterator eV = vertices.end();

    while( sV != eV )
    {
        if( sV->first == aVertexList )
        {
            --sV->second;

            if( aFlagAll || 0 == sV->second )
            {
                sV->first->delReference(this);
                vertices.erase( sV );
            }

            return true;
        }

        ++sV;
    }

    return false;
}


bool IGES_ENTITY_504::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_504::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_504::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_504::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_504::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_504::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}
