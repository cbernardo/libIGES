/*
 * file: entity408.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 4-8: Subfigure Instance, Section 4.137, p.557(585+)
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
#include <mcad_helpers.h>
#include <entity124.h>
#include <entity308.h>
#include <entity408.h>

using namespace std;


IGES_ENTITY_408::IGES_ENTITY_408( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 408;
    form = 0;

    DE = NULL;
    X = 0.0;
    Y = 0.0;
    Z = 0.0;
    S = 1.0;

    return;
}


IGES_ENTITY_408::~IGES_ENTITY_408()
{
    if( DE )
        DE->DelReference( this );

    return;
}


bool IGES_ENTITY_408::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
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

    bool dup = false;

    if( iDE )
    {
        if( (iDE & 1) == 0 || iDE < 0 || iDE > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE sequence for Subfigure Definition (" << iDE << ")\n";
            return false;
        }

        int iEnt = iDE >> 1;

        if( iEnt >= (int)entities->size() )
        {
            ERRMSG << "\n + [INFO] DE sequence out of bounds for directrix curve (" << iDE << ")\n";
            return false;
        }

        DE = dynamic_cast<IGES_ENTITY_308*>((*entities)[iEnt]);

        if( NULL == DE )
        {
            ERRMSG << "\n + [CORRUPT FILE] DE sequence is not a Subfigure Definition (" << iDE << ")\n";
            return false;
        }

        if( !DE->AddReference( this, dup ) )
        {
            DE = NULL;
            ERRMSG << "\n + [INFO] could not add reference to Subfigure Definition (" << iDE << ")\n";
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            return false;
        }

    }

    return true;
}


bool IGES_ENTITY_408::format( int &index )
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
        ERRMSG << "\n + [INFO] unassigned Subfigure Definition\n";
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

    double* pt[4] = { &X, &Y, &Z, &S };

    for( int i = 0; i < 4; ++i )
    {
        if( i == 3 && extras.empty() )
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


bool IGES_ENTITY_408::rescale( double sf )
{
    X *= sf;
    Y *= sf;
    Z *= sf;

    return true;
}


bool IGES_ENTITY_408::Unlink( IGES_ENTITY* aChildEntity )
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


bool IGES_ENTITY_408::IsOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT) || NULL == DE )
        return true;

    return false;
}


bool IGES_ENTITY_408::AddReference( IGES_ENTITY* aParentEntity,
                                                     bool& isDuplicate )
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

    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_408::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_408::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Subfigure Instance\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_408::ReadPD( std::ifstream& aFile, int& aSequenceVar )
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

    if( !ParseReal( pdout, idx, X, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no LX value for Tabulated Cylinder\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, Y, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no LY value for Tabulated Cylinder\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, Z, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no LZ value for Tabulated Cylinder\n";
        pdout.clear();
        return false;
    }

    double rdef = 1.0;

    if( eor )
    {
        S = 1.0;
    }
    else
    {
        if( !ParseReal( pdout, idx, S, eor, pd, rd, &rdef ) )
        {
            ERRMSG << "\n + [BAD FILE] no value for S\n";
            pdout.clear();
            return false;
        }
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


bool IGES_ENTITY_408::SetEntityForm( int aForm )
{
    if( aForm != 0 )
    {
        ERRMSG << "\n + [ERROR] entity only supports Form 0\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_408::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    hierarchy = aHierarchy;
    return true;
}


bool IGES_ENTITY_408::GetDE( IGES_ENTITY_308** aPtr )
{
    *aPtr = DE;

    if( !DE )
        return false;

    return true;
}


bool IGES_ENTITY_408::SetDE( IGES_ENTITY_308* aPtr )
{
    if( DE )
        DE->DelReference( this );

    DE = aPtr;

    if( NULL == aPtr )
        return true;

    int eT = aPtr->GetEntityType();

    if( eT != ENT_SUBFIGURE_DEFINITION )
    {
        DE = NULL;
        ERRMSG << "\n + [ERROR] invalid entity type (";
        cerr << eT << "); only type 308 is allowed\n";
        return false;
    }

    bool dup = false;

    if( !DE->AddReference( this, dup ) )
    {
        DE = NULL;
        ERRMSG << "\n + [INFO] could not add child entity reference\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        DE = NULL;
        return false;
    }

    return true;
}


int IGES_ENTITY_408::GetDepthLevel( void )
{
    // note: the specification is not clear about whether Type IGES_ENTITY_308
    // (Subfigure Definition) may indirectly reference a Type 308 of the same
    // DEPTH via inclusion within Type 408. To be absolutely safe, it is best
    // to implement a GetDepthLevel() in Type 408 to ensure that processors
    // which expect strict ordering of indirect references will be able to
    // process the files which we create.

    if( DE )
        return DE->GetDepthLevel();

    return 0;
}
