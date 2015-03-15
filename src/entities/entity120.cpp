/*
 * file: entity110.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 120: Surface of Revolution, Section 4.18, p.116+ (144+)
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
#include <iges_curve.h>
#include <entity120.h>
#include <entity124.h>

using namespace std;

IGES_ENTITY_120::IGES_ENTITY_120( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 120;
    form = 0;
    iL = 0;
    iC = 0;
    L = NULL;
    C = NULL;
    SA = 0;
    TA = M_PI;
}


IGES_ENTITY_120::~IGES_ENTITY_120()
{
    if( L )
        L->DelReference( this );

    if( C )
        C->DelReference( this );

    return;
}


bool IGES_ENTITY_120::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not register associations\n";
        return false;
    }

    int iEnt;

    if( iL )
    {
        iEnt = iL >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid entity (DE:" << iL << ")\n";
            return false;
        }

        L = dynamic_cast<IGES_CURVE*>((*entities)[iEnt]);

        if( !L )
        {
            ERRMSG << "\n + [INFO] could not associate line entity with DE " << iL << "\n";
            return false;
        }

        iEnt = L->GetEntityType();

        if( iEnt != 110 )
        {
            L = NULL;
            ERRMSG << "\n + [VIOLATION] invalid entity (Type: " << iEnt << "); only Type 110 (Line) is permitted\n";
            return false;
        }

        if( !L->AddReference( this ) )
        {
            L = NULL;
            ERRMSG << "\n + [INFO] could not associate line entity with DE " << iL << "\n";
        }
    }

    if( iC )
    {
        iEnt = iC >> 1;

        if( iEnt < 0 || iEnt >= (int)(*entities).size() )
        {
            ERRMSG << "\n + [INFO] invalid entity (DE:" << iC << ")\n";
            return false;
        }

        C = dynamic_cast<IGES_CURVE*>((*entities)[iEnt]);

        if( !C )
        {
            ERRMSG << "\n + [INFO] could not associate curve entity with DE " << iC << "\n";
            return false;
        }

        if( !C->AddReference( this ) )
        {
            C = NULL;
            ERRMSG << "\n + [INFO] could not associate curve (generatrix) with DE " << iC << "\n";
        }
    }

    return true;
}


bool IGES_ENTITY_120::format( int &index )
{
    pdout.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( !L )
    {
        ERRMSG << "\n + [INFO] no valid Line Entity\n";
        return false;
    }

    if( !C )
    {
        ERRMSG << "\n + [INFO] no valid Curve Entity\n";
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
    ostr << L->GetDESequence() << pd;
    ostr << C->GetDESequence() << pd;
    string lstr = ostr.str();
    string tstr;

    if( !FormatPDREal( tstr, SA, pd, IGES_ANGLE_RES ) )
    {
        ERRMSG << "\n + [INFO] could not format Start Angle\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    
    if( extras.empty() )
    {
        if( !FormatPDREal( tstr, TA, rd, IGES_ANGLE_RES ) )
        {
            ERRMSG << "\n + [INFO] could not format Terminate Angle\n";
            return false;
        }
    }
    else
    {
        if( !FormatPDREal( tstr, TA, pd, IGES_ANGLE_RES ) )
        {
            ERRMSG << "\n + [INFO] could not format Terminate Angle\n";
            return false;
        }
    }

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


bool IGES_ENTITY_120::rescale( double sf )
{
    // nothing to do
    return true;
}


bool IGES_ENTITY_120::Unlink( IGES_ENTITY* aChild )
{
    if( IGES_ENTITY::Unlink( aChild ) )
        return true;

    if( aChild == (IGES_ENTITY*)L )
    {
        L = NULL;
        return true;
    }

    if( aChild == (IGES_ENTITY*)C )
    {
        C = NULL;
        return true;
    }

    return false;
}


bool IGES_ENTITY_120::IsOrphaned( void )
{
    if( (refs.empty() && depends != STAT_INDEPENDENT) || !L || !C )
        return true;

    return false;
}


bool IGES_ENTITY_120::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_120::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_120::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
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
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Surface of Revolution\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_120::ReadPD( std::ifstream& aFile, int& aSequenceVar )
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

    int iPtr;

    if( !ParseInt( pdout, idx, iPtr, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the pointer to the line entity\n";
        return false;
    }

    if( iPtr < 1 || iPtr > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid entity pointer: " << iPtr << "\n";
        return false;
    }

    iL = iPtr;

    if( !ParseInt( pdout, idx, iPtr, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the pointer to the curve entity\n";
        return false;
    }

    if( iPtr < 1 || iPtr > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid entity pointer: " << iPtr << "\n";
        return false;
    }

    iC = iPtr;

    if( !ParseReal( pdout, idx, SA, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the Start Angle\n";
        return false;
    }

    if( !ParseReal( pdout, idx, TA, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the Terminate Angle\n";
        return false;
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

    // note: normally a scale would be performed here (re. globalData.convert)
    // but this entity does not own scalable data.

    pdout.clear();
    return true;
}


bool IGES_ENTITY_120::SetEntityForm( int aForm )
{
    if( aForm == 0 )
        return true;

    ERRMSG << "\n + [BUG] Surface of Revolution only supports Form 0 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_120::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // hierarchy is ignored so always return true
    return true;
}
