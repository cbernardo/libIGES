/*
 * file: entity124.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 124: Transformation Matrix, Section 4.21, p.123+ (151+)
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

/*
 * NOTE: Representing a rotation O about an axis U(x,y,z) with a
 * 3x3 matrix:
 *
 * R =  cos(O) + Ux**2(1-cos(O))    UxUy(1-cos(O))-Uz(sin(O))   UxUz(1-cos(O))+Uy(sin(O))
 *      UyUx(1-cos(O))+Uz(sin(O))   cos(O) + Uy**2(1-cos(O))    UyUz(1-cos(O))-Ux(sin(O))
 *      UzUx(1-cos(O))-Uy(sin(O))   UzUy(1-cos(O))+Ux(sin(O))   cos(O) + UZ**2(1-cos(O))
 *
 * When a scale operation is simultaneously applied, each row of R must be multiplied by the scale:
 * R1* = R1* * Sx
 * R2* = R2* * Sy
 * R3* = R3* * Sz
 *
 */


#include <cstring>
#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>


using namespace std;


IGES_POINT::IGES_POINT()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
    return;
}


IGES_POINT::IGES_POINT( const IGES_POINT& p )
{
    x = p.x;
    y = p.y;
    z = p.z;
    return;
}


IGES_POINT::IGES_POINT( const double x, const double y, const double z )
{
    IGES_POINT::x = x;
    IGES_POINT::y = y;
    IGES_POINT::z = z;
    return;
}


IGES_POINT& IGES_POINT::operator*=( const double scalar )
{
    *this = *this * scalar;
    return *this;
}


IGES_POINT& IGES_POINT::operator+=( const IGES_POINT& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}


IGES_POINT  IGES_POINT::operator+( const IGES_POINT& v )
{
    IGES_POINT p;
    p.x += v.x;
    p.y += v.y;
    p.z += v.z;
    return p;
}


IGES_POINT& IGES_POINT::operator-=( const IGES_POINT& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}


IGES_POINT  IGES_POINT::operator-( const IGES_POINT& v )
{
    IGES_POINT p;
    p.x -= v.x;
    p.y -= v.y;
    p.z -= v.z;
    return p;
}


IGES_POINT operator*( const IGES_POINT& v, const double scalar )
{
    IGES_POINT pt;
    pt.x = v.x * scalar;
    pt.y = v.y * scalar;
    pt.z = v.z * scalar;
    return pt;
}


IGES_POINT operator*( const double scalar, const IGES_POINT& v )
{
    return v * scalar;
}


IGES_MATRIX::IGES_MATRIX()
{
    memset( v, 0, sizeof(v) );
    return;
}


IGES_MATRIX::IGES_MATRIX( const IGES_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] = m.v[i][j];
    }
}


IGES_MATRIX& IGES_MATRIX::operator*=( double scalar )
{
    *this = *this * scalar;
    return *this;
}

IGES_MATRIX& IGES_MATRIX::operator*=( const IGES_MATRIX& m )
{
    *this = *this * m;
    return *this;
}


IGES_MATRIX& IGES_MATRIX::operator+=( const IGES_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] += m.v[i][j];
    }

    return *this;
}


IGES_MATRIX  IGES_MATRIX::operator+( const IGES_MATRIX& m )
{
    IGES_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = v[i][j] + m.v[i][j];
    }

    return tmp;
}


IGES_MATRIX& IGES_MATRIX::operator-=( const IGES_MATRIX& m )
{
    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            v[i][j] -= m.v[i][j];
    }

    return *this;
}


IGES_MATRIX IGES_MATRIX::operator-( const IGES_MATRIX& m )
{
    IGES_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = v[i][j] - m.v[i][j];
    }

    return tmp;
}


IGES_POINT operator*( const IGES_MATRIX& m, const IGES_POINT& v )
{
    IGES_POINT pt;
    pt.x = m.v[0][0] * v.x + m.v[0][1] * v.y + m.v[0][2] * v.z;
    pt.y = m.v[1][0] * v.x + m.v[1][1] * v.y + m.v[1][2] * v.z;
    pt.z = m.v[2][0] * v.x + m.v[2][1] * v.y + m.v[2][2] * v.z;

    return pt;
}


IGES_MATRIX operator*( const IGES_MATRIX& m, const IGES_MATRIX& n )
{
    IGES_MATRIX tmp;

    // First row
    tmp.v[0][0] = m.v[0][0] * n.v[0][0] + m.v[0][1] * n.v[1][0]
                    + m.v[0][2] * n.v[2][0];

    tmp.v[0][1] = m.v[0][0] * n.v[0][1] + m.v[0][1] * n.v[1][1]
                    + m.v[0][2] * n.v[2][1];

    tmp.v[0][2] = m.v[0][0] * n.v[0][2] + m.v[0][1] * n.v[1][2]
                    + m.v[0][2] * n.v[2][2];

    // Second row
    tmp.v[1][0] = m.v[1][0] * n.v[0][0] + m.v[1][1] * n.v[1][0]
                    + m.v[1][2] * n.v[2][0];

    tmp.v[1][1] = m.v[1][0] * n.v[0][1] + m.v[1][1] * n.v[1][1]
                    + m.v[1][2] * n.v[2][1];

    tmp.v[1][2] = m.v[1][0] * n.v[0][2] + m.v[1][1] * n.v[1][2]
                    + m.v[1][2] * n.v[2][2];

    // Third row
    tmp.v[2][0] = m.v[2][0] * n.v[0][0] + m.v[2][1] * n.v[1][0]
                    + m.v[2][2] * n.v[2][0];

    tmp.v[2][1] = m.v[2][0] * n.v[0][1] + m.v[2][1] * n.v[1][1]
                    + m.v[2][2] * n.v[2][1];

    tmp.v[2][2] = m.v[2][0] * n.v[0][2] + m.v[2][1] * n.v[1][2]
                    + m.v[2][2] * n.v[2][2];

    return tmp;
}


IGES_MATRIX operator*( const IGES_MATRIX& m, double scalar )
{
    IGES_MATRIX tmp;

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
            tmp.v[i][j] = m.v[i][j] * scalar;
    }

    return tmp;
}


IGES_MATRIX operator*( double scalar, const IGES_MATRIX& m )
{
    return m * scalar;
}


IGES_TRANSFORM::IGES_TRANSFORM()
{
    return;
}

IGES_TRANSFORM::IGES_TRANSFORM( const IGES_TRANSFORM& t )
{
    R = t.R;
    T = t.T;
    return;
}


IGES_TRANSFORM::IGES_TRANSFORM( const IGES_MATRIX& m, const IGES_POINT& v )
{
    R = m;
    T = v;
    return;
}


IGES_TRANSFORM& IGES_TRANSFORM::operator*=(const IGES_TRANSFORM& m)
{
    T = R * m.T + T;
    R = R * m.R;
    return *this;
}


IGES_TRANSFORM& IGES_TRANSFORM::operator*=(const double scalar)
{
    R *= scalar;
    T *= scalar;
    return *this;
}

// scalar * TX
IGES_TRANSFORM operator*( const double scalar, const IGES_TRANSFORM& m )
{
    IGES_TRANSFORM v( m );
    v *= scalar;
    return v;
}


// TX0 * TX1
IGES_TRANSFORM operator*( const IGES_TRANSFORM& m, const IGES_TRANSFORM& n )
{
    IGES_TRANSFORM v( m );
    v *= n;

    return v;
}


// TX * V (perform a transform + offset)
IGES_POINT operator*( const IGES_TRANSFORM& m, const IGES_POINT& v  )
{
    IGES_POINT p = m.R * v + m.T;
    return p;
}


IGES_ENTITY_124::IGES_ENTITY_124( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 124;
    form = 0;
    return;
}


IGES_ENTITY_124::~IGES_ENTITY_124()
{
    return;
}


bool IGES_ENTITY_124::associate( std::vector<IGES_ENTITY*>* entities )
{
    return IGES_ENTITY::associate( entities );
}


bool IGES_ENTITY_124::format( int &index )
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

    double* pt[3] = { &T.T.x, &T.T.y, &T.T.z };

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
        {
            if( !FormatPDREal( tstr, T.R.v[i][j], pd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format Transform::R[";
                cerr << i << "][" << j << "]\n";
                return false;
            }

            AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
        }

        if( i == 2 && extras.empty() )
        {
            if( !FormatPDREal( tstr, *pt[i], rd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format Transform::T[";
                cerr << i << "]\n";
                return false;
            }
        }
        else
        {
            if( !FormatPDREal( tstr, *pt[i], pd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format Transform::T[";
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


bool IGES_ENTITY_124::rescale( double sf )
{
    // to maintain relationships between the model and real space
    // all transforms must scale with a change in model scale or
    // a change in length unit
    T *= sf;
    return true;
}


bool IGES_ENTITY_124::Unlink( IGES_ENTITY* aChildEntity )
{
    if( !aChildEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed to method\n";
        return false;
    }

    if( aChildEntity->GetEntityType() != ENT_TRANSFORMATION_MATRIX )
    {
        ERRMSG << "\n + [BUG] invalid entity type on entity to unlink (";
        cerr << aChildEntity->GetEntityType() << ")\n";
        return false;
    }

    if( aChildEntity == pTransform )
    {
        pTransform = NULL;
        transform = 0;
        return true;
    }

    return false;
}


bool IGES_ENTITY_124::IsOrphaned( void )
{
    if( refs.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_124::IGES_ENTITY_124::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_124::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_124::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    lineFontPattern = 0;            // N.A.
    level = 0;                      // N.A.
    view = 0;                       // N.A.
    labelAssoc = 0;                 // N.A.
    visible = true;                 // N.A., but true => Blank Status = 0
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 && form != 1 && form < 10 && form > 12 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Transform\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_124::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Circle Entity\n";
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

    double* pt[3] = { &T.T.x, &T.T.y, &T.T.z };

    for( int i = 0; i < 3; ++i )
    {
        for( int j = 0; j < 3; ++j )
        {
            if( !ParseReal( pdout, idx, T.R.v[i][j], eor, pd, rd ) )
            {
                ERRMSG << "\n + [BAD FILE] no entry for Transform::R[";
                cerr << i << "][" << j << "]\n";
                return false;
            }
        }

        if( !ParseReal( pdout, idx, *pt[i], eor, pd, rd ) )
        {
            ERRMSG << "\n + [BAD FILE] no entry for Transform::T[";
            cerr << i << "]\n";
            return false;
        }
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

    return true;
}


bool IGES_ENTITY_124::SetEntityForm( int aForm )
{
    if( aForm != 0 && aForm != 1 && aForm < 10 && aForm > 12 )
    {
        ERRMSG << "\n + [BUG] invalid Form Number (" << aForm << ") specified\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_124::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    ERRMSG << "\n + [WARNING] Subordinate Entity Switch (dependency) not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetEntityUse( IGES_STAT_USE aUseCase )
{
    return IGES_ENTITY::SetEntityUse( aUseCase );
}


bool IGES_ENTITY_124::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    ERRMSG << "\n + [WARNING] hierarchy not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetLevel( int aLevel )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetLevel( IGES_ENTITY* aLevel )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


bool IGES_ENTITY_124::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [WARNING] method not supported by Transform Entity\n";
    return false;
}


// retrieves the overall transform matrix
IGES_TRANSFORM IGES_ENTITY_124::GetTransformMatrix( void )
{
    IGES_TRANSFORM V;

    // note: as per spec, any referenced Transforms are applied later
    if( pTransform )
        V = ((IGES_ENTITY_124*)pTransform)->GetTransformMatrix() * T;
    else
        V = T;

    return V;
}
