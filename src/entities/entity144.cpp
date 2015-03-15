/*
 * file: entity144.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 144: Trimmed Parametric Surface, Section 4.34, p.181 (209+)
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

#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity144.h>

using namespace std;


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
        PTS->DelReference( this );

    if( PTO )
        PTO->DelReference( this );

    std::list<IGES_ENTITY*>::iterator sPTI = PTI.begin();
    std::list<IGES_ENTITY*>::iterator ePTI = PTI.end();

    while( sPTI != ePTI )
    {
        (*sPTI)->DelReference( this );
        ++sPTI;
    }

    return;
}


bool IGES_ENTITY_144::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not register associations\n";
        iPTI.clear();
        return false;
    }

    int iEnt;

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

        if( !PTS->AddReference( this ) )
        {
            PTS = NULL;
            ERRMSG << "\n + [INFO] could not associate surface entity with DE " << PTS << "\n";
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

        PTO = (*entities)[iEnt];

        if( !PTO->AddReference( this ) )
        {
            PTO = NULL;
            ERRMSG << "\n + [INFO] could not associate outline entity with DE " << iPTO << "\n";
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
    IGES_ENTITY* ep;
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

        ep = (*entities)[iEnt];

        if( !ep->AddReference( this ) )
        {
            ERRMSG << "\n + [INFO] could not associate cutout entity with DE " << iDE << "\n";
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

    if( NULL == PTO )
    {
        ERRMSG << "\n + [BUG] unspecified outline entity\n";
        return false;
    }

    if( N1 < 0 || N1 >  1 )
    {
        ERRMSG << "\n + [BUG] invalid value for N1 (" << N1 << ")\n";
        return false;
    }

    N2 = PTI.size();

    iPTS = PTS->GetDESequence();
    iPTO = PTO->GetDESequence();

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
        std::list<IGES_ENTITY*>::iterator sPTI = PTI.begin();
        std::list<IGES_ENTITY*>::iterator ePTI = PTI.end();

        while( sPTI != ePTI )
        {
            seqPTI = (*sPTI)->GetDESequence();

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


bool IGES_ENTITY_144::Unlink( IGES_ENTITY* aChild )
{
    if( IGES_ENTITY::Unlink( aChild ) )
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
        std::list<IGES_ENTITY*>::iterator sPTI = PTI.begin();
        std::list<IGES_ENTITY*>::iterator ePTI = PTI.end();

        while( sPTI != ePTI )
        {
            if( aChild == *sPTI )
            {
                sPTI = PTI.erase( sPTI );
                return true;
            }

            ++sPTI;
        }
    }

    return false;
}


bool IGES_ENTITY_144::IsOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT)
        || ( NULL == PTS ) || ( NULL == PTO ) )
        return true;

    return false;
}


bool IGES_ENTITY_144::AddReference( IGES_ENTITY* aParentEntity )
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
        std::list<IGES_ENTITY*>::iterator sPTI = PTI.begin();
        std::list<IGES_ENTITY*>::iterator ePTI = PTI.end();

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

    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_144::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_144::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
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


bool IGES_ENTITY_144::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Surface of Revolution\n";
        return false;
    }

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    idx = pdout.find( pd );

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        return false;
    }

    ++idx;

    if( !ParseInt( pdout, idx, iPTS, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read surface entity DE \n";
        return false;
    }

    if( iPTS < 0 || iPTS > 9999997 || (iPTS & 1) == 0 )
    {
        ERRMSG << "\n + [INFO] invalid surface entity DE (" << iPTS << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, N1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read N1 parameter\n";
        return false;
    }

    if( N1 < 0 || N1 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for N1 (" << N1 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, N2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read N2 parameter\n";
        return false;
    }

    if( N2 < 0 )
    {
        ERRMSG << "\n + [INFO] invalid N2 parameter (" << N2 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, iPTO, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read outline DE pointer\n";
        return false;
    }

    if( iPTO < 0 || (iPTO & 1) == 0 || iPTO > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid outline DE pointer (" << iPTO << ")\n";
        return false;
    }

    int tIdx;

    for( int i = 0; i < N2; ++ i )
    {
        if( !ParseInt( pdout, idx, tIdx, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read cutout #" << (iPTI.size() + 1) << "\n";
            return false;
        }

        if( tIdx < 0 || (tIdx & 1) == 0 || tIdx > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE pointer for cutout #";
            cerr << (iPTI.size() + 1) << " (" << tIdx << ")\n";
            return false;
        }

        iPTI.push_back( tIdx );
    }

    if( !eor && !readExtraParams( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read optional pointers\n";
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
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


bool IGES_ENTITY_144::GetPTS( IGES_ENTITY** aPtr )
{
    *aPtr = PTS;

    if( NULL == PTS )
        return false;

    return true;
}


bool IGES_ENTITY_144::SetPTS( IGES_ENTITY* aPtr )
{
    if( PTS )
        PTS->DelReference( this );

    PTS = aPtr;

    if( NULL == aPtr )
        return true;

    if( !PTS->AddReference( this ) )
    {
        PTS = NULL;
        return false;
    }

    return true;
}


bool IGES_ENTITY_144::GetPTO( IGES_ENTITY** aPtr )
{
    *aPtr = PTO;

    if( NULL == PTO )
        return false;

    return true;
}


bool IGES_ENTITY_144::SetPTO( IGES_ENTITY* aPtr )
{
    if( PTO )
        PTO->DelReference( this );

    PTO = aPtr;

    if( NULL == aPtr )
        return true;

    if( !PTO->AddReference( this ) )
    {
        PTO = NULL;
        return false;
    }

    return true;
}


bool IGES_ENTITY_144::GetPTIList( std::list<IGES_ENTITY*>& aList )
{
    aList = PTI;
    return true;
}


bool IGES_ENTITY_144::AddPTI( IGES_ENTITY* aPtr )
{
    if( NULL == aPtr )
    {
        ERRMSG << "\n + [INFO] [BUG] NULL pointer passed\n";
        return false;
    }

    // ensure we have no references to this object
    if( aPtr == pStructure || aPtr == pLineFontPattern
        || aPtr == pLevel || aPtr == pView
        || aPtr == pTransform || aPtr == pLabelAssoc
        || aPtr == pColor )
    {
        ERRMSG << "\n + [BUG] invalid reference requested for PTI list\n";
        return false;
    }

    std::list<IGES_ENTITY*>::iterator bref = refs.begin();
    std::list<IGES_ENTITY*>::iterator eref = refs.end();

    while( bref != eref )
    {
        if( aPtr == *bref )
        {
            ERRMSG << "\n + [BUG] circular reference requested for PTI list\n";
            return false;
        }

        ++bref;
    }

    // check if the entity is a child in extras<>
    bref = extras.begin();
    eref = extras.end();

    while( bref != eref )
    {
        if( aPtr == *bref )
        {
            ERRMSG << "\n + [BUG] invalid reference requested for PTI list\n";
            return false;
        }

        ++bref;
    }

    bref = PTI.begin();
    eref = PTI.end();

    while( bref != eref )
    {
        // while this is a bug, we can di the right thing and simply ignore the
        // additional reference
        if( aPtr == *bref )
            return true;

        ++bref;
    }

    if( !aPtr->AddReference( this ) )
    {
        ERRMSG << "\n + [INFO] [BUG] could not add child reference\n";
        return false;
    }

    PTI.push_back( aPtr );

    return true;
}


bool IGES_ENTITY_144::DelPTI( IGES_ENTITY* aPtr )
{
    std::list<IGES_ENTITY*>::iterator bref = PTI.begin();
    std::list<IGES_ENTITY*>::iterator eref = PTI.end();

    while( bref != eref )
    {
        if( aPtr == *bref )
        {
            PTI.erase( bref );
            return true;
        }

        ++bref;
    }

    return false;
}
