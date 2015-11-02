/*
 * file: entity514.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 514: Shell, Section 4.151, p.595+ (623+)
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

using namespace std;


IGES_ENTITY_514::IGES_ENTITY_514( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 514;
    visible = true;

    return;
}


IGES_ENTITY_514::~IGES_ENTITY_514()
{
    vector< pair< IGES_ENTITY_510*, bool > >::iterator sF = mfaces.begin();
    vector< pair< IGES_ENTITY_510*, bool > >::iterator eF = mfaces.end();

    while( sF != eF )
    {
        sF->first->delReference( this );
        ++sF;
    }

    mfaces.clear();
    return;
}


bool IGES_ENTITY_514::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    if( ifaces.empty() )
    {
        ERRMSG << "\n + [INFO] invalid shell; no faces\n";
        ifaces.clear();
        return false;
    }

    list<pair<int, bool> >::iterator sF = ifaces.begin();
    list<pair<int, bool> >::iterator eF = ifaces.end();
    int iEnt;
    int eType;
    bool dup = false;
    IGES_ENTITY* ep;

    while( sF != eF )
    {
        iEnt = sF->first >> 1;

        if( iEnt >= (int)entities->size() )
        {
            ERRMSG << "\n + [INFO] invalid DE (" << sF->first;
            cerr << "), list size is " << entities->size() << "\n";
            ifaces.clear();
            return false;
        }

        ep = (*entities)[iEnt];
        eType = ep->GetEntityType();

        if( eType != ENT_FACE )
        {
            ERRMSG << "\n + [INFO] invalid DE (" << sF->first;
            cerr << "), entity is not a face\n";
            ifaces.clear();
            return false;
        }

        if( !ep->addReference(this, dup) )
        {
            ERRMSG << "\n + [INFO] could not add reference to face\n";
            ifaces.clear();
            return false;
        }

        mfaces.push_back( pair<IGES_ENTITY_510*, bool>( (IGES_ENTITY_510*)ep, sF->second ) );
        ++sF;
    }

    ifaces.clear();
    return true;
}


bool IGES_ENTITY_514::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    if( index < 1 || index > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( mfaces.empty() )
    {
        ERRMSG << "\n + [INFO] no valid faces\n";
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
    ostr << mfaces.size() << pd;
    string fStr = ostr.str();
    string tStr;

    vector< pair< IGES_ENTITY_510*, bool > >::iterator sF = mfaces.begin();
    vector< pair< IGES_ENTITY_510*, bool > >::iterator eF = --(mfaces.end());

    while( sF != eF )
    {
        ostr.str("");
        ostr << sF->first->getDESequence() << pd;
        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
        ostr.str("");

        if( sF->second )
            ostr << "1" << pd;
        else
            ostr << "0" << pd;

        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
        ++sF;
    }

    ostr.str("");
    ostr << sF->first->getDESequence() << pd;
    tStr = ostr.str();
    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
    ostr.str("");

    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    ostr.str("");

    if( sF->second )
        ostr << "1" << idelim;
    else
        ostr << "0" << idelim;

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


bool IGES_ENTITY_514::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_514::unlink(IGES_ENTITY *aChildEntity)
{
    if(IGES_ENTITY::unlink(aChildEntity) )
        return true;

    vector< pair< IGES_ENTITY_510*, bool > >::iterator sF = mfaces.begin();
    vector< pair< IGES_ENTITY_510*, bool > >::iterator eF = mfaces.end();

    while( sF != eF )
    {
        if( aChildEntity == sF->first )
        {
            mfaces.erase( sF );
            return true;
        }

        ++sF;
    }

    return false;
}


bool IGES_ENTITY_514::isOrphaned( void )
{
    if( (refs.empty() && depends) || mfaces.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_514::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
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


bool IGES_ENTITY_514::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_514::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    lineWeightNum = 0;              // N.A.
    colorNum = 0;                   // N.A.

    if( form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Shell\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_514::readPD(std::ifstream &aFile, int &aSequenceVar)
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

    int nF; // number faces in the shell

    if( !ParseInt( pdout, idx, nF, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of faces\n";
        pdout.clear();
        return false;
    }

    if( nF < 1 )
    {
        ERRMSG << "\n + [INFO] invalid number of faces: " << nF << "\n";
        pdout.clear();
        return false;
    }

    int tmpI;
    int iEnt;

    // read in DEs and OFs for faces
    for( int i = 0; i < nF; ++i )
    {
        if( !ParseInt( pdout, idx, iEnt, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read face DE\n";
            ifaces.clear();
            pdout.clear();
            return false;
        }

        if( iEnt < 1 || iEnt > 9999997 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid DE to face (" << iEnt << ")\n";
            ifaces.clear();
            pdout.clear();
            return false;
        }

        if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read face DE\n";
            ifaces.clear();
            pdout.clear();
            return false;
        }

        if( tmpI < 0 || tmpI > 1 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid OF to loop (" << tmpI << ")\n";
            ifaces.clear();
            pdout.clear();
            return false;
        }

        if( tmpI )
            ifaces.push_back( pair<int, bool>( iEnt, true ) );
        else
            ifaces.push_back( pair<int, bool>( iEnt, false ) );

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


bool IGES_ENTITY_514::SetEntityForm( int aForm )
{
    if( 1 == aForm || 2 == aForm )
    {
        form = aForm;
        return true;
    }

    ERRMSG << "\n + [BUG] Shell Entity only supports Forms 1 and 2 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_514::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Shell Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_514::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_514::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}

// XXX - MORE TO BE ADDED
//WARNING: TO BE IMPLEMENTED
