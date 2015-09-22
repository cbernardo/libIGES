/*
 * file: entity142.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 142: Curve on a Parametric Surface, Section 4.32, p.178 (206+)
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
#include <entity142.h>

using namespace std;


IGES_ENTITY_142::IGES_ENTITY_142( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 142;
    form = 0;

    CRTN = 0;
    iSPTR = 0;
    iBPTR = 0;
    iCPTR = 0;
    PREF = 0;

    SPTR = NULL;
    BPTR = NULL;
    CPTR = NULL;

    return;
}


IGES_ENTITY_142::~IGES_ENTITY_142()
{
    if( SPTR )
        SPTR->delReference(this);

    if( BPTR )
        BPTR->delReference(this);

    if( CPTR )
        CPTR->delReference(this);

    return;
}


void IGES_ENTITY_142::Compact( void )
{
    return;
}


bool IGES_ENTITY_142::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] could not register associations\n";
        return false;
    }

    int iEnt;
    bool dup = false;

    if( iSPTR )
    {
        iEnt = iSPTR >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid surface entity (DE:" << iSPTR << ")\n";
            return false;
        }

        SPTR = (*entities)[iEnt];

        if( !SPTR->addReference(this, dup) )
        {
            SPTR = NULL;
            ERRMSG << "\n + [INFO] could not associate surface entity with DE " << iSPTR << "\n";
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            SPTR = NULL;
            return false;
        }

    }
    else
    {
        ERRMSG << "\n + [VIOLATION] unspecified surface entity\n";
        return false;
    }

    if( iBPTR )
    {
        iEnt = iBPTR >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid boundary entity (DE:" << iBPTR << ")\n";
            return false;
        }

        BPTR = (*entities)[iEnt];

        if( !BPTR->addReference(this, dup) )
        {
            BPTR = NULL;
            ERRMSG << "\n + [INFO] could not associate boundary entity with DE " << iBPTR << "\n";
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            BPTR = NULL;
            return false;
        }

    }
    else
    {
        BPTR = NULL;
    }

    if( iCPTR )
    {
        iEnt = iCPTR >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid bounding curve entity (DE:" << iCPTR << ")\n";
            return false;
        }

        CPTR = (*entities)[iEnt];

        if( !CPTR->addReference(this, dup) )
        {
            CPTR = NULL;
            ERRMSG << "\n + [INFO] could not associate bounding curve entity with DE " << iCPTR << "\n";
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            CPTR = NULL;
            return false;
        }

    }
    else
    {
        CPTR = NULL;
    }

    return true;
}


bool IGES_ENTITY_142::format( int &index )
{
    pdout.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( NULL == SPTR )
    {
        ERRMSG << "\n + [BUG] unspecified surface entity\n";
        return false;
    }

    if( NULL == BPTR && NULL == CPTR )
    {
        ERRMSG << "\n + [BUG] both BPTR and CPTR are unspecified\n";
        return false;
    }

    if( NULL == BPTR )
    {
        PREF = 2;
        iBPTR = 0;
    }
    else
    {
        iBPTR = BPTR->getDESequence();
    }

    if( NULL == CPTR )
    {
        PREF = 1;
        iCPTR = 0;
    }
    else
    {
        iCPTR = CPTR->getDESequence();
    }

    if( CRTN < 0 || CRTN >  3 )
    {
        ERRMSG << "\n + [BUG] invalid construction method (" << CRTN << ")\n";
        return false;
    }

    if( PREF < 0 || PREF >  3 )
    {
        ERRMSG << "\n + [BUG] invalid curve entity preference (" << PREF << ")\n";
        return false;
    }

    iSPTR = SPTR->getDESequence();

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
    ostr << CRTN << pd;
    ostr << iSPTR << pd;
    ostr << iBPTR << pd;
    ostr << iCPTR << pd;
    string lstr = ostr.str();

    string tstr;
    ostr.str("");

    if( extras.empty() )
        ostr << PREF << rd;
    else
        ostr << PREF << pd;

    tstr = ostr.str();
    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

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


bool IGES_ENTITY_142::rescale( double sf )
{
    // there is nothing to scale so this function always succeeds
    return true;
}


bool IGES_ENTITY_142::unlink(IGES_ENTITY *aChild)
{
    if(IGES_ENTITY::unlink(aChild) )
        return true;

    if( aChild == SPTR )
    {
        SPTR = NULL;
        return true;
    }

    if( aChild == BPTR )
    {
        BPTR = NULL;
        return true;
    }

    if( aChild == CPTR )
    {
        CPTR = NULL;
        return true;
    }

    return false;
}


bool IGES_ENTITY_142::isOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT)
        || ( NULL == SPTR ) || ( NULL == BPTR && NULL == CPTR ) )
        return true;

    return false;
}


bool IGES_ENTITY_142::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for aParentEntity\n";
        return false;
    }

    if( aParentEntity == SPTR || aParentEntity == BPTR || aParentEntity == CPTR )
    {
        ERRMSG << "\n + [BUG] circular reference requested\n";
        return false;
    }

    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_142::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_142::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
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
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Curve on Parametric Surface\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_142::readPD(std::ifstream &aFile, int &aSequenceVar)
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

    idx = (int)pdout.find( pd );

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    if( !ParseInt( pdout, idx, CRTN, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read construction method (CRTN)\n";
        pdout.clear();
        return false;
    }

    if( CRTN < 0 || CRTN > 3 )
    {
        ERRMSG << "\n + [INFO] invalid construction method (CRTN = " << CRTN << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, iSPTR, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read surface DE pointer\n";
        pdout.clear();
        return false;
    }

    if( iSPTR < 0 || (iSPTR & 1) == 0 || iSPTR > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid surface DE pointer (" << iSPTR << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, iBPTR, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read boundary DE pointer\n";
        pdout.clear();
        return false;
    }

    if( iBPTR < 0 || ( iBPTR && (iBPTR & 1) == 0 ) || iBPTR > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid boundary DE pointer (" << iBPTR << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, iCPTR, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read bounding curve DE pointer\n";
        pdout.clear();
        return false;
    }

    if( iCPTR < 0 || ( iCPTR && (iCPTR & 1) == 0 ) || iCPTR > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid bounding curve DE pointer (" << iCPTR << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PREF, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read curve representation preference\n";
        pdout.clear();
        return false;
    }

    if( PREF < 0 || PREF > 3 )
    {
        ERRMSG << "\n + [INFO] invalid curve representation preference (" << PREF << ")\n";
        pdout.clear();
        return false;
    }

    // ensure that if one of iBPTR or iCPTR are NULL, the PREFERENCE is set appropriately
    if( !iBPTR && !iCPTR )
    {
        ERRMSG << "\n + [INFO] both BPTR and CPTR are NULL\n";
        pdout.clear();
        return false;
    }

    if( !iBPTR && PREF != 2 )
    {
        // force the preference
        PREF = 2;
        ERRMSG << "\n + [INFO] BPTR is NULL; forcing PREF to 2\n";
    }

    if( !iCPTR && PREF != 1 )
    {
        // force the preference
        PREF = 1;
        ERRMSG << "\n + [INFO] CPTR is NULL; forcing PREF to 1\n";
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


bool IGES_ENTITY_142::SetEntityForm( int aForm )
{
    if( aForm == 0 )
        return true;

    ERRMSG << "\n + [BUG] Curve on Parametric Surface only supports Form 0 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_142::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase == 0 )
        return true;

    ERRMSG << "\n + [BUG] Curve on Parametric Surface only supports Use 0 (GEOMETRY) (requested use: ";
    cerr << aUseCase << ")\n";
    return false;
}


bool IGES_ENTITY_142::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] entity does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_142::GetSPTR( IGES_ENTITY** aPtr )
{
    *aPtr = SPTR;

    if( NULL == SPTR )
        return false;

    return true;
}


bool IGES_ENTITY_142::SetSPTR( IGES_ENTITY* aPtr )
{
    if( NULL != SPTR )
        SPTR->delReference(this);

    SPTR = aPtr;

    if( NULL == SPTR )
        return true;

    bool dup;

    if( !SPTR->addReference(this, dup) )
    {
        SPTR = NULL;
        ERRMSG << "\n + [INFO] could not add reference to child entity\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        SPTR = NULL;
        return false;
    }

    SPTR->SetDependency( STAT_DEP_PHY );

    return true;
}


bool IGES_ENTITY_142::GetBPTR( IGES_ENTITY** aPtr )
{
    *aPtr = BPTR;

    if( NULL == BPTR )
        return false;

    return true;
}


bool IGES_ENTITY_142::SetBPTR( IGES_ENTITY* aPtr )
{
    if( NULL != BPTR )
        BPTR->delReference(this);

    BPTR = aPtr;

    if( NULL == BPTR )
        return true;

    bool dup = false;

    if( !BPTR->addReference(this, dup) )
    {
        BPTR = NULL;
        ERRMSG << "\n + [INFO] could not add reference to child entity\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        BPTR = NULL;
        return false;
    }

    BPTR->SetDependency( STAT_DEP_PHY );

    return true;
}


bool IGES_ENTITY_142::GetCPTR( IGES_ENTITY** aPtr )
{
    *aPtr = CPTR;

    if( NULL == CPTR )
        return false;

    return true;
}


bool IGES_ENTITY_142::SetCPTR( IGES_ENTITY* aPtr )
{
    if( NULL != CPTR )
        CPTR->delReference(this);

    CPTR = aPtr;

    if( NULL == CPTR )
        return true;

    bool dup = false;

    if( !CPTR->addReference(this, dup) )
    {
        CPTR = NULL;
        ERRMSG << "\n + [INFO] could not add reference to child entity\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        CPTR = NULL;
        return false;
    }

    CPTR->SetDependency( STAT_DEP_PHY );

    return true;
}
