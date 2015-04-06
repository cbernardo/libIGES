/*
 * file: entity122.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 122: Tabulated Cylinder, Section 4.19, p.119(147+)
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
#include <cmath>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <iges_helpers.h>
#include <entity124.h>
#include <entity122.h>

using namespace std;


IGES_ENTITY_122::IGES_ENTITY_122( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 122;
    form = 0;

    DE = NULL;
    iDE = 0;
    LX = 0.0;
    LY = 0.0;
    LZ = 0.0;

    return;
}


IGES_ENTITY_122::~IGES_ENTITY_122()
{
    if( DE )
        DE->DelReference( this );

    return;
}


bool IGES_ENTITY_122::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    structure = 0;

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->DelReference( this );
        pStructure = NULL;
    }

    if( iDE )
    {
        if( (iDE & 1) == 0 || iDE < 0 || iDE > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE sequence for directrix curve (" << iDE << ")\n";
            return false;
        }

        int iEnt = iDE >> 1;

        if( iEnt >= (int)entities->size() )
        {
            ERRMSG << "\n + [INFO] DE sequence out of bounds for directrix curve (" << iDE << ")\n";
            return false;
        }

        DE = dynamic_cast<IGES_CURVE*>((*entities)[iEnt]);

        if( NULL == DE )
        {
            ERRMSG << "\n + [CORRUPT FILE] DE sequence is not a curve entity (" << iDE << ")\n";
            return false;
        }

        if( !DE->AddReference( this ) )
        {
            DE = NULL;
            ERRMSG << "\n + [INFO] could not add reference to directrix curve entity (" << iDE << ")\n";
            return false;
        }
    }

    return true;
}


bool IGES_ENTITY_122::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( NULL == DE )
    {
        ERRMSG << "\n + [INFO] unassigned directrix\n";
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
    double uir = parent->globalData.minResolution;

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << DE->GetDESequence() << pd;
    string lstr = ostr.str();
    string tstr;

    double* pt[3] = { &LX, &LY,&LZ };

    for( int i = 0; i < 3; ++i )
    {
        if( i == 2 && extras.empty() )
        {
            if( !FormatPDREal( tstr, *pt[i], rd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format datum [";
                cerr << i << "]\n";
                return false;
            }
        }
        else
        {
            if( !FormatPDREal( tstr, *pt[i], pd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format datum [";
                cerr << i << "]\n";
                return false;
            }
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

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


bool IGES_ENTITY_122::rescale( double sf )
{
    LX *= sf;
    LY *= sf;
    LZ *= sf;

    return true;
}


bool IGES_ENTITY_122::Unlink( IGES_ENTITY* aChildEntity )
{
    if( !aChildEntity )
    {
        ERRMSG << "\n + [BUG] method invoked with NULL pointer\n";
        return false;
    }

    if( IGES_ENTITY::Unlink( aChildEntity ) )
        return true;

    if( aChildEntity == DE )
    {
        DE = NULL;
        return true;
    }

    return false;
}


bool IGES_ENTITY_122::IsOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT) || NULL == DE )
        return true;

    return false;
}


bool IGES_ENTITY_122::IGES_ENTITY_122::AddReference( IGES_ENTITY* aParentEntity )
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL parameter passed\n";
        return false;
    }

    if( aParentEntity == DE )
    {
        ERRMSG << "\n + [INFO] requesting circular reference\n";
        return false;
    }

    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_122::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_122::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Tabulated Cylinder\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_122::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Circle Entity\n";
        pdout.clear();
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
        pdout.clear();
        return false;
    }

    ++idx;

    if( !ParseInt( pdout, idx, iDE, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no pointer to directrix DE\n";
        pdout.clear();
        return false;
    }

    if( iDE < 0 || (iDE & 1) == 0 || iDE > 9999997 )
    {
        ERRMSG << "\n + [BAD FILE] invalid value for directrix DE (" << iDE << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, LX, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no LX value for Tabulated Cylinder\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, LY, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no LY value for Tabulated Cylinder\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, LZ, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no LZ value for Tabulated Cylinder\n";
        pdout.clear();
        return false;
    }

    if( parent->globalData.convert )
        rescale( parent->globalData.cf );

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


bool IGES_ENTITY_122::SetEntityForm( int aForm )
{
    if( aForm != 0 )
    {
        ERRMSG << "\n + [ERROR] entity only supports Form 0\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_122::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored by a Right Circular Cylinder so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] entity does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_122::GetDE( IGES_CURVE** aPtr )
{
    *aPtr = DE;

    if( DE )
        return true;

    return false;
}


bool IGES_ENTITY_122::SetDE( IGES_CURVE* aPtr )
{
    if( DE )
    {
        DE->DelReference( this );
        DE = NULL;
    }

    if( !aPtr->AddReference( this ) )
        return false;

    DE = aPtr;
    DE->SetDependency( STAT_DEP_PHY );

    return true;
}
