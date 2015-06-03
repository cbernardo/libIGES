/*
 * file: entity100.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 104: Conic Arc, Section 4.5, p.74+ (102+)
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
#include <entity104.h>
#include <entity124.h>

using namespace std;


IGES_ENTITY_104::IGES_ENTITY_104( IGES* aParent ) : IGES_CURVE( aParent )
{
    entityType = 104;
    form = 0;

    A = 0.0;
    B = 0.0;
    C = 0.0;
    D = 0.0;
    E = 0.0;
    F = 0.0;
    ZT = 0.0;
    X1 = 0.0;
    Y1 = 0.0;
    X2 = 0.0;
    Y2 = 0.0;

    return;
}   // IGES_ENTITY_104( IGES* aParent )


IGES_ENTITY_104::~IGES_ENTITY_104()
{
    return;
}   // ~IGES_ENTITY_104()


int IGES_ENTITY_104::getForm()
{
    // General form of the conic:
    // AX**2 + BXY + CY**2 + DX + EY + F = 0
    // Using the discriminant Q = B**2 - 4AC, the conic is:
    // # C = A, B = 0 == circle (we should not have this case for IGES)
    // # Q = 0 == parabola
    // # Q < 0 == ellipse
    // # Q > 0 == hyperbola
    double q = B*B - 4*A*C;

    // From IGES spec: Using the determinants Q1 .. Q3
    // Q1 = A * (CF - EE/4) - B/2 * ( BF/2 - DE/4 ) + D/2 * ( BE/4 - CD/2 )
    // Q2 = AC  - B*B/4
    // Q3 = A + C
    // IF Q2 > 0 && Q1*Q3 < 0 == ellipse
    // IF Q2 < 0 && Q1 != 0 == hyperbola
    // IF Q2 = 0 && Q1 != 0 == parabola
    double q1 = A*(C*F - E*E/4.0) - B*(B*F/2.0 - D*E/4.0)/2.0 + D*(B*E/4.0 - C*D/2.0)/2.0;
    double q2 = A*C - B*B/4;
    double q3 = A + C;

    if( B == 0 && C == A )
        ERRMSG << "\n + [WARNING]: [CONIC] section is a circle\n";

    if( q2 > 0.0 && q1*q3 < 0.0 )
    {
        // ellipse
        if( !(q < 0.0) )
            ERRMSG << "\n + [WARNING]: [CONIC] elliptical tests do not tally\n";

        return 1;
    }

    if( q2 < 0.0 && q1 != 0.0 )
    {
        // hyperbola
        if( !(q > 0.0) )
            ERRMSG << "\n + [WARNING]: [CONIC] hyperbolic tests do not  tally\n";

        return 2;
    }

    if( q2 == 0.0 && q1 != 0.0 )
    {
        // parabola
        if( !(q == 0.0) )
            ERRMSG << "\n + [WARNING]: [CONIC] parabolic tests do not  tally\n";

        return 3;
    }

    ERRMSG << "\n + [INFO]: [CONIC] invalid data; cannot classify the conic\n";

    return 0;
}


bool IGES_ENTITY_104::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->DelReference( this );
        pStructure = NULL;
    }

    return true;
}


bool IGES_ENTITY_104::format( int &index )
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

    ostringstream ostr;
    ostr << entityType << pd;
    string fStr = ostr.str();
    string tStr;

    if( form == 0 )
        form = getForm();

    double* dp[11] = { &A, &B, &C, &D, &E, &F, &ZT, &X1, &Y1, &X2, &Y2 };

    for( int i = 0; i < 10; ++i )
    {
        // note: we require a lot of digits since the points
        // of a conic are extremely sensitive to the general
        // coefficients
        if( !FormatPDREal( tStr, *dp[i], pd, 1e-10 ) )
        {
            ERRMSG << "\n + [INFO] could not format datum " << i << "\n";
            return false;
        }

        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
    }

    // note: 2 sets of OPTIONAL parameters may exist at the end of
    // any PD; see p.32/60+ for details; if optional parameters
    // need to be written then we should use 'pd' rather than 'rd'
    // in this call to FormatPDREal()
    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    if( !FormatPDREal( tStr, *dp[10], idelim, 1e-10 ) )
    {
        ERRMSG << "\n + [INFO] could not format datum 10\n";
        pdout.clear();
        return false;
    }

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


bool IGES_ENTITY_104::Unlink( IGES_ENTITY* aChild )
{
    return IGES_ENTITY::Unlink( aChild );
}


bool IGES_ENTITY_104::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_104::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_104::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_104::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form < 1 || form > 3 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in conic\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_104::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Conic Entity\n";
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
    double* dp[11] = { &A, &B, &C, &D, &E, &F, &ZT, &X1, &Y1, &X2, &Y2 };

    for( int i = 0; i < 11; ++i )
    {
        if( !ParseReal( pdout, idx, *dp[i], eor, pd, rd ) )
        {
            ERRMSG << "\n + [BAD FILE] missing datum (>=" << i << ") for Conic Entity\n";
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


bool IGES_ENTITY_104::SetEntityForm( int aForm )
{
    if( aForm < 1 || aForm > 3 )
    {
        ERRMSG << "\n + [BUG] invalid Conic Form (";
        cerr << aForm << ")\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_104::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored by a Circle Entity so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] Conic Entity does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_104::rescale( double sf )
{
    A *= sf;
    B *= sf;
    C *= sf;
    D *= sf;
    E *= sf;
    F *= sf;
    ZT *= sf;
    X1 *= sf;
    Y1 *= sf;
    X2 *= sf;
    Y2 *= sf;

    return true;
}


bool IGES_ENTITY_104::GetStartPoint( MCAD_POINT& pt, bool xform )
{
    pt.x = X1;
    pt.y = Y1;
    pt.z = ZT;

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


bool IGES_ENTITY_104::GetEndPoint( MCAD_POINT& pt, bool xform )
{
    pt.x = X2;
    pt.y = Y2;
    pt.z = ZT;

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


int IGES_ENTITY_104::GetNSegments( void )
{
    return 1;
}


bool IGES_ENTITY_104::IsClosed( void )
{
    int ftype = getForm();

    // test if the ellipse is closed
    if( ftype == 1 && X1 == X2 && Y1 == Y2 )
        return true;

    return false;
}


int IGES_ENTITY_104::GetNCurves( void )
{
    return 0;
}


IGES_CURVE* IGES_ENTITY_104::GetCurve( int index )
{
    return NULL;
}


bool IGES_ENTITY_104::Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform )
{
    if( 1 !=nSeg )
    {
        ERRMSG << "\n + [INFO] nSeg (" << nSeg << ") must be 1 for a conic entity\n";
        return false;
    }

    if( var < 0.0 || var > 1.0 )
    {
        ERRMSG << "\n + [INFO] var (" << var << ") must be in the range (0,1)\n";
        return false;
    }

    MCAD_POINT pt0;

    if( !form )
    {
        form = getForm();

        if( 0 == form )
        {
            ERRMSG << "\n + [INFO] invalid conic section parameters\n";
            return false;
        }
    }

    bool ok;

    switch( form )
    {
        case 1:
            // ellipse
            ok = getPtEllipse( pt0, var );
            break;

        case 2:
            // hyperbola
            ok = getPtHyperbola( pt0, var );
            break;

        case 3:
            // parabola
            ok = getPtParabola( pt0, var );
            break;

        default:
            ERRMSG << "\n + [INFO] invalid conic section parameters\n";
            return false;
            break;
    }

    if( !ok )
    {
        ERRMSG << "\n + [INFO] could not interpolate conic type " << form << "\n";
        return false;
    }

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt0;
    else
        pt = pt0;

    return true;
}


bool IGES_ENTITY_104::getPtEllipse( MCAD_POINT& pt0, double var )
{
    if( A == 0.0 || (F < 0.0 && A < 0.0) || (F > 0.0 && A > 0.0) )
    {
        ERRMSG << "\n + [BUG] invalid values for A, F in ellipse\n";
        return false;
    }

    if( C == 0.0 || (F < 0.0 && C < 0.0) || (F > 0.0 && C > 0.0) )
    {
        ERRMSG << "\n + [BUG] invalid values for C, F in ellipse\n";
        return false;
    }

    double a = sqrt( -F / A );
    double b = sqrt( -F / C );
    double t1 = atan2( Y1, X1 );
    double t2 = atan2( Y2, X2 );

    if( t1 < 0.0 )
        t1 += 2.0 * M_PI;

    if( t2 <= t1 )
        t2 += 2.0 * M_PI;

    double t = t1 + var * (t2 - t1);
    pt0.x = a * cos(t);
    pt0.y = b * sin(t);
    pt0.z = ZT;
    return true;
}


bool IGES_ENTITY_104::getPtHyperbola( MCAD_POINT& pt0, double var )
{
    if( X1 == X2 && Y1 == Y2 )
    {
        ERRMSG << "\n + [BUG] invalid hyperbola parameters (section is a point)\n";
        return false;
    }

    double a;
    double b;
    double t;
    double t1 = atan2( Y1, X1 );
    double t2 = atan2( Y2, X2 );

    if( t1 <= - M_PI * 0.5 )
        t1 += 2.0 * M_PI;

    if( t2 >= M_PI * 0.5 )
        t2 -= 2.0 * M_PI;

    if( F*A < 0.0 && F*C > 0.0 )
    {
        a = sqrt( -F/A );
        b = sqrt( F/C );
        t = t1 + var * ( t2 - t1 );

        pt0.x = a / cos(t);
        pt0.y = b * tan(t);
        pt0.z = ZT;
        return true;
    }

    if( F*A > 0.0 && F*C < 0.0 )
    {
        a = sqrt( F/A );
        b = sqrt( -F/C );
        t = t1 + var * ( t2 - t1 );

        pt0.x = a * tan(t);
        pt0.y = b / cos(t);
        pt0.z = ZT;
        return true;
    }

    ERRMSG << "\n + [BUG]: could not calculate point on hyperbola\n";
    return false;
}


bool IGES_ENTITY_104::getPtParabola( MCAD_POINT& pt0, double var )
{
    if( X1 == X2 && Y1 == Y2 )
    {
        ERRMSG << "\n + [BUG] invalid parabola parameters (section is a point)\n";
        return false;
    }

    double t = 0.0;

    if( A != 0.0 && E != 0.0 )
    {
        t = X1 + var * ( X2 - X1 );
        pt0.x = t;
        pt0.y = -(A/E)*t*t;
        pt0.z = ZT;
        return true;
    }

    if( C != 0.0 && D != 0.0 )
    {
        t = Y1 + var * ( Y2 - Y1 );
        pt0.x = -(C/D)*t*t;
        pt0.y = t;
        pt0.z = ZT;
        return true;
    }

    ERRMSG << "\n + [BUG]: could not calculate point on parabola\n";
    return false;
}
