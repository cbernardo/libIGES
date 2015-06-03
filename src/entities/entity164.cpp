/*
 * file: entity164.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 164: Solid of Linear Extrusion, Section 4.44, p.204(232+)
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

#include <cmath>
#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <mcad_helpers.h>
#include <entity164.h>

using namespace std;


IGES_ENTITY_164::IGES_ENTITY_164( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 164;
    form = 0;

    iPtr = 0;
    PTR = NULL;
    L = 0.0;
    I1 = 0.0;
    J1 = 0.0;
    K1 = 1.0;

    return;
}


IGES_ENTITY_164::~IGES_ENTITY_164()
{
    if( PTR &&  !PTR->DelReference( this ) )
    {
        ERRMSG << "\n + [BUG] could not delete reference from a child entity\n";
    }

    return;
}


bool IGES_ENTITY_164::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    structure = 0;

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->DelReference( this );
        pStructure = NULL;
    }

    if( iPtr < 1 )
    {
        ERRMSG << "\n + [INFO] invalid pointer to closed curve\n";
        return false;
    }

    int iEnt = (iPtr >> 1);
    int sEnt = (int)entities->size();

    if( iEnt >= sEnt )
    {
        ERRMSG << "\n + [INFO] invalid pointer (DE: ";
        cerr << iPtr <<" ) does not exist\n";
        return false;
    }

    bool dup = false;

    if( !(*entities)[iEnt]->AddReference( this, dup ) )
    {
        ERRMSG << "\n + [INFO] could not establish reference to child entity\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
        return false;
    }

    PTR = dynamic_cast<IGES_CURVE*>((*entities)[iEnt]);

    if( NULL == PTR )
    {
        ERRMSG << "\n + [INFO] could not establish reference to child entity\n";
        cerr << " + Child Entity Type " << ((*entities)[iEnt])->GetEntityType() << "\n";
        return false;
    }

    // ensure that the curve is a closed curve
    if( !PTR->IsClosed() )
    {
        ERRMSG << "\n + [VIOLATION] Child entity (Type: " << ((*entities)[iEnt])->GetEntityType();
        cerr << ", DE: " << iPtr << ") is not a closed curve\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_164::format( int &index )
{
    pdout.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( !PTR )
    {
        ERRMSG << "\n + [INFO] invalid closed curve entity\n";
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
    string lstr = ostr.str();
    string tstr;

    ostr.str("");
    ostr << PTR->GetDESequence() << pd;
    tstr = ostr.str();

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    double* pt[4] = { &L, &I1, &J1, &K1 };

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


bool IGES_ENTITY_164::rescale( double sf )
{
    L *= sf;
    return true;
}


bool IGES_ENTITY_164::Unlink( IGES_ENTITY* aChildEntity )
{
    if( IGES_ENTITY::Unlink( aChildEntity ) )
        return true;

    if( aChildEntity == PTR )
    {
        PTR = NULL;
        return true;
    }

    return false;
}


bool IGES_ENTITY_164::IsOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT) || !PTR )
        return true;

    return false;
}


bool IGES_ENTITY_164::IGES_ENTITY_164::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] parent entity is NULL\n";
        return false;
    }

    if( aParentEntity == PTR )
    {
        ERRMSG << "\n + [BUG] child entity is it's own grandpa\n";
        return false;
    }

    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_164::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_164::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
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
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Circle\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_164::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for SOlid of Linear Extrusion\n";
        pdout.clear();
        return false;
    }

    PTR = NULL;
    iPtr = 0;

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

    if( !ParseInt( pdout, idx, iPtr, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of segments in the Composite Curve\n";
        pdout.clear();
        return false;
    }

    if( iPtr < 1 )
    {
        ERRMSG << "\n + [INFO] invalid DE sequence for closed curve entity: " << iPtr << "\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, L, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no length value for Solid of LInear Extrusion\n";
        pdout.clear();
        return false;
    }

    double ddef = 0.0;

    if( !eor )
    {
        if( !ParseReal( pdout, idx, I1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no I1 value for Solid of LInear Extrusion\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        I1 = 0.0;
    }

    if( !eor )
    {
        if( !ParseReal( pdout, idx, J1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no J1 value for Solid of LInear Extrusion\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        J1 = 0.0;
    }

    ddef = 1.0;

    if( !eor )
    {
        if( !ParseReal( pdout, idx, K1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no K1 value for Solid of LInear Extrusion\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        K1 = 0.0;
    }

    // ensure (I1,J1,K1) is a unit vector
    if( !CheckNormal( I1, J1, K1 ) )
    {
        ERRMSG << "\n + [BAD FILE] invalid normal vector (cannot be normalized)\n";
        pdout.clear();
        return false;
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


bool IGES_ENTITY_164::SetEntityForm( int aForm )
{
    if( aForm != 0 )
    {
        ERRMSG << "\n + [ERROR] Solid of Linear Extrusion only supports Form 0\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_164::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase != STAT_USE_GEOMETRY )
    {
        ERRMSG << "\n + [ERROR] Solid of Linear Extrusion only supports STAT_USE_GEOMETRY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_164::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored by a Solid of Linear Extrusion so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] Solid of Linear Extrusion does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_164::GetClosedCurve( IGES_CURVE** aCurve )
{
    if( !PTR )
    {
        *aCurve = NULL;
        return false;
    }

    *aCurve = PTR;
    return true;
}

bool IGES_ENTITY_164::SetClosedCurve( IGES_CURVE* aCurve )
{
    if( !aCurve )
    {
        ERRMSG << "\n + [ERROR] NULL passed as curve entity pointer\n";
        return false;
    }

    if( !aCurve->IsClosed() )
    {
        ERRMSG << "\n + [ERROR] closed curve is required; supplied curve is not closed\n";
        return false;
    }

    if( PTR )
    {
        PTR->DelReference( this );
    }

    PTR = aCurve;

    bool dup = false;

    if( !PTR->AddReference( this, dup ) )
    {
        ERRMSG << "\n + [ERROR] could not register association with closed curve\n";
        PTR = NULL; // necessary to prevent segfaults
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        PTR = NULL;
        return false;
    }

    PTR->SetDependency( STAT_DEP_PHY );
    return true;
}
