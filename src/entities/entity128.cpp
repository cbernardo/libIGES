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
#include <sisl.h>
#include <iges.h>
#include <iges_io.h>
#include <mcad_helpers.h>
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

    nKnots1 = 0;
    nKnots2 = 0;
    nCoeffs1 = 0;
    nCoeffs2 = 0;
    knots1 = NULL;
    knots2 = NULL;
    coeffs = NULL;
    ssurf = NULL;

    return;
}


IGES_ENTITY_128::~IGES_ENTITY_128()
{
    if( knots1 )
        delete [] knots1;

    if( knots2 )
        delete [] knots2;

    if( coeffs )
        delete [] coeffs;

    if( ssurf )
        freeSurf( ssurf );

    return;
}


bool IGES_ENTITY_128::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_128::format( int &index )
{
    pdout.clear();

    if( !knots1 || !knots2 || !coeffs )
    {
        ERRMSG << "\n + [INFO] no surface data\n";
        return false;
    }

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
    double uir = 1e-8;  // any REAL parameters are NURBS data, maintain high precision

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
    if( (2 + K1 + M1) != nKnots1 )
    {
        ERRMSG << "\n + [INFO] invalid number of knots[1] (" << nKnots1;
        cerr << ") expecting " << (2 + K1 + M1) << "\n";
        return false;
    }

    // # of knots2 = 2 + K2 + M2
    if( (2 + K2 + M2) != nKnots2 )
    {
        ERRMSG << "\n + [INFO] invalid number of knots[2] (" << nKnots1;
        cerr << ") expecting " << (2 + K2 + M2) << "\n";
        return false;
    }

    // # of weights = (K1 + 1)*(K2 + 1)
    int C = (K1 + 1)*(K2 + 1);

    if( C != nCoeffs1 * nCoeffs2 )
    {
        ERRMSG << "\n + [INFO] invalid number of weights (" << (nCoeffs1 * nCoeffs2);
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

    for( int i = 0; i < nKnots1; ++i )
    {
        if( !FormatPDREal( tstr, knots1[i], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots[1]\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    for( int i = 0; i < nKnots2; ++i )
    {
        if( !FormatPDREal( tstr, knots2[i], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots[2]\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    if( 0 == PROP3 )
    {
        for( int i = 0, j = 3; i < C; ++i, j += 4 )
        {
            if( !FormatPDREal( tstr, coeffs[j], pd, 1e-6 ) )
            {
                ERRMSG << "\n + [INFO] could not format weights\n";
                return false;
            }

            AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
        }

    }
    else
    {
        double tD = 1.0;

        for( int i = 0; i < C; ++i )
        {
            if( !FormatPDREal( tstr, tD, pd, 0.1 ) )
            {
                ERRMSG << "\n + [INFO] could not format weights\n";
                return false;
            }

            AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
        }
    }

    for( int i = 0, j = 0; i < C; ++i )
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

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( 0 == PROP3 )
            ++j;
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
    if( !coeffs )
        return true;

    int C = nCoeffs1 * nCoeffs2;

    if( 0 == PROP3 )
    {
        for( int i = 0, j = 0; i < C; ++i )
        {
            coeffs[j] *= sf;
            ++j;
            coeffs[j] *= sf;
            ++j;
            coeffs[j] *= sf;
            j += 2;
        }
    }
    else
    {
        for( int i = 0, j = 0; i < C; ++i )
        {
            coeffs[j] *= sf;
            ++j;
            coeffs[j] *= sf;
            ++j;
            coeffs[j] *= sf;
            ++j;
        }
    }

    return true;
}


bool IGES_ENTITY_128::unlink(IGES_ENTITY *aChild)
{
    return IGES_ENTITY::unlink(aChild);
}


bool IGES_ENTITY_128::isOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_128::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_128::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_128::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
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


bool IGES_ENTITY_128::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for B-Spline Surface\n";
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

    if( !ParseInt( pdout, idx, K1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K1 (upper index sum)\n";
        pdout.clear();
        return false;
    }

    if( K1 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K1 value (" << K1 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, K2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K2 (upper index sum)\n";
        pdout.clear();
        return false;
    }

    if( K2 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K2 value (" << K2 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, M1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M1 (degree of basis functions)\n";
        pdout.clear();
        return false;
    }

    if( M1 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M1 value (" << M1 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, M2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M2 (degree of basis functions)\n";
        pdout.clear();
        return false;
    }

    if( M2 < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M2 value (" << M2 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP1 (0/1:open/closed for all U)\n";
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
        ERRMSG << "\n + [INFO] couldn't read PROP2 (0/1:open/closed for all V)\n";
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
        ERRMSG << "\n + [INFO] couldn't read PROP4 (0/1:nonperiodic/periodic in U)\n";
        pdout.clear();
        return false;
    }

    if( PROP4 != 0 && PROP4 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP4 value (" << PROP4 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP5, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP5 (0/1:nonperiodic/periodic in V)\n";
        pdout.clear();
        return false;
    }

    if( PROP5 != 0 && PROP5 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP5 value (" << PROP5 << ")\n";
        pdout.clear();
        return false;
    }

    double tR;

    nKnots1 = 2 + K1 + M1;

    knots1 = new double[nKnots1];

    if( NULL == knots1 )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for knots1\n";
        pdout.clear();
        return false;
    }

    for( int i = 0; i < nKnots1; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot1 value #" << (i + 1) << "\n";
            delete [] knots1;
            knots1 = NULL;
            pdout.clear();
            return false;
        }

        knots1[i] = tR;
    }

    nKnots2 = 2 + K2 + M2;

    knots2 = new double[nKnots2];

    if( NULL == knots2 )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for knots2\n";
        delete [] knots1;
        knots1 = NULL;
        pdout.clear();
        return false;
    }

    for( int i = 0; i < nKnots2; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot2 value #" << (i + 1) << "\n";
            delete [] knots1;
            knots1 = NULL;
            delete [] knots2;
            knots2 = NULL;
            pdout.clear();
            return false;
        }

        knots2[i] = tR;
    }

    nCoeffs1 = (K1 + 1);
    nCoeffs2 = (K2 + 1);
    int C = nCoeffs1 * nCoeffs2;

    if( 0 == PROP3 )
    {
        // rational splines
        coeffs = new double[C * 4];
    }
    else
    {
        // polynomial splines
        coeffs = new double[C * 3];
    }

    if( NULL == coeffs )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for coefficients\n";
        delete [] knots1;
        knots1 = NULL;
        delete [] knots2;
        knots2 = NULL;
        pdout.clear();
        return false;
    }

    if( 0 == PROP3 )
    {
        for( int i = 0, j = 3; i < C; ++i, j += 4 )
        {
            if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
            {
                ERRMSG << "\n + [INFO] couldn't read weight value #" << (i + 1) << "\n";
                delete [] knots1;
                knots1 = NULL;
                delete [] knots2;
                knots2 = NULL;
                delete [] coeffs;
                coeffs = NULL;
                pdout.clear();
                return false;
            }

            if( tR <= 0 )
            {
                ERRMSG << "\n + [CORRUPT FILE] invalid weight (" << tR << ")\n";
                delete [] knots1;
                knots1 = NULL;
                delete [] knots2;
                knots2 = NULL;
                delete [] coeffs;
                coeffs = NULL;
                pdout.clear();
                return false;
            }

            coeffs[j] = tR;
        }
    }
    else
    {
        for( int i = 0; i < C; ++i )
        {
            if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
            {
                ERRMSG << "\n + [INFO] couldn't read weight value #" << (i + 1) << "\n";
                delete [] knots1;
                knots1 = NULL;
                delete [] knots2;
                knots2 = NULL;
                delete [] coeffs;
                coeffs = NULL;
                pdout.clear();
                return false;
            }

            if( tR <= 0 )
            {
                ERRMSG << "\n + [CORRUPT FILE] invalid weight (" << tR << ")\n";
                delete [] knots1;
                knots1 = NULL;
                delete [] knots2;
                knots2 = NULL;
                delete [] coeffs;
                coeffs = NULL;
                pdout.clear();
                return false;
            }
        }
    }

    double tX;
    double tY;
    double tZ;

    for( int i = 0, j = 0; i < C; ++i )
    {
        if( !ParseReal( pdout, idx, tX, eor, pd, rd )
            || !ParseReal( pdout, idx, tY, eor, pd, rd )
            || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read control point #" << (i + 1) << "\n";
            delete [] knots1;
            knots1 = NULL;
            delete [] knots2;
            knots2 = NULL;
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

    if( !ParseReal( pdout, idx, U0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value U0\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, U1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value U1\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, V0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value V0\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, V1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value V1\n";
        pdout.clear();
        return false;
    }

    if( U0 < -1e-10 || U0 > 1e-10 )
    {
        // shift the knot values
        for( int i = 0; i < nKnots1; ++i )
            knots1[i] -= U0;

        U1 -= U0;
        U0 = 0.0;
    }

    if( 1.0 != U1 )
    {
        // normalize the knot vector
        for( int i = 0; i < nKnots1; ++i )
            knots1[i] /= U1;

        U1 = 1.0;
    }

    if( V0 < -1e-10 || V0 > 1e-10 )
    {
        // shift the knot values
        for( int i = 0; i < nKnots2; ++i )
            knots2[i] -= V0;

        V1 -= V0;
        V0 = 0.0;
    }

    if( 1.0 != V1 )
    {
        // normalize the knot vector
        for( int i = 0; i < nKnots2; ++i )
            knots2[i] /= V1;

        V1 = 1.0;
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


bool IGES_ENTITY_128::IsRational( void )
{
    if( 0 == PROP3 )
        return true;

    return false;
}


bool IGES_ENTITY_128::isClosed1( void )
{
    if( 1 == PROP1 )
        return true;

    return false;
}


bool IGES_ENTITY_128::isClosed2( void )
{
    if( 1 == PROP2 )
        return true;

    return false;
}


bool IGES_ENTITY_128::isPeriodic1( void )
{
    if( 1 == PROP4 )
        return true;

    return false;
}


bool IGES_ENTITY_128::isPeriodic2( void )
{
    if( 1 == PROP5 )
        return true;

    return false;
}


bool IGES_ENTITY_128::GetNURBSData( int& nCoeff1, int& nCoeff2,
                                    int& order1, int& order2,
                                    double** knot1, double** knot2,
                                    double** coeff, bool& isRational,
                                    bool& isClosed1, bool& isClosed2,
                                    bool& isPeriodic1, bool& isPeriodic2 )
{
    nCoeff1 = 0;
    nCoeff2 = 0;
    order1 = 0 ;
    order2 = 0 ;
    knot1 = NULL;
    knot2 = NULL;
    coeff = NULL;

    if( !knots1 )
        return false;

    *knot1 = knots1;
    *knot2 = knots2;
    *coeff = coeffs;
    nCoeff1 = nCoeffs1;
    nCoeff2 = nCoeffs2;
    order1 = M1 + 1;
    order2 = M2 + 1;

    if( PROP1 )
        isClosed1 = true;
    else
        isClosed1 = false;

    if( PROP2 )
        isClosed2 = true;
    else
        isClosed2 = false;

    if( PROP3 )
        isRational = false;
    else
        isRational = true;

    if( PROP4 )
        isPeriodic1 = true;
    else
        isPeriodic1 = false;

    if( PROP5 )
        isPeriodic2 = true;
    else
        isPeriodic2 = false;

    return true;
}


bool IGES_ENTITY_128::SetNURBSData( int nCoeff1, int nCoeff2, int order1, int order2,
                                    const double* knot1, const double* knot2,
                                    const double* coeff, bool isRational,
                                    bool isPeriodic1, bool isPeriodic2 )
{
    if( !knot1 || !knot2 || !coeff )
    {
        ERRMSG << "\n + [INFO] invalid NURBS parameter pointer (NULL)\n";
        return false;
    }

    if( order1 < 2 )
    {
        ERRMSG << "\n + [INFO] invalid order1; minimum is 2 which represents a line\n";
        return false;
    }

    if( order2 < 2 )
    {
        ERRMSG << "\n + [INFO] invalid order2; minimum is 2 which represents a line\n";
        return false;
    }

    if( nCoeff1 < order1 )
    {
        ERRMSG << "\n + [INFO] invalid number of control points in parameter 1; minimum is equal to the order of the B-Splines\n";
        return false;
    }

    if( nCoeff2 < order2 )
    {
        ERRMSG << "\n + [INFO] invalid number of control points in parameter 2; minimum is equal to the order of the B-Splines\n";
        return false;
    }

    // M = Degree of basis function; Order = Degree + 1
    // # of knots = 2 + K + M
    // # of coefficients = K + 1

    nKnots1 = nCoeff1 + order1;
    nKnots2 = nCoeff2 + order2;
    nCoeffs1 = nCoeff1;
    nCoeffs2 = nCoeff2;
    K1 = nCoeff1 - 1;
    K2 = nCoeff2 - 1;
    M1 = order1 - 1;
    M2 = order2 - 1;

    if( ssurf )
    {
        freeSurf( ssurf );
        ssurf = NULL;
    }

    if( knots1 )
    {
        delete [] knots1;
        knots1 = NULL;
    }

    if( knots2 )
    {
        delete [] knots2;
        knots2 = NULL;
    }

    if( coeffs )
    {
        delete [] coeffs;
        coeffs = NULL;
    }

    // flag whether the surface is rational or polynomial
    if( isRational )
        PROP3 = 0;
    else
        PROP3 = 1;

    knots1 = new double[nKnots1];

    if( !knots1 )
    {
        ERRMSG << "\n + [INFO] memory allocation failed for knots1[]\n";
        return false;
    }

    knots2 = new double[nKnots2];

    if( !knots2 )
    {
        ERRMSG << "\n + [INFO] memory allocation failed for knots2[]\n";
        delete [] knots1;
        knots1 = NULL;
        return false;
    }

    int nDbls;

    if( isRational )
        nDbls = nCoeffs1 * nCoeffs2 * 4;
    else
        nDbls = nCoeffs1 * nCoeffs2 * 3;

    coeffs = new double[nDbls];

    if( !coeffs )
    {
        ERRMSG << "\n + [INFO] memory allocation failed for coeffs[]\n";
        delete [] knots1;
        knots1 = NULL;
        delete [] knots2;
        knots2 = NULL;
        return false;
    }

    for( int i = 0; i < nKnots1; ++i )
        knots1[i] = knot1[i];

    for( int i = 0; i < nKnots2; ++i )
        knots2[i] = knot2[i];

    U0 = knots1[0];
    U1 = knots1[nKnots1 -1];
    V0 = knots2[0];
    V1 = knots2[nKnots2 -1];

    if( U0 < -1e-10 || U0 > 1e-10 )
    {
        // shift the knot values
        for( int i = 0; i < nKnots1; ++i )
            knots1[i] -= U0;

        U1 -= U0;
        U0 = 0.0;
    }

    if( 1.0 != U1 )
    {
        // normalize the knot vector
        for( int i = 0; i < nKnots1; ++i )
            knots1[i] /= U1;

        U1 = 1.0;
    }

    if( V0 < -1e-10 || V0 > 1e-10 )
    {
        // shift the knot values
        for( int i = 0; i < nKnots2; ++i )
            knots2[i] -= V0;

        V1 -= V0;
        V0 = 0.0;
    }

    if( 1.0 != V1 )
    {
        // normalize the knot vector
        for( int i = 0; i < nKnots2; ++i )
            knots2[i] /= V1;

        V1 = 1.0;
    }

    for( int i = 0; i < nDbls; ++i )
        coeffs[i] = coeff[i];

    ssurf = newSurf( nCoeffs1, nCoeffs2, M1 + 1, M2 + 1,
                     knots1, knots2, coeffs, PROP3 ? 1 : 2, 3, 0 );

    if( !ssurf )
    {
        ERRMSG << "\n + [INFO] memory allocation failed in SISL newSurf()\n";
        return false;
    }

    int stat = 0;
    s1603( ssurf, &U0, &V0, &U1, &V1, &stat );

    switch ( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified problems determining U,V parameter values\n";
            break;

        default:
            ERRMSG << "\n + [INFO] could not determine U,V parameter values\n";
            return false;
            break;
    }

    // determine closure; we rely on the user to supply the correct periodicity
    double uir = 1e-8;
    stat = 0;

    if( parent )
        uir = parent->globalData.minResolution;

    do
    {
        int dg1, dg2, dg3, dg4;
        s1450( ssurf, uir, &PROP1, &PROP2, &dg1, &dg2, &dg3, &dg4, &stat );
    } while( 0 );

    switch ( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified problems determining closure\n";
            break;

        default:
            ERRMSG << "\n + [INFO] could not determine closure\n";
            return false;
            break;
    }

    if( !PROP1 && isPeriodic1 )
    {
        ERRMSG << "\n + [WARNING] surface open in Parameter 1 specified as periodic\n";
        isPeriodic1 = false;
    }

    if( !PROP2 && isPeriodic2 )
    {
        ERRMSG << "\n + [WARNING] surface open in Parameter 2 specified as periodic\n";
        isPeriodic2 = false;
    }

    if( isPeriodic1 )
        PROP4 = 1;
    else
        PROP4 = 0;

    if( isPeriodic2 )
        PROP5 = 1;
    else
        PROP5 = 0;

    return true;
}
