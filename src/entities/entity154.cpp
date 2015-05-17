/*
 * file: entity154.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 154: Right Circular Cylinder, Section 4.39, p.193(221+)
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
#include <entity154.h>

using namespace std;


IGES_ENTITY_154::IGES_ENTITY_154( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 154;
    form = 0;

    H = 0.0;
    R = 0.0;
    X1 = 0.0;
    Y1 = 0.0;
    Z1 = 0.0;
    I1 = 0.0;
    J1 = 0.0;
    K1 = 1.0;

    return;
}


IGES_ENTITY_154::~IGES_ENTITY_154()
{
    return;
}


bool IGES_ENTITY_154::Associate( std::vector<IGES_ENTITY*>* entities )
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

    return true;
}


bool IGES_ENTITY_154::format( int &index )
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
    double uir = parent->globalData.minResolution;

    ostringstream ostr;
    ostr << entityType << pd;
    string lstr = ostr.str();
    string tstr;

    double* pt[8] = { &H, &R, &X1, &Y1, &Z1, &I1, &J1, &K1 };

    for( int i = 0; i < 8; ++i )
    {
        if( i == 7 && extras.empty() )
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


bool IGES_ENTITY_154::rescale( double sf )
{
    H *= sf;
    R *= sf;
    X1 *= sf;
    Y1 *= sf;
    Z1 *= sf;

    return true;
}


bool IGES_ENTITY_154::Unlink( IGES_ENTITY* aChildEntity )
{
    return IGES_ENTITY::Unlink( aChildEntity );
}


bool IGES_ENTITY_154::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_154::IGES_ENTITY_154::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_154::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_154::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    use = STAT_USE_GEOMETRY;        // fixed value
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Right Circular Cylinder\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_154::ReadPD( std::ifstream& aFile, int& aSequenceVar )
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
    double minRes = parent->globalData.minResolution;

    idx = pdout.find( pd );

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    if( !ParseReal( pdout, idx, H, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no height for Right Circular Cylinder\n";
        pdout.clear();
        return false;
    }

    if( H < minRes )
    {
        if( H > 0.0 )
        {
            ERRMSG << "\n + [BAD FILE] Right Circular Cylinder height < MinRes \n";
            H = minRes;
        }
        else
        {
            ERRMSG << "\n + [BAD FILE] invalid height for Right Circular Cylinder\n";
            pdout.clear();
            return false;
        }
    }

    if( !ParseReal( pdout, idx, R, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no radius for Right Circular Cylinder\n";
        pdout.clear();
        return false;
    }

    if( R < minRes )
    {
        if( R > 0.0 )
        {
            ERRMSG << "\n + [BAD FILE] Right Circular Cylinder radius < MinRes \n";
            H = minRes;
        }
        else
        {
            ERRMSG << "\n + [BAD FILE] invalid radius for Right Circular Cylinder\n";
            pdout.clear();
            return false;
        }
    }

    double ddef = 0.0;

    if( !eor )
    {
        if( !ParseReal( pdout, idx, X1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no X1 value for Right Circular Cylinder\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        X1 = 0.0;
    }

    if( !eor )
    {
        if( !ParseReal( pdout, idx, Y1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no Y1 value for Right Circular Cylinder\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        Y1 = 0.0;
    }

    if( !eor )
    {
        if( !ParseReal( pdout, idx, Z1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no Z1 value for Right Circular Cylinder\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        Z1 = 0.0;
    }

    if( !eor )
    {
        if( !ParseReal( pdout, idx, I1, eor, pd, rd, &ddef ) )
        {
            ERRMSG << "\n + [BAD FILE] no I1 value for Right Circular Cylinder\n";
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
            ERRMSG << "\n + [BAD FILE] no J1 value for Right Circular Cylinder\n";
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
        if( !ParseReal( pdout, idx, K1, eor, pd, rd ) )
        {
            ERRMSG << "\n + [BAD FILE] no K1 value for Right Circular Cylinder\n";
            pdout.clear();
            return false;
        }
    }
    else
    {
        K1 = 1.0;
    }

    // ensure (I,J,K) is a unit vector
    if( !CheckNormal( I1, J1, K1 ) )
    {
        ERRMSG << "\n + [BAD FILE] invalid normal vector (cannot be normalized)\n";
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


bool IGES_ENTITY_154::SetEntityForm( int aForm )
{
    if( aForm != 0 )
    {
        ERRMSG << "\n + [ERROR] Right Circular Cylinder only supports Form 0\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_154::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase != STAT_USE_GEOMETRY )
    {
        ERRMSG << "\n + [ERROR] Right Circular Cylinder only supports STAT_USE_GEOMETRY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_154::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored by a Right Circular Cylinder so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] Right Circular Cylinder does not support hierarchy\n";
    return true;
}
