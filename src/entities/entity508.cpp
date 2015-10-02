/*
 * file: entity508.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 508: Loop, Section 4.149, p.590+ (618+)
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

#include <sstream>

#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity508.h>


using namespace std;


LOOP_PAIR::LOOP_PAIR()
{
    orientFlag = false;
    curve = NULL;
    return;
}


LOOP_PAIR::LOOP_PAIR( bool aOrientFlag, IGES_ENTITY* aCurve )
{
    orientFlag = aOrientFlag;
    curve = aCurve;
    return;
}


LOOP_DATA::LOOP_DATA()
{
    isVertex = false;
    orientFlag = true;
    data = NULL;
    idx = 0;
}

bool LOOP_DATA::GetPCurves( size_t& aListSize, LOOP_PAIR**& aPCurveList )
{
    if( pcurves.empty() )
    {
        aListSize = 0;
        aPCurveList = NULL;
        return false;
    }

    aListSize = pcurves.size();
    aPCurveList = &pcurves[0];
    return true;
}


IGES_ENTITY_508::IGES_ENTITY_508( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 508;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY;         // required by specification

    return;
}


IGES_ENTITY_508::~IGES_ENTITY_508()
{
    // unlink all PS curves
    vector< LOOP_DATA* >::iterator sF = edges.begin();
    vector< LOOP_DATA* >::iterator eF = edges.end();

    while( sF != eF )
    {
        vector< LOOP_PAIR* >::iterator sP = (*sF)->pcurves.begin();
        vector< LOOP_PAIR* >::iterator eP = (*sF)->pcurves.end();

        while( sP != eP )
        {
            (*sP)->curve->unlink(this);
            delete *sP;
            ++sP;
        }

        delete *sF;
        ++sF;
    }

    edges.clear();

    // unlink the edge entities
    list< pair< IGES_ENTITY*, int > >::iterator sE = redges.begin();
    list< pair< IGES_ENTITY*, int > >::iterator eE = redges.end();

    while( sE != eE )
    {
        sE->first->unlink(this);
        ++sE;
    }

    redges.clear();
    return;
}


bool IGES_ENTITY_508::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        deItems.clear();
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    if( deItems.empty() )
        return true;

    IGES_ENTITY* p0;
    list<LOOP_DEIDX>::iterator sI = deItems.begin();
    list<LOOP_DEIDX>::iterator eI = deItems.end();
    int nI = (int)entities->size();
    int lI;
    LOOP_DATA* ldata;

    while( sI != eI )
    {
        ldata = new LOOP_DATA;
        ldata->isVertex = sI->isVertex;
        ldata->data = NULL;
        ldata->idx  = sI->idx;
        ldata->orientFlag = sI->orientFlag;
        ldata->pcurves.clear();
        lI = sI->data >> 1;

        if( 0 > lI || nI <= lI )
        {
            ERRMSG << "\n + [CORRUPT FILE] edge index exceeds number of entities in DE ";
            cerr << sequenceNumber << "\n";
            deItems.clear();
            return false;
        }

        ldata->data = (*entities)[lI];
        list< pair<bool, int> >::iterator sP = sI->pcurves.begin();
        list< pair<bool, int> >::iterator eP = sI->pcurves.end();

        while( sP != eP )
        {
            lI = sP->second >> 1;

            if( 0 > lI || nI <= lI )
            {
                ERRMSG << "\n + [CORRUPT FILE] PS curve index exceeds number of entities in DE";
                cerr << sequenceNumber << "\n";
                deItems.clear();
                return false;
            }

            p0 = (*entities)[lI];
            LOOP_PAIR* lp = new LOOP_PAIR;
            lp->orientFlag = sP->first;
            lp->curve = p0;
            ldata->pcurves.push_back( lp );
            ++sP;
        }

        if( !AddEdge( ldata ) )
        {
            delete ldata;
            ERRMSG << "\n + [INFO] could not add edge data for entity ";
            cerr << sequenceNumber << "\n";
            deItems.clear();
            return false;
        }

        ldata = NULL;
        ++sI;
    }

    deItems.clear();
    return true;
}


bool IGES_ENTITY_508::format( int &index )
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

    vector< LOOP_DATA* >::iterator sV = edges.begin();
    vector< LOOP_DATA* >::iterator eV = edges.end();
    vector< LOOP_DATA* >::iterator iV = --edges.end();
    int acc = 0;

    while( sV != eV )
    {
        if( !(*sV)->data )
        {
            ERRMSG << "\n + [BUG] null pointer in Loop structure\n";
            pdout.clear();
            return false;
        }

        // isVertex(n)
        ostr.str("");

        if( (*sV)->isVertex )
            ostr << "1" << pd;
        else
            ostr << "0" << pd;

        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        // edge(n)
        ostr.str("");
        ostr << (*sV)->data->getDESequence() << pd;
        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        // idx(n)
        ostr.str("");
        ostr << (*sV)->idx << pd;
        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        // OF(n)
        ostr.str("");

        if( (*sV)->orientFlag )
            ostr << "1" << pd;
        else
            ostr << "0" << pd;

        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        // K(n)
        ostr.str("");

        if( sV == iV && extras.empty() )
            ostr << (*sV)->pcurves.size() << rd;
        else
            ostr << (*sV)->pcurves.size() << pd;

        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        // write out PS curve data
        vector< LOOP_PAIR* >::iterator sP = (*sV)->pcurves.begin();
        vector< LOOP_PAIR* >::iterator eP = (*sV)->pcurves.end();
        vector< LOOP_PAIR* >::iterator iP = --((*sV)->pcurves.end());

        while( sP != iP )
        {
            // ISOP(n,k)
            ostr.str("");

            if( (*sP)->orientFlag )
                ostr << "1" << pd;
            else
                ostr << "0" << pd;

            tStr = ostr.str();
            AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

            // CURV(n, k)
            ostr.str("");

            if( sV == iV && sP == iP && extras.empty() )
                ostr << (*sP)->curve->getDESequence() << rd;
            else
                ostr << (*sP)->curve->getDESequence() << pd;

            tStr = ostr.str();
            AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

            ++sP;
        }

        ++acc;
        ++sV;
    }

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


bool IGES_ENTITY_508::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_508::unlink(IGES_ENTITY *aChildEntity)
{
    if(IGES_ENTITY::unlink(aChildEntity) )
        return true;

    int eType = aChildEntity->GetEntityType();

    if( ENT_VERTEX == eType || ENT_EDGE == eType )
    {
        if( delEdge( aChildEntity, true, true ) )
            return true;

        ERRMSG << "\n +[BUG] failed to unlink edge entity from E508\n";
        return false;
    }

    if( delPCurve( aChildEntity, true, true ) )
        return true;

    ERRMSG << "\n +[BUG] failed to unlink entity " << aChildEntity->GetEntityType();
    cerr << " from E508\n";
    return false;
}


bool IGES_ENTITY_508::isOrphaned( void )
{
    if( refs.empty() || edges.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_508::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    isDuplicate = false;

    // check for circular refs
    if( this == aParentEntity )
    {
        ERRMSG << "\n + [BUG] self-reference requested\n";
        return false;
    }

    vector< LOOP_DATA* >::iterator sF = edges.begin();
    vector< LOOP_DATA* >::iterator eF = edges.end();

    while( sF != eF )
    {
        if( aParentEntity == (*sF)->data )
        {
            ERRMSG << "\n + [BUG] circular reference with curve entity requested\n";
            return false;
        }

        vector< LOOP_PAIR* >::iterator sP = (*sF)->pcurves.begin();
        vector< LOOP_PAIR* >::iterator eP = (*sF)->pcurves.end();

        while( sP != eP )
        {
            if( (*sP)->curve == aParentEntity )
            {
                ERRMSG << "\n + [BUG] circular reference with PS curve entity requested\n";
                return false;
            }

            ++sP;
        }

        ++sF;
    }

    bool ok = IGES_ENTITY::addReference(aParentEntity, isDuplicate);

    if( ok )
        return true;

    ERRMSG << "\n + [INFO] could not add parent reference\n";
    return false;
}


bool IGES_ENTITY_508::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_508::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    depends = STAT_DEP_PHY;         // required
    view = 0;                       // N.A.
    transform = 0;                  // N.A.

    if( 1 != form && 0 != form )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in Loop\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_508::readPD(std::ifstream &aFile, int &aSequenceVar)
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

    int nE; // number of edge tuples

    if( !ParseInt( pdout, idx, nE, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of edges tuples\n";
        pdout.clear();
        return false;
    }

    if( nE < 1 )
    {
        ERRMSG << "\n + [INFO] invalid number of edges: " << nE << "\n";
        pdout.clear();
        return false;
    }

    int tmpI;   // temporary integer
    int tI1;    // temporary integer
    int nP;     // number of parameter space curves associated with an edge

    // read a tuple and associated data
    for( int i = 0; i < nE; ++i )
    {
        LOOP_DEIDX tDI;
        tDI.pcurves.clear();

        if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the TYPE flag\n";
            pdout.clear();
            return false;
        }

        switch( tmpI )
        {
            case 0:
                tDI.isVertex = false;
                break;

            case 1:
                tDI.isVertex = true;
                break;

            default:
                ERRMSG << "\n + [INFO] invalid TYPE flag: " << tmpI << "\n";
                pdout.clear();
                return false;
                break;
        }

        if( !ParseInt( pdout, idx, tDI.data, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the edge DE\n";
            pdout.clear();
            return false;
        }

        if( !ParseInt( pdout, idx, tDI.idx, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read index into edge list\n";
            pdout.clear();
            return false;
        }

        if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the orientation flag\n";
            pdout.clear();
            return false;
        }

        switch( tmpI )
        {
            case 0:
                tDI.orientFlag = false;
                break;

            case 1:
                tDI.orientFlag = true;
                break;

            default:
                ERRMSG << "\n + [INFO] invalid orientation flag: " << tmpI << "\n";
                pdout.clear();
                return false;
                break;
        }

        // number of associated parameter space curves
        if( !ParseInt( pdout, idx, nP, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the number of PS curves\n";
            pdout.clear();
            return false;
        }

        if( nP < 0 )
        {
            ERRMSG << "\n + [INFO] invalid number of parameter space curves: " << nP << "\n";
            pdout.clear();
            return false;
        }

        for( int j = 0; j < nP; ++j )
        {
            if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
            {
                ERRMSG << "\n + [INFO] couldn't read the ISOP flag of a PS curve\n";
                pdout.clear();
                return false;
            }

            if( !ParseInt( pdout, idx, tI1, eor, pd, rd ) )
            {
                ERRMSG << "\n + [INFO] couldn't read the DE of a PS curve\n";
                pdout.clear();
                return false;
            }

            switch( tmpI )
            {
                case 0:
                    tDI.pcurves.push_back( pair<bool, int>( false, tI1 ) );
                    break;

                case 1:
                    tDI.pcurves.push_back( pair<bool, int>( true, tI1 ) );
                    break;

                default:
                    ERRMSG << "\n + [INFO] ISOP flag: " << tmpI << "\n";
                    pdout.clear();
                    return false;
                    break;
            }
        }

        deItems.push_back( tDI );
    }   // read in an edge tuple

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


bool IGES_ENTITY_508::SetEntityForm( int aForm )
{
    if( 1 == aForm || 0 == aForm )
        return true;

    // note: the specification document states that the available forms are
    // 0 and 1, but only Form 1 is specified. Assuming that the specification
    // is not in error, this software should accept both 0 and 1 as valid
    // forms but only write Form 1 on output.
    ERRMSG << "\n + [BUG] Loop Entity only supports Form 0/1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_508::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Loop Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_508::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Loop Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_508::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    hierarchy = aHierarchy;
    return true;
}


bool IGES_ENTITY_508::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


// add a parent reference to a Vertex or Edge list entity and maintain a refcount
bool IGES_ENTITY_508::addEdge( IGES_ENTITY* aEdge )
{
    list<pair<IGES_ENTITY*, int> >::iterator sE = redges.begin();
    list<pair<IGES_ENTITY*, int> >::iterator eE = redges.end();

    while( sE != eE )
    {
        if( sE->first == aEdge )
        {
            ++sE->second;
            return true;
        }

        ++sE;
    }

    bool dup = false;

    if( !aEdge->addReference(this, dup) )
    {
        ERRMSG << "\n + [INFO]: could not add parent entity to edge\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: duplicate reference to edge\n";
        return false;
    }

    redges.push_back(pair<IGES_ENTITY*, int>( aEdge, 1 ) );

    if( NULL != parent && parent != aEdge->GetParentIGES() )
        parent->AddEntity( (IGES_ENTITY*)aEdge );

    return true;
}


// decrement refcount and release entity if appropriate; aFlagAll indicates
// that all LOOP_DATA structures containing this edge and their associated
// PCurves should be released
bool IGES_ENTITY_508::delEdge( IGES_ENTITY* aEdge, bool aFlagAll, bool aFlagUnlink )
{
    bool ok = false;    // flag indicates true if the entity has been matched

    list<pair<IGES_ENTITY*, int> >::iterator sE = redges.begin();
    list<pair<IGES_ENTITY*, int> >::iterator eE = redges.end();

    while( sE != eE )
    {
        IGES_ENTITY* ep = sE->first;

        if( aEdge == ep )
        {
            ok = true;

            if( !aFlagUnlink )
                ep->delReference(this);

            if( aFlagAll || (--sE->second == 0) )
            {
                vector< LOOP_DATA* >::reverse_iterator sF = edges.rbegin();
                vector< LOOP_DATA* >::reverse_iterator eF = edges.rend();

                while( sF != eF )
                {
                    if( (*sF)->data == ep )
                    {
                        vector< LOOP_PAIR* >::reverse_iterator sP = (*sF)->pcurves.rbegin();
                        vector< LOOP_PAIR* >::reverse_iterator eP = (*sF)->pcurves.rend();

                        while( sP != eP )
                        {
                            delPCurve( (*sP)->curve, false, false );
                            delete *sP;
                            ++sP;
                        }

                        delete *sF;
                        --sF;
                        edges.pop_back();
                        continue;
                    }

                    ++sF;
                }

                sE = redges.erase( sE );
                continue;
            }   // deleted last reference
        }   // found entity to delete

        ++sE;
    }   // searched list of entities

    return ok;
}


// add a parent reference to a parameter space curve and ensure no duplicates
bool IGES_ENTITY_508::addPCurve( IGES_ENTITY* aCurve )
{
    vector< LOOP_DATA* >::iterator sF = edges.begin();
    vector< LOOP_DATA* >::iterator eF = edges.end();

    while( sF != eF )
    {
        vector< LOOP_PAIR* >::iterator sP = (*sF)->pcurves.begin();
        vector< LOOP_PAIR* >::iterator eP = (*sF)->pcurves.end();

        while( sP != eP )
        {
            if( (*sP)->curve == aCurve )
            {
                ERRMSG << "\n + [BUG]: duplicate reference to PS curve\n";
                return false;
            }

            ++sP;
        }

        ++sF;
    }

    bool dup = false;

    if( !aCurve->addReference(this, dup) )
    {
        ERRMSG << "\n +[INFO] could not add parent reference to PS curve\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: unhandled duplicate reference to PS curve\n";
        return false;
    }

    if( NULL != parent && parent != aCurve->GetParentIGES() )
        parent->AddEntity( aCurve );

    return true;
}


// delete parent reference from the given parameter space curve
bool IGES_ENTITY_508::delPCurve( IGES_ENTITY* aCurve, bool aFlagDelEdge, bool aFlagUnlink )
{
    vector< LOOP_DATA* >::iterator sF = edges.begin();
    vector< LOOP_DATA* >::iterator eF = edges.end();

    while( sF != eF )
    {
        vector< LOOP_PAIR* >::reverse_iterator sP = (*sF)->pcurves.rbegin();
        vector< LOOP_PAIR* >::reverse_iterator eP = (*sF)->pcurves.rend();

        while( sP != eP )
        {
            if( (*sP)->curve == aCurve )
            {
                if( aFlagDelEdge )
                {
                    while( !(*sF)->pcurves.empty() )
                    {
                        (*sF)->pcurves.back()->curve->delReference(this);
                        (*sF)->pcurves.pop_back();
                    }

                    delEdge( (*sF)->data, false, false );
                }
                else
                {
                    if( !aFlagUnlink )
                        (*sP)->curve->delReference(this);

                    delete *sP;
                    (*sF)->pcurves.pop_back();
                }

                return true;
            }

            ++sP;
        }

        ++sF;
    }

    return false;
}


bool IGES_ENTITY_508::GetLoopData( size_t aListSize, LOOP_DATA**& aEdgeList )
{
    if( edges.empty() )
    {
        aListSize = 0;
        aEdgeList = NULL;
        return false;
    }

    aListSize = edges.size();
    aEdgeList = &edges[0];
    return true;
}


bool IGES_ENTITY_508::AddEdge( LOOP_DATA*& aEdge )
{
    if( NULL == aEdge->data )
    {
        ERRMSG << "\n +[BUG] NULL pointer passed for edge\n";
        return false;
    }

    if( !addEdge( aEdge->data ) )
    {
        ERRMSG << "\n +[INFO] could not add edge to list\n";
        return false;
    }

    vector< LOOP_PAIR* >::iterator sP = aEdge->pcurves.begin();
    vector< LOOP_PAIR* >::iterator eP = aEdge->pcurves.end();

    while( sP != eP )
    {
        if( !addPCurve( (*sP)->curve ) )
        {
            while( sP != aEdge->pcurves.begin() )
            {
                (*sP)->curve->delReference(this);
                --sP;
            }

            (*sP)->curve->delReference(this);
            ERRMSG << "\n +[INFO] could not add pcurve to list\n";
            return false;
        }

        ++sP;
    }

    edges.push_back( aEdge );
    return true;
}
