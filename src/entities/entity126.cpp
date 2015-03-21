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

    nKnots = 0;
    nCoeffs = 0;
    knots = NULL;
    coeffs = NULL;

    return;
}


IGES_ENTITY_126::~IGES_ENTITY_126()
{
    if( knots )
        delete [] knots;

    if( coeffs )
        delete [] coeffs;

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
    if( (2 + K + M) != nKnots )
    {
        ERRMSG << "\n + [INFO] invalid number of knots (" << nKnots;
        cerr << ") expecting " << (2 + K + M) << "\n";
        return false;
    }

    // # of coefficients = K + 1
    if( (1 + K) != nCoeffs )
    {
        ERRMSG << "\n + [INFO] invalid number of coefficients (" << nCoeffs;
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

    for( int i = 0; i < nKnots; ++i )
    {
        if( !FormatPDREal( tstr, knots[i], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    double tD = 1.0;

    for( int i = 0, j = 3; i < nCoeffs; ++i )
    {
        if( 0 == PROP3 )
            tD = coeffs[j];

        if( !FormatPDREal( tstr, tD, pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format weights\n";
            return false;
        }

        j += 3;
        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    for( int i = 0, j = 0; i < nCoeffs; ++i )
    {
        if( !FormatPDREal( tstr, coeffs[j++], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( !FormatPDREal( tstr, coeffs[j++], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( !FormatPDREal( tstr, coeffs[j++], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        if( 0 == PROP3 )
            ++j;

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
    if( NULL == coeffs )
        return true;

    for( int i = 0, j = 0; i < nCoeffs; ++i )
    {
        coeffs[j] *= sf;
        ++j;
        coeffs[j] *= sf;
        ++j;
        coeffs[j] *= sf;
        ++j;

        if( 0 != PROP3 )
            ++j;
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

    if( !ParseInt( pdout, idx, K, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K (upper index sum)\n";
        pdout.clear();
        return false;
    }

    if( K < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K value (" << K << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, M, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M (degree of basis functions)\n";
        pdout.clear();
        return false;
    }

    if( M < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M value (" << M << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP1 (0/1:planar/nonplanar)\n";
        pdout.clear();
        return false;
    }

    if( PROP1 != 0 && PROP1 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP1 value (" << PROP1 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP2 (0/1:open/closed curve)\n";
        pdout.clear();
        return false;
    }

    if( PROP2 != 0 && PROP2 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP2 value (" << PROP2 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP3, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP3 (0/1:rational/polynomial)\n";
        pdout.clear();
        return false;
    }

    if( PROP3 != 0 && PROP3 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP3 value (" << PROP3 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP4, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP4 (0/1:nonperiodic/periodic)\n";
        pdout.clear();
        return false;
    }

    if( PROP4 != 0 && PROP4 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP4 value (" << PROP4 << ")\n";
        pdout.clear();
        return false;
    }

    double tR;

    if( knots )
        delete [] knots;

    if( coeffs )
        delete [] coeffs;

    knots = NULL;
    coeffs = NULL;
    nKnots = 2 + K + M;
    knots = new double[nKnots];

    if( NULL == knots )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for knots\n";
        pdout.clear();
        return false;
    }

    for( int i = 0; i < nKnots; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot value #" << (i + 1) << "\n";
            delete [] knots;
            knots = NULL;
            pdout.clear();
            return false;
        }

        knots[i] = tR;
    }

    nCoeffs = K + 1;

    if( 0 == PROP3 )
        coeffs = new double[nCoeffs * 4];   // rational
    else
        coeffs = new double[nCoeffs * 3];   // polynomial

    if( NULL == coeffs )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for coefficients\n";
        delete [] knots;
        knots = NULL;
        pdout.clear();
        return false;
    }

    for( int i = 0, j = 3; i <= K; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read weight value #" << (i + 1) << "\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        if( tR <= 0 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid weight (" << tR << ")\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        if( 0 == PROP3 )
        {
            coeffs[j] = tR;
            j += 4;
        }
    }

    double tX;
    double tY;
    double tZ;

    for( int i = 0, j = 0; i <= K; ++i )
    {
        if( !ParseReal( pdout, idx, tX, eor, pd, rd )
            || !ParseReal( pdout, idx, tY, eor, pd, rd )
            || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read control point #" << (i + 1) << "\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        coeffs[j++] = tX;
        coeffs[j++] = tY;
        coeffs[j++] = tZ;

        if( 0 == PROP3 )
            ++j;
    }

    if( !ParseReal( pdout, idx, V0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, V1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    // unit normal vector (ignored if curve is not planar)
    if( !ParseReal( pdout, idx, tX, eor, pd, rd )
        || !ParseReal( pdout, idx, tY, eor, pd, rd )
        || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read unit normal vector\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
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
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
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


bool IGES_ENTITY_126::GetNURBSData( int& nCoeff, int& order, double** knot, double** coeff, bool& isRational,
                                    bool& isClosed, bool& isPeriodic )
{
    nCoeff = 0;
    order =0 ;
    knot = NULL;
    coeff = NULL;

    if( !knots )
        return false;

    *knot = knots;
    *coeff = coeffs;
    nCoeff = nCoeffs;
    order = M + 1;

    if( PROP2 )
        isClosed = true;
    else
        isClosed = false;

    if( PROP3 )
        isRational = false;
    else
        isRational = true;

    if( PROP4 )
        isPeriodic = true;
    else
        isPeriodic = false;

    return true;
}



bool IGES_ENTITY_126::SetNURBSData( int& nCoeff, int& order, double* knot, double* coeff, bool& isRational,
                   bool& isClosed, bool& isPeriodic )
{
    if( !knot || !coeff )
    {
        ERRMSG << "\n + [INFO] invalid NURBS parameter pointer (NULL)\n";
        return false;
    }

    if( order < 2 )
    {
        ERRMSG << "\n + [INFO] invalid order; minimum is 2 which represents a line\n";
        return false;
    }

    if( nCoeff < order )
    {
        ERRMSG << "\n + [INFO] invalid number of control points; minimum is equal to the order of the B-Splines\n";
        return false;
    }

    // M = Degree of basis function; Order = Degree + 1
    // # of knots = 2 + K + M
    // # of coefficients = K + 1

    nKnots = nCoeff + order;
    nCoeffs = nCoeff;
    K = nCoeff - 1;
    M = order - 1;

    // XXX - delete SISL curve if applicable

    if( knots )
    {
        delete [] knots;
        knots = NULL;
    }

    if( coeffs )
    {
        delete [] coeffs;
        coeffs = NULL;
    }

    knots = knot;
    coeffs = coeff;

    return true;
}
