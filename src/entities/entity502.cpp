/*
 * file: entity502.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 502: Vertex List, Section 4.147, p.586+ (614+)
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

#include <error_macros.h>
#include <sstream>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity502.h>

using namespace std;


IGES_ENTITY_502::IGES_ENTITY_502( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 502;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY; // required by specification
    use = STAT_USE_GEOMETRY;
    hierarchy = STAT_HIER_ALL_SUB;

    return;
}


IGES_ENTITY_502::~IGES_ENTITY_502()
{
    vertices.clear();
    return;
}


bool IGES_ENTITY_502::Associate( std::vector<IGES_ENTITY*>* entities )
{
    return IGES_ENTITY::Associate( entities );
}


bool IGES_ENTITY_502::format( int &index )
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
    ostr << vertices.size() << pd;
    string fStr = ostr.str();
    string tStr;

    vector<IGES_POINT>::iterator sV = vertices.begin();
    vector<IGES_POINT>::iterator eV = --vertices.end();
    double vals[3];
    int acc = 0;

    while( sV != eV )
    {
        vals[0] = sV->x;
        vals[1] = sV->y;
        vals[2] = sV->z;

        for( int i = 0; i < 3; ++i )
        {
            if( !FormatPDREal( tStr, vals[i], pd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format Point[" << acc << "]\n";
                return false;
            }

            AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
        }

        ++acc;
        ++sV;
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

    vals[0] = sV->x;
    vals[1] = sV->y;
    vals[2] = sV->z;

    for( int i = 0; i < 3; ++i )
    {
        if( !FormatPDREal( tStr, vals[i], idelim, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format Point[" << acc << "]\n";
            return false;
        }

        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
    }

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


bool IGES_ENTITY_502::rescale( double sf )
{
    if( vertices.empty() )
        return true;

    vector<IGES_POINT>::iterator sV = vertices.begin();
    vector<IGES_POINT>::iterator eV = vertices.end();

    while( sV != eV )
    {
        *sV = (*sV) * sf;
        ++sV;
    }

    return true;
}


bool IGES_ENTITY_502::Unlink( IGES_ENTITY* aChildEntity )
{
    ERRMSG << "\n + [BUG] Unlink() invoked on Entity 502 (Vertex List)\n";
    return false;
}


bool IGES_ENTITY_502::IsOrphaned( void )
{
    if( refs.empty() || vertices.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_502::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_502::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_502::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored
    depends = STAT_DEP_PHY;         // required
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    lineWeightNum = 0;              // N.A.
    colorNum = 0;                   // N.A.

    if( form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Vertex List\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_502::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Vertex Entity\n";
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

    int nV;

    if( !ParseInt( pdout, idx, nV, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of vertices in the list\n";
        pdout.clear();
        return false;
    }

    if( nV < 1 )
    {
        ERRMSG << "\n + [INFO] invalid number of vertices: " << nV << "\n";
        pdout.clear();
        return false;
    }

    IGES_POINT p0;
    double* pp[3] = { &p0.x, &p0.y, &p0.z };

    for( int i = 0; i < nV; ++i )
    {
        for( int j = 0; j < 3; ++j )
        {
            if( !ParseReal( pdout, idx, *pp[j], eor, pd, rd ) )
            {
                ERRMSG << "\n + [BAD FILE] no datum for vertex " << i << "\n";
                pdout.clear();
                return false;
            }
        }

        vertices.push_back( p0 );
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

    if( parent->globalData.convert )
        rescale( parent->globalData.cf );

    return true;
}


bool IGES_ENTITY_502::SetEntityForm( int aForm )
{
    if( aForm == 1 )
        return true;

    ERRMSG << "\n + [BUG] Vertex Entity only supports Form 1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_502::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Vertex Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_502::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Vertex Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_502::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored by a Vertex Entity so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] Vertex Entity does not support hierarchy\n";
    return true;
}


std::vector<IGES_POINT>* IGES_ENTITY_502::GetVertices( void )
{
    return &vertices;
}


void IGES_ENTITY_502::AddVertex( IGES_POINT aPoint )
{
    vertices.push_back( aPoint );
    return;
}
