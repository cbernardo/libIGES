/*
 * file: entity126.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 126: NURBS Curve, Section 4.23, p.133(161+)
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
#include <entity126.h>

using namespace std;

IGES_ENTITY_126::IGES_ENTITY_126( IGES* aParent ) : IGES_CURVE( aParent )
{
    entityType = 126;
    form = 0;
    K = 0;
    M = 0;
    PROP1 = 0;
    PROP2 = 0;
    PROP3 = 0;
    PROP4 = 0;
    V0 = 0.0;
    V1 = 0.0;

    return;
}


IGES_ENTITY_126::~IGES_ENTITY_126()
{
    return;
}


bool IGES_ENTITY_126::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_126::format( int &index )
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

    if( K < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for K\n";
        return false;
    }

    if( M < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for M\n";
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

    // # of knots = 2 + K + M
    if( (2 + K + M) != (int)knots.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of knots (" << knots.size();
        cerr << ") expecting " << (2 + K + M) << "\n";
        return false;
    }

    // # of weights = K + 1
    if( (1 + K) != (int)weights.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of weights (" << weights.size();
        cerr << ") expecting " << (1 + K) << "\n";
        return false;
    }

    // # of control points = K + 1
    if( (1 + K) != (int)controls.size() )
    {
        ERRMSG << "\n + [INFO] invalid number of control points (" << controls.size();
        cerr << ") expecting " << (1 + K) << "\n";
        return false;
    }

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << K << pd;
    ostr << M << pd;
    ostr << PROP1 << pd;
    ostr << PROP2 << pd;
    ostr << PROP3 << pd;
    ostr << PROP4 << pd;
    string lstr = ostr.str();
    string tstr;

    std::list<double>::iterator sD = knots.begin();
    std::list<double>::iterator eD = knots.end();

    while( sD != eD )
    {
        if( !FormatPDREal( tstr, *sD, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots\n";
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

    if( !FormatPDREal( tstr, V0, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V0\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, V1, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V1\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, vnorm.x, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format normal vector\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, vnorm.y, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format normal vector\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    char tc = rd;

    if( !extras.empty() )
        tc = pd;

    if( !FormatPDREal( tstr, vnorm.z, tc, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format normal vector\n";
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


bool IGES_ENTITY_126::rescale( double sf )
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


bool IGES_ENTITY_126::Unlink( IGES_ENTITY* aChild )
{
    return IGES_ENTITY::Unlink( aChild );
}


bool IGES_ENTITY_126::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_126::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_126::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_126::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 && form != 1 && form != 2
        && form != 3 && form != 4 && form != 5 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in NURBS curve\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_126::ReadPD( std::ifstream& aFile, int& aSequenceVar )
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

    if( !ParseInt( pdout, idx, K, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K (upper index sum)\n";
        return false;
    }

    if( K < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K value (" << K << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, M, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M (degree of basis functions)\n";
        return false;
    }

    if( M < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M value (" << M << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP1 (0/1:planar/nonplanar)\n";
        return false;
    }

    if( PROP1 != 0 && PROP1 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP1 value (" << PROP1 << ")\n";
        return false;
    }

    if( !ParseInt( pdout, idx, PROP2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP2 (0/1:open/closed curve)\n";
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
        ERRMSG << "\n + [INFO] couldn't read PROP4 (0/1:nonperiodic/periodic)\n";
        return false;
    }

    if( PROP4 != 0 && PROP4 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP4 value (" << PROP4 << ")\n";
        return false;
    }

    double tR;

    for( int i = 2 + K + M; i > 0; --i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot value #" << (knots.size() + 1) << "\n";
            return false;
        }

        knots.push_back( tR );
    }

    for( int i = 0; i <= K; ++i )
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

    for( int i = 0; i <= K; ++i )
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

    if( !ParseReal( pdout, idx, V0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value\n";
        return false;
    }

    if( V0 < 0.0 || V0 > 1.0 )
    {
        ERRMSG << "\n + [INFO] invalid V0 value (" << V0 << ")\n";
        return false;
    }

    if( !ParseReal( pdout, idx, V1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value\n";
        return false;
    }

    if( V1 < 0.0 || V1 > 1.0 )
    {
        ERRMSG << "\n + [INFO] invalid V1 value (" << V1 << ")\n";
        return false;
    }

    // unit normal vector (ignored if curve is not planar)
    if( !ParseReal( pdout, idx, tX, eor, pd, rd )
        || !ParseReal( pdout, idx, tY, eor, pd, rd )
        || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read unit normal vector\n";
        return false;
    }

    if( PROP1 == 1 )
    {
        if( !CheckNormal( tX, tY, tZ ) )
        {

        }

        vnorm.x = tX;
        vnorm.y = tY;
        vnorm.z = tZ;
    }
    else
    {
        vnorm.x = 0.0;
        vnorm.y = 0.0;
        vnorm.z = 1.0;
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


bool IGES_ENTITY_126::SetEntityForm( int aForm )
{
    if( aForm != 0 && aForm != 1 && aForm != 2
        && aForm != 3 && aForm != 4 && aForm != 5 )
    {
        ERRMSG << "\n + [INFO] invalid Form(" << aForm;
        cerr << "), valid forms are 0..5 only\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_126::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // hierarchy is ignored so always return true
    return true;
}


bool IGES_ENTITY_126::IsClosed()
{
    // XXX - TO BE IMPLEMENTED
#warning TO BE IMPLEMENTED
    return false;
}


int IGES_ENTITY_126::GetNCurves( void )
{
    return 1;
}


IGES_CURVE* IGES_ENTITY_126::GetCurve( int index )
{
    // there are no child curves; return NULL
    return NULL;
}


IGES_POINT IGES_ENTITY_126::GetStartPoint( bool xform )
{
    // XXX - TO BE IMPLEMENTED
    IGES_POINT pt( 0.0,0.0,0.0 );
#warning TO BE IMPLEMENTED
    return pt;
}


IGES_POINT IGES_ENTITY_126::GetEndPoint( bool xform )
{
    // XXX - TO BE IMPLEMENTED
    IGES_POINT pt( 0.0,0.0,0.0 );
#warning TO BE IMPLEMENTED
    return pt;
}


int IGES_ENTITY_126::GetNSegments( void )
{
    return 1;
}

bool IGES_ENTITY_126::Interpolate( IGES_POINT& pt, int nSeg, double var, bool xform )
{
    // XXX - TO BE IMPLEMENTED
#warning TO BE IMPLEMENTED
    return false;
}
