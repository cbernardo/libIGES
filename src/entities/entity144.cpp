/*
 * file: entity144.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 144: Trimmed Parametric Surface, Section 4.34, p.181 (209+)
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
#include <core/iges.h>
#include <core/iges_io.h>
#include <core/entity124.h>
#include <core/entity142.h>
#include <core/entity144.h>

using namespace std;

static bool checkInclusion144( int aEnt )
{
    // ensure the entity is one of:
    // E106-63 (copious data)
    // E108 (plane)
    // E114 (parametric spline surface)
    // E118 (ruled surface)
    // E120 (surface of revolution)
    // E122 (tabulated cylinder)
    // E128 (NURBS surface)
    // E140 (offset surface)
    // E143 (bounded surface)
    // E190 (plane surface)
    // E192 (right circular cylindrical surface)
    // E194 (right circular conical surface)
    // E196 (spherical surface)
    // E198 (toroidal surface)

    switch( aEnt )
    {
        case ENT_COPIOUS_DATA:
        case ENT_PLANE:
        case ENT_PARAM_SPLINE_SURFACE:
        case ENT_RULED_SURFACE:
        case ENT_SURFACE_OF_REVOLUTION:
        case ENT_TABULATED_CYLINDER:
        case ENT_NURBS_SURFACE:
        case ENT_OFFSET_SURFACE:
        case ENT_BOUNDED_SURFACE:
        case ENT_PLANE_SURFACE:
        case ENT_RIGHT_CIRCULAR_CYLINDRICAL_SURFACE:
        case ENT_RIGHT_CIRCULAR_CONICAL_SURFACE:
        case ENT_SPHERICAL_SURFACE:
        case ENT_TOROIDAL_SURFACE:
            break;

        default:
            return false;
            break;
    }

    return true;
}


IGES_ENTITY_144::IGES_ENTITY_144( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 144;
    form = 0;

    N1 = 0;
    N2 = 0;
    iPTS = 0;
    iPTO = 0;
    PTS = NULL;
    PTO = NULL;

    return;
}


IGES_ENTITY_144::~IGES_ENTITY_144()
{
    if( PTS )
        PTS->delReference(this);

    if( PTO )
        PTO->delReference(this);

    std::list<IGES_ENTITY_142*>::iterator sPTI = PTI.begin();
    std::list<IGES_ENTITY_142*>::iterator ePTI = PTI.end();

    while( sPTI != ePTI )
    {
        (*sPTI)->delReference(this);
        ++sPTI;
    }

    return;
}


void IGES_ENTITY_144::Compact( void )
{
    IGES_ENTITY::Compact();
    vPTI.clear();
    return;
}


bool IGES_ENTITY_144::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] could not register associations\n";
        iPTI.clear();
        return false;
    }

    int iEnt;
    bool dup = false;

    if( iPTS )
    {
        iEnt = iPTS >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid surface entity (DE:" << iPTS << ")\n";
            iPTI.clear();
            return false;
        }

        PTS = (*entities)[iEnt];

        if( !checkInclusion144( PTS->GetEntityType()) )
        {
            ERRMSG << "\n + [INFO] invalid entity type (" << PTS->GetEntityType() << ") for PTS\n";
            PTS = NULL;
            return false;
        }

        if( !PTS->addReference(this, dup) )
        {
            PTS = NULL;
            ERRMSG << "\n + [INFO] could not associate surface entity with DE " << PTS << "\n";
            iPTI.clear();
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            PTS = NULL;
            iPTI.clear();
            return false;
        }

    }
    else
    {
        ERRMSG << "\n + [VIOLATION] unspecified surface entity\n";
        iPTI.clear();
        return false;
    }

    if( iPTO )
    {
        iEnt = iPTO >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid outline entity (DE:" << iPTO << ")\n";
            iPTI.clear();
            return false;
        }

        PTO = dynamic_cast<IGES_ENTITY_142*>((*entities)[iEnt]);

        if( NULL == PTO )
        {
            ERRMSG << "\n + [INFO] invalid outline entity (DE:" << iPTO << ") - not type 142\n";
            iPTI.clear();
            return false;
        }

        if( !PTO->addReference(this, dup) )
        {
            PTO = NULL;
            ERRMSG << "\n + [INFO] could not associate outline entity with DE " << iPTO << "\n";
            iPTI.clear();
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            PTO = NULL;
            iPTI.clear();
            return false;
        }

    }
    else
    {
        ERRMSG << "\n + [VIOLATION] unspecified boundary entity\n";
        iPTI.clear();
        return false;
    }

    int iDE;
    IGES_ENTITY_142* ep;
    std::list<int>::iterator sPTI = iPTI.begin();
    std::list<int>::iterator ePTI = iPTI.end();

    while( sPTI != ePTI )
    {
        iDE = *sPTI;

        if( iDE <= 0 || (iDE & 1) == 0 || iDE > 9999997 )
        {
            ERRMSG << "\n + [VIOLATION] invalid DE sequence number\n";
            iPTI.clear();
            return false;
        }

        iEnt = iDE >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid cutout entity (DE:" << iDE << ")\n";
            iPTI.clear();
            return false;
        }

        ep = dynamic_cast<IGES_ENTITY_142*>((*entities)[iEnt]);

        if( NULL == ep )
        {
            ERRMSG << "\n + [INFO] invalid cutout entity (DE:" << iDE << ") - not type 142\n";
            iPTI.clear();
            return false;
        }

        if( !ep->addReference(this, dup) )
        {
            ERRMSG << "\n + [INFO] could not associate cutout entity with DE " << iDE << "\n";
            iPTI.clear();
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [BUG]: adding duplicate entry\n";
            PTS = NULL;
            iPTI.clear();
            return false;
        }

        PTI.push_back( ep );
        ++sPTI;
    }

    iPTI.clear();
    return true;
}


bool IGES_ENTITY_144::format( int &index )
{
    pdout.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( NULL == PTS )
    {
        ERRMSG << "\n + [BUG] unspecified surface entity\n";
        return false;
    }

    if( N1 < 0 || N1 >  1 )
    {
        ERRMSG << "\n + [BUG] invalid value for N1 (" << N1 << ")\n";
        return false;
    }

	N2 = (int)PTI.size();

    iPTS = PTS->getDESequence();

    if( PTO )
        iPTO = PTO->getDESequence();
    else
        iPTO = 0;

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
    ostr << iPTS << pd;
    ostr << N1 << pd;
    ostr << N2 << pd;
    string lstr = ostr.str();
    string tstr;
    ostr.str("");

    if( PTI.empty() && extras.empty() )
        ostr << iPTO << rd;
    else
        ostr << iPTO << pd;

    tstr = ostr.str();
    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    int seqPTI;

    if( !PTI.empty() )
    {
        std::list<IGES_ENTITY_142*>::iterator sPTI = PTI.begin();
        std::list<IGES_ENTITY_142*>::iterator ePTI = PTI.end();

        while( sPTI != ePTI )
        {
            seqPTI = (*sPTI)->getDESequence();

            ++sPTI;
            ostr.str("");

            if( sPTI == ePTI && extras.empty() )
                ostr << seqPTI << rd;
            else
                ostr << seqPTI << pd;

            tstr = ostr.str();
            AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
        }
    }

    if( !extras.empty() && !formatExtraParams( lstr, index, pd, rd ) )
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


bool IGES_ENTITY_144::rescale( double sf )
{
    // there is nothing to scale so this function always succeeds
    return true;
}


bool IGES_ENTITY_144::unlink(IGES_ENTITY *aChild)
{
    if(IGES_ENTITY::unlink(aChild) )
        return true;

    if( aChild == PTS )
    {
        PTS = NULL;
        return true;
    }

    if( aChild == PTO )
    {
        PTO = NULL;
        return true;
    }

    if( !PTI.empty() )
    {
        std::list<IGES_ENTITY_142*>::iterator sPTI = PTI.begin();
        std::list<IGES_ENTITY_142*>::iterator ePTI = PTI.end();

        while( sPTI != ePTI )
        {
            if( aChild == *sPTI )
            {
                PTI.erase( sPTI );
                N2 = (int)PTI.size();
                return true;
            }

            ++sPTI;
        }
    }

    return false;
}


bool IGES_ENTITY_144::isOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT)
        || ( NULL == PTS ) )
        return true;

    return false;
}


bool IGES_ENTITY_144::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for aParentEntity\n";
        return false;
    }

    if( aParentEntity == PTS || aParentEntity == PTO )
    {
        ERRMSG << "\n + [BUG] circular reference requested\n";
        return false;
    }

    if( !PTI.empty() )
    {
        std::list<IGES_ENTITY_142*>::iterator sPTI = PTI.begin();
        std::list<IGES_ENTITY_142*>::iterator ePTI = PTI.end();

        while( sPTI != ePTI )
        {
            if( aParentEntity == *sPTI )
            {
                ERRMSG << "\n + [BUG] circular reference requested\n";
                return false;
            }

            ++sPTI;
        }
    }

    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_144::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_144::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    use = STAT_USE_GEOMETRY;        // fixed
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Trimmed Parametric Surface\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_144::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Surface of Revolution\n";
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

    if( !ParseInt( pdout, idx, iPTS, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read surface entity DE\n";
        pdout.clear();
        return false;
    }

    if( iPTS < 0 || iPTS > 9999997 || (iPTS & 1) == 0 )
    {
        ERRMSG << "\n + [INFO] invalid surface entity DE (" << iPTS << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, N1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read N1 parameter\n";
        pdout.clear();
        return false;
    }

    if( N1 < 0 || N1 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for N1 (" << N1 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, N2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read N2 parameter\n";
        pdout.clear();
        return false;
    }

    if( N2 < 0 )
    {
        ERRMSG << "\n + [INFO] invalid N2 parameter (" << N2 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, iPTO, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read outline DE pointer\n";
        pdout.clear();
        return false;
    }

    if( iPTO < 0 || (iPTO & 1) == 0 || iPTO > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid outline DE pointer (" << iPTO << ")\n";
        pdout.clear();
        return false;
    }

    int tIdx;

    for( int i = 0; i < N2; ++ i )
    {
        if( !ParseInt( pdout, idx, tIdx, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read cutout #" << (iPTI.size() + 1) << "\n";
            pdout.clear();
            return false;
        }

        if( tIdx < 0 || (tIdx & 1) == 0 || tIdx > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE pointer for cutout #";
            cerr << (iPTI.size() + 1) << " (" << tIdx << ")\n";
            pdout.clear();
            return false;
        }

        iPTI.push_back( tIdx );
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
    return true;
}


bool IGES_ENTITY_144::SetEntityForm( int aForm )
{
    if( aForm == 0 )
        return true;

    ERRMSG << "\n + [BUG] Trimmed Parametric Surface only supports Form 0 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_144::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase == 0 )
        return true;

    ERRMSG << "\n + [BUG] Trimmed Parametric Surface only supports Use 0 (GEOMETRY) (requested use: ";
    cerr << aUseCase << ")\n";
    return false;
}


bool IGES_ENTITY_144::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] entity does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_144::GetPTS( IGES_ENTITY*& aPtr )
{
    aPtr = PTS;

    if( NULL == PTS )
        return false;

    return true;
}


bool IGES_ENTITY_144::SetPTS( IGES_ENTITY* aPtr )
{
    if( PTS )
        PTS->delReference(this);

    PTS = aPtr;

    if( NULL == aPtr )
        return true;

    if( !checkInclusion144( aPtr->GetEntityType()) )
    {
        ERRMSG << "\n + [INFO] invalid entity type (" << PTS->GetEntityType() << ") for PTS\n";
        PTS = NULL;
        return false;
    }

    bool dup = false;

    if( !PTS->addReference(this, dup) )
    {
        PTS = NULL;
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        PTS = NULL;
        return false;
    }

    PTS->SetDependency( STAT_DEP_PHY );

    if( NULL == PTO )
        N1 = 0;
    else
        N1 = 1;

    if( NULL != parent && parent != PTS->GetParentIGES() )
        parent->AddEntity( PTS );

    return true;
}


bool IGES_ENTITY_144::GetPTO( IGES_ENTITY_142*& aPtr )
{
    aPtr = PTO;

    if( NULL == PTO && 0 != N1 )
        return false;

    return true;
}


bool IGES_ENTITY_144::SetPTO( IGES_ENTITY_142* aPtr )
{
    if( PTO )
        PTO->delReference(this);

    PTO = aPtr;
    N1 = 0;

    if( NULL == aPtr )
        return true;

    bool dup = false;

    if( !PTO->addReference(this, dup) )
    {
        PTO = NULL;
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        PTO = NULL;
        return false;
    }

    PTO->SetDependency( STAT_DEP_PHY );
    N1 = 1;

    if( NULL != parent && parent != PTO->GetParentIGES() )
        parent->AddEntity( PTO );

    return true;
}


int IGES_ENTITY_144::GetNPTI( void )
{
    return (int)PTI.size();
}


bool IGES_ENTITY_144::GetPTIList( size_t& aListSize, IGES_ENTITY_142**& aPTIList )
{
    if( PTI.empty() )
    {
        aListSize = 0;
        aPTIList = NULL;
        return false;
    }

    if( PTI.size() != vPTI.size() )
    {
        vPTI.clear();
        std::list<IGES_ENTITY_142*>::iterator sL = PTI.begin();
        std::list<IGES_ENTITY_142*>::iterator eL = PTI.end();

        while( sL != eL )
        {
            vPTI.push_back( *sL );
            ++sL;
        }
    }

    aListSize = vPTI.size();
    aPTIList = &vPTI[0];
    return true;
}


IGES_ENTITY_142* IGES_ENTITY_144::GetPTI( int aIndex )
{
    int ne = (int)PTI.size();

    if( aIndex < 0 || aIndex >= ne )
        return NULL;

    std::list< IGES_ENTITY_142* >::iterator sI = PTI.begin();

    for( int i = 0; i < aIndex; ++i )
        ++sI;

    return *sI;
}


bool IGES_ENTITY_144::AddPTI( IGES_ENTITY_142* aPtr )
{
    if( NULL == aPtr )
    {
        ERRMSG << "\n + [INFO] [BUG] NULL pointer passed\n";
        return false;
    }

    std::list<IGES_ENTITY_142*>::iterator bref = PTI.begin();
    std::list<IGES_ENTITY_142*>::iterator eref = PTI.end();

    while( bref != eref )
    {
        // while this is a bug, we can do the right thing and simply ignore the
        // additional reference
        if( aPtr == *bref )
        {
            vPTI.clear();
            return true;
        }

        ++bref;
    }

    bool dup = false;

    if( !aPtr->addReference(this, dup) )
    {
        ERRMSG << "\n + [INFO] [BUG] could not add child reference\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        return false;
    }

    aPtr->SetDependency( STAT_DEP_PHY );

    PTI.push_back( aPtr );
    N2 = (int)PTI.size();

    vPTI.clear();

    if( NULL != parent && parent != aPtr->GetParentIGES() )
        parent->AddEntity( aPtr );

    return true;
}


bool IGES_ENTITY_144::DelPTI( IGES_ENTITY_142* aPtr )
{
    std::list<IGES_ENTITY_142*>::iterator bref = PTI.begin();
    std::list<IGES_ENTITY_142*>::iterator eref = PTI.end();

    while( bref != eref )
    {
        if( aPtr == *bref )
        {
            PTI.erase( bref );
            N2 = (int)PTI.size();
            return true;
        }

        ++bref;
    }

    return false;
}
