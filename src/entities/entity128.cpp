/*
 * file: entity126.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 128: NURBS Surface, Section 4.24, p.137(165+)
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
#include <iges_helpers.h>
#include <entity124.h>
#include <entity128.h>

using namespace std;

IGES_ENTITY_128::IGES_ENTITY_128( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 128;
    form = 0;
    K1 = 0;
    K2 = 0;
    M1 = 0;
    M2 = 0;
    PROP1 = 0;
    PROP2 = 0;
    PROP3 = 0;
    PROP4 = 0;
    PROP5 = 0;
    U0 = 0.0;
    U1 = 0.0;
    V0 = 0.0;
    V1 = 0.0;

    return;
}


IGES_ENTITY_128::~IGES_ENTITY_128()
{
    return;
}


bool IGES_ENTITY_128::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_128::format( int &index )
{
    pdout.clear();

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
    double uir = 1e-6;  // any REAL parameters are NURBS controls; maintain high precision

    if( K1 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for K1\n";
        return false;
    }

    if( K2 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for K2\n";
        return false;
    }

    if( M1 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for M1\n";
        return false;
    }

    if( M2 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for M2\n";
        return false;
    }

    if( PROP1 < 0 || PROP1 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP1\n";
        return false;
    }

    if( PROP2 < 0 || PROP2 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP2\n";
        return false;
    }

    if( PROP3 < 0 || PROP3 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP3\n";
        return false;
    }

    if( PROP4 < 0 || PROP4 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP4\n";
        return false;
    }

    if( PROP5 < 0 || PROP5 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP5\n";
        return false;
    }

    // # of knots1 = 2 + K1 + M1
    if( (2 + K1 + M1) != (int)knots1.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of knots[1] (" << knots1.size();
        cerr << ") expecting " << (2 + K1 + M1) << "\n";
        return false;
    }

    // # of knots2 = 2 + K2 + M2
    if( (2 + K2 + M2) != (int)knots2.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of knots[2] (" << knots2.size();
        cerr << ") expecting " << (2 + K2 + M2) << "\n";
        return false;
    }

    // # of weights = (K1 + 1)*(K2 + 1)
    int C = (K1 + 1)*(K2 + 1);

    if( C != (int)weights.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of weights (" << weights.size();
        cerr << ") expecting " << C << "\n";
        return false;
    }

    // # of control points = (K1 + 1)*(K2 + 1)
    if( C != (int)controls.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of control points (" << controls.size();
        cerr << ") expecting " << C << "\n";
        return false;
    }

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << K1 << pd;
    ostr << K2 << pd;
    ostr << M1 << pd;
    ostr << M2 << pd;
    ostr << PROP1 << pd;
    ostr << PROP2 << pd;
    ostr << PROP3 << pd;
    ostr << PROP4 << pd;
    ostr << PROP5 << pd;
    string lstr = ostr.str();
    string tstr;

    std::list<double>::iterator sD = knots1.begin();
    std::list<double>::iterator eD = knots1.end();

    while( sD != eD )
    {
        if( !FormatPDREal( tstr, *sD, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots[1]\n";
            return false;
        }

        ++sD;
        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    sD = knots2.begin();
    eD = knots2.end();

    while( sD != eD )
    {
        if( !FormatPDREal( tstr, *sD, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots[2]\n";
            return false;
        }

        ++sD;
        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    sD = weights.begin();
    eD = weights.end();

    while( sD != eD )
    {
        if( !FormatPDREal( tstr, *sD, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format weights\n";
            return false;
        }

        ++sD;
        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    std::list<IGES_POINT>::iterator sP = controls.begin();
    std::list<IGES_POINT>::iterator eP = controls.end();

    while( sP != eP )
    {
        if( !FormatPDREal( tstr, (*sP).x, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( !FormatPDREal( tstr, (*sP).y, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( !FormatPDREal( tstr, (*sP).z, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        ++sP;
        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    if( !FormatPDREal( tstr, U0, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V0\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, U1, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V1\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, V0, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V0\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    char tc = rd;

    if( !extras.empty() )
        tc = pd;

    if( !FormatPDREal( tstr, V1, tc, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V1\n";
        return false;
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


bool IGES_ENTITY_128::rescale( double sf )
{
    list<IGES_POINT>::iterator sPt = controls.begin();
    list<IGES_POINT>::iterator ePt = controls.end();

    while( sPt != ePt )
    {
        *sPt = *sPt * sf;
        ++sPt;
    }

    return true;
}


bool IGES_ENTITY_128::Unlink( IGES_ENTITY* aChild )
{
    return IGES_ENTITY::Unlink( aChild );
}


bool IGES_ENTITY_128::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_128::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_128::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_128::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form < 0 || form > 9 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in NURBS surface\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_128::ReadPD( std::ifstream& aFile, int& aSequenceVar )
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

    if( !ParseInt( pdout, idx, K1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K1 (upper index sum)\n";
        return false;
    }

    if( K1 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K1 value (" << K1 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, K2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K2 (upper index sum)\n";
        return false;
    }

    if( K2 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K2 value (" << K2 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, M1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M1 (degree of basis functions)\n";
        return false;
    }

    if( M1 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M1 value (" << M1 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, M2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M2 (degree of basis functions)\n";
        return false;
    }

    if( M2 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M2 value (" << M2 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP1 (0/1:open/closed for all U)\n";
        return false;
    }

    if( PROP1 != 0 && PROP1 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP1 value (" << PROP1 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP2 (0/1:open/closed for all V)\n";
        return false;
    }

    if( PROP2 != 0 && PROP2 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP2 value (" << PROP2 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP3, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP3 (0/1:rational/polynomial)\n";
        return false;
    }

    if( PROP3 != 0 && PROP3 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP3 value (" << PROP3 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP4, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP4 (0/1:nonperiodic/periodic in U)\n";
        return false;
    }

    if( PROP4 != 0 && PROP4 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP4 value (" << PROP4 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP5, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP5 (0/1:nonperiodic/periodic in V)\n";
        return false;
    }

    if( PROP5 != 0 && PROP5 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP5 value (" << PROP5 << ")\n";
        return false;
    }

    double tR;

    for( int i = 2 + K1 + M1; i > 0; --i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot value #" << (knots1.size() + 1) << "\n";
            return false;
        }

        knots1.push_back( tR );
    }

    for( int i = 2 + K2 + M2; i > 0; --i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot value #" << (knots2.size() + 1) << "\n";
            return false;
        }

        knots2.push_back( tR );
    }

    int C = (K1 + 1)*(K2 + 1);

    for( int i = 0; i < C; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read weight value #" << (i + 1) << "\n";
            return false;
        }

        weights.push_back( tR );
    }

    double tX;
    double tY;
    double tZ;

    for( int i = 0; i < C; ++i )
    {
        if( !ParseReal( pdout, idx, tX, eor, pd, rd )
            || !ParseReal( pdout, idx, tY, eor, pd, rd )
            || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read control point #" << (i + 1) << "\n";
            return false;
        }

        controls.push_back( IGES_POINT( tX, tY, tZ ) );
    }

    if( !ParseReal( pdout, idx, U0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value U0\n";
        return false;
    }

    if( U0 < 0.0 || U0 > 1.0 )
    {
        ERRMSG << "\n + [INFO] invalid U0 value (" << V0 << ")\n";
        return false;
    }

    if( !ParseReal( pdout, idx, U1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value U1\n";
        return false;
    }

    if( U1 < 0.0 || U1 > 1.0 )
    {
        ERRMSG << "\n + [INFO] invalid U1 value (" << U1 << ")\n";
        return false;
    }

    if( !ParseReal( pdout, idx, V0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value V0\n";
        return false;
    }

    if( V0 < 0.0 || V0 > 1.0 )
    {
        ERRMSG << "\n + [INFO] invalid V0 value (" << V0 << ")\n";
        return false;
    }

    if( !ParseReal( pdout, idx, V1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value V1\n";
        return false;
    }

    if( V1 < 0.0 || V1 > 1.0 )
    {
        ERRMSG << "\n + [INFO] invalid V1 value (" << V1 << ")\n";
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

    if( parent->globalData.convert )
        rescale( parent->globalData.cf );

    pdout.clear();
    return true;
}


bool IGES_ENTITY_128::SetEntityForm( int aForm )
{
    if( aForm < 0 || aForm > 9 )
    {
        ERRMSG << "\n + [INFO] invalid Form(" << aForm;
        cerr << "), valid forms are 0..9 only\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_128::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // hierarchy is ignored so always return true
    return true;
}
