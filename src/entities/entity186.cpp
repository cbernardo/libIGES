/*
 * file: entity186.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 186: Manifold Solid B-REP Object (MSBO)
 *              Section 4.49, p.216+ (244+)
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

#include <error_macros.h>
#include <sstream>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity502.h>
#include <entity504.h>
#include <entity508.h>
#include <entity510.h>
#include <entity514.h>
#include <entity186.h>

using namespace std;


IGES_ENTITY_186::IGES_ENTITY_186( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 186;
    form = 0;

    visible = true;
    mDEshell = 0;
    mshell = NULL;
    mSOF = false;

    return;
}


IGES_ENTITY_186::~IGES_ENTITY_186()
{
    if( mshell )
        mshell->delReference(this);

    list<pair<IGES_ENTITY_514*, bool> >::iterator sV = mvoids.begin();
    list<pair<IGES_ENTITY_514*, bool> >::iterator eV = mvoids.end();

    while( sV != eV )
    {
        sV->first->delReference(this);
        ++sV;
    }

    mvoids.clear();
    return;
}


void IGES_ENTITY_186::Compact( void )
{
    return;
}


bool IGES_ENTITY_186::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        ivoids.clear();
        return false;
    }

    if( 0 >= mDEshell )
    {
        ERRMSG << "\n + [INFO] invalid outer shell DE\n";
        ivoids.clear();
        return false;
    }

    int iEnt = mDEshell >> 1;

    if( iEnt >= (int)entities->size() )
    {
        ERRMSG << "\n + [INFO] invalid DE (" << mDEshell;
        cerr << "), list size is " << entities->size() << "\n";
        ivoids.clear();
        return false;
    }

    if( ENT_SHELL != (*entities)[iEnt]->GetEntityType() )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid entity for outer shell (Type: ";
        cerr << (*entities)[iEnt]->GetEntityType() << ")\n";
        ivoids.clear();
        return false;
    }

    if( 1 != (*entities)[iEnt]->GetEntityForm() )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid entity form for outer shell (Form: ";
        cerr << (*entities)[iEnt]->GetEntityForm() << ")\n";
        ivoids.clear();
        return false;
    }

    mshell = (IGES_ENTITY_514*)((*entities)[iEnt]);
    bool dup = false;

    if( !mshell->addReference(this, dup) )
    {
        ERRMSG << "\n + [INFO] could not add reference to outer shell entity\n";
        mshell = NULL;
        ivoids.clear();
        return false;
    }

    list<pair<int, bool> >::iterator sV = ivoids.begin();
    list<pair<int, bool> >::iterator eV = ivoids.end();
    IGES_ENTITY* ep;

    while( sV != eV )
    {
        iEnt = sV->first >> 1;

        if( iEnt >= (int)entities->size() )
        {
            ERRMSG << "\n + [INFO] invalid DE for void (" << sV->first;
            cerr << "), list size is " << entities->size() << "\n";
            ivoids.clear();
            return false;
        }

        ep = (*entities)[iEnt];

        if( ENT_SHELL != ep->GetEntityType() )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid entity for void shell (Type: ";
            cerr << ep->GetEntityType() << ")\n";
            ivoids.clear();
            return false;
        }

        if( 1 != ep->GetEntityForm() )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid entity form for void shell (Form: ";
            cerr << ep->GetEntityForm() << ")\n";
            ivoids.clear();
            return false;
        }

        if( !ep->addReference(this, dup) )
        {
            ERRMSG << "\n + [INFO] could not add reference void shell\n";
            ivoids.clear();
            return false;
        }

        mvoids.push_back( pair<IGES_ENTITY_514*, bool>( (IGES_ENTITY_514*)ep, sV->second ) );
        ++sV;
    }

    ivoids.clear();
    return true;
}


bool IGES_ENTITY_186::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    if( index < 1 || index > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( !mshell )
    {
        ERRMSG << "\n + [INFO] no valid shell\n";
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
    ostr << mshell->getDESequence() << pd;

    if( mSOF )
        ostr << "1" << pd;
    else
        ostr << "0" << pd;

    string fStr = ostr.str();
    string tStr;

    if( mvoids.empty() )
    {
        ostr.str("");

        if( extras.empty() )
            ostr << "0" << rd;
        else
            ostr << "0" << pd;

        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
    }

    list<pair<IGES_ENTITY_514*, bool> >::iterator sV = mvoids.begin();
    list<pair<IGES_ENTITY_514*, bool> >::iterator eV = mvoids.end();
    list<pair<IGES_ENTITY_514*, bool> >::iterator iV = --(mvoids.end());

    while( sV != eV )
    {
        ostr.str("");
        ostr << sV->first->getDESequence() << pd;
        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

        ostr.str("");

        if( sV->second )
            ostr << "1";
        else
            ostr << "0";

        if( sV == iV && extras.empty() )
            ostr << rd;
        else
            ostr << pd;

        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
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


bool IGES_ENTITY_186::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_186::unlink(IGES_ENTITY *aChildEntity)
{
    if(IGES_ENTITY::unlink(aChildEntity) )
        return true;

    if( aChildEntity == mshell )
    {
        mshell = NULL;
        return true;
    }


    list<pair<IGES_ENTITY_514*, bool> >::iterator sV = mvoids.begin();
    list<pair<IGES_ENTITY_514*, bool> >::iterator eV = mvoids.end();

    while( sV != eV )
    {
        if( aChildEntity == sV->first )
        {
            mvoids.erase( sV );
            return true;
        }

        ++sV;
    }

    return false;
}


bool IGES_ENTITY_186::isOrphaned( void )
{
    if( NULL == mshell )
        return true;

    return false;
}


bool IGES_ENTITY_186::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed to method\n";
        return false;
    }

    // NOTE: TO BE IMPLEMENTED:
    // Checks for circular references have been omitted

    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_186::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_186::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.

    if( 0 != form )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in MSBO\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_186::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for MSBO Entity\n";
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

    // DE to the outer shell entity
    if( !ParseInt( pdout, idx, mDEshell, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the shell entity DE\n";
        pdout.clear();
        return false;
    }

    if( mDEshell < 1 || mDEshell > 9999997 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid DE to shell entity (" << mDEshell<< ")\n";
        pdout.clear();
        return false;
    }

    int tmpI;

    if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the shell SOF\n";
        pdout.clear();
        return false;
    }

    if( tmpI < 0 || tmpI > 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] bad SOF value for outer shell (" << tmpI << ")\n";
        pdout.clear();
        return false;
    }

    if( 1 == tmpI )
        mSOF = true;
    else
        mSOF = false;

    int nS; // number of shells in the MSBO

    if( !ParseInt( pdout, idx, nS, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of void shells\n";
        pdout.clear();
        return false;
    }

    if( nS < 0 )
    {
        ERRMSG << "\n + [INFO] invalid number of void shells: " << nS << "\n";
        pdout.clear();
        return false;
    }

    int tmpJ;

    for( int i = 0; i < nS; ++i )
    {
        if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read void DE\n";
            ivoids.clear();
            pdout.clear();
            return false;
        }

        if( tmpI < 1 || tmpI > 9999997 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid DE to void (" << tmpI << ")\n";
            ivoids.clear();
            pdout.clear();
            return false;
        }

        if( !ParseInt( pdout, idx, tmpJ, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the VOF\n";
            ivoids.clear();
            pdout.clear();
            return false;
        }

        if( tmpJ < 0 || tmpJ > 1 )
        {
            ERRMSG << "\n + [CORRUPT FILE] bad VOF value (" << tmpJ << ")\n";
            ivoids.clear();
            pdout.clear();
            return false;
        }

        if( 1 == tmpJ )
            ivoids.push_back( pair<int, bool>( tmpI, true ) );
        else
            ivoids.push_back( pair<int, bool>( tmpI, false ) );

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

    // note: this entity never performs scaling

    return true;
}


bool IGES_ENTITY_186::SetEntityForm( int aForm )
{
    if( 0 == aForm )
        return true;

    ERRMSG << "\n + [BUG] MSBO Entity only supports Form 0 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}
