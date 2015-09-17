/*
 * file: entity100.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 100: Circle, Section 4.3, p.66+ (94+)
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
#include <entity100.h>
#include <entity124.h>

using namespace std;


IGES_ENTITY_100::IGES_ENTITY_100( IGES* aParent ) : IGES_CURVE( aParent )
{
    entityType = 100;
    form = 0;

    zOffset = 0.0;
    xCenter = 0.0;
    yCenter = 0.0;
    xStart = 1.0;
    yStart = 0.0;
    xEnd = 1.0;
    yEnd = 0.0;

    radius = 0.0;
    startAng = 0.0;
    endAng = 0.0;

    return;
}   // IGES_ENTITY_100( IGES* aParent )


IGES_ENTITY_100::~IGES_ENTITY_100()
{
    return;
}   // ~IGES_ENTITY_100()


bool IGES_ENTITY_100::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->delReference(this);
        pStructure = NULL;
    }

    return true;
}


bool IGES_ENTITY_100::format( int &index )
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
    string fStr = ostr.str();
    string tStr;

    if( !FormatPDREal( tStr, zOffset, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format zOffset\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, xCenter, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format xCenter\n";
        pdout.clear();
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, yCenter, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format yCenter\n";
        pdout.clear();
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, xStart, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format xStart\n";
        pdout.clear();
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, yStart, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format yStart\n";
        pdout.clear();
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, xEnd, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format xEnd\n";
        pdout.clear();
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    // note: 2 sets of OPTIONAL parameters may exist at the end of
    // any PD; see p.32/60+ for details; if optional parameters
    // need to be written then we should use 'pd' rather than 'rd'
    // in this call to FormatPDREal()
    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    if( !FormatPDREal( tStr, yEnd, idelim, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format yEnd\n";
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


bool IGES_ENTITY_100::unlink(IGES_ENTITY *aChild)
{
    return IGES_ENTITY::unlink(aChild);
}


bool IGES_ENTITY_100::isOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_100::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_100::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_100::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Circle\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_100::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Circle Entity\n";
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

    if( !ParseReal( pdout, idx, zOffset, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no zOffset datum for Circle Entity\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, xCenter, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no xCenter datum for Circle Entity\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, yCenter, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no yCenter datum for Circle Entity\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, xStart, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no xStart datum for Circle Entity\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, yStart, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no yStart datum for Circle Entity\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, xEnd, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no xEnd datum for Circle Entity\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, yEnd, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no yEnd datum for Circle Entity\n";
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

    // NOTE:
    // Ideally we would check that the radii are non-zero and equal;
    // however if we discover that some MCAD software produces
    // coordinates which cannot meet these criteria so we must assume
    // that everything is OK
    MCAD_POINT p0( xCenter, yCenter, 0.0 );
    MCAD_POINT p1( xStart, yStart, 0.0 );
    MCAD_POINT p2( xEnd, yEnd, 0.0 );
    // note: relax the resolution criteria to allow more permissive
    // acceptance of files
    double uir = 2.0 * parent->globalData.minResolution;

    if( PointMatches( p0, p1, uir ) )
    {
        ERRMSG << "\n + [BAD FILE] Circle (DE " << sequenceNumber;
        cerr << ") has a radius of 0\n";
        return false;
    }

    p1 = p1 - p0;
    p2 = p2 - p0;

    double d1 = p1.x*p1.x + p1.y*p1.y;
    double d2 = p2.x*p2.x + p2.y*p2.y;
    d1 = d2 - d1;

    if( abs( d1 ) > uir )
    {
        ERRMSG << "\n + [BAD FILE] Circle (DE " << sequenceNumber;
        cerr << ") has unequal radii (diff: " << d1 << ") " << uir << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_100::SetEntityForm( int aForm )
{
    if( aForm == 0 )
        return true;

    ERRMSG << "\n + [BUG] Circle Entity only supports Form 0 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_100::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // the hierarchy is ignored by a Circle Entity so this function always succeeds
    ERRMSG << "\n + [WARNING] [BUG] Circle Entity does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_100::rescale( double sf )
{
    zOffset *= sf;
    xCenter *= sf;
    yCenter *= sf;
    xStart *= sf;
    yStart *= sf;
    xEnd *= sf;
    yEnd *= sf;

    return true;
}


bool IGES_ENTITY_100::GetStartPoint( MCAD_POINT& pt, bool xform )
{
    pt.x = xStart;
    pt.y = yStart;
    pt.z = zOffset;

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


bool IGES_ENTITY_100::GetEndPoint( MCAD_POINT& pt, bool xform )
{
    pt.x = xEnd;
    pt.y = yEnd;
    pt.z = zOffset;

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


int IGES_ENTITY_100::GetNSegments( void )
{
    return 1;
}


bool IGES_ENTITY_100::IsClosed( void )
{
    MCAD_POINT p0( xCenter, yCenter, 0.0 );
    MCAD_POINT p1( xStart, yStart, 0.0 );
    MCAD_POINT p2( xEnd, yEnd, 0.0 );

    double rm = 0.001;

    if( parent )
        rm = parent->globalData.minResolution;

    // a zero-radius circle is not acceptable so
    // this function must fail
    if( PointMatches( p0, p1, rm ) )
    {
        return false;
    }

    return PointMatches( p1, p2, rm );
}


int IGES_ENTITY_100::GetNCurves( void )
{
    return 0;
}


IGES_CURVE* IGES_ENTITY_100::GetCurve( int index )
{
    return NULL;
}


bool IGES_ENTITY_100::Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform )
{
    if( 1 !=nSeg )
    {
        ERRMSG << "\n + [INFO] nSeg (" << nSeg << ") must be 1 for a circle entity\n";
        return false;
    }

    if( var < 0.0 || var > 1.0 )
    {
        ERRMSG << "\n + [INFO] var (" << var << ") must be in the range (0,1)\n";
        return false;
    }

    // if one of the endpoints is requested then re-establish the parameters
    double uir = 1e-6;

    if( parent )
        uir = parent->globalData.minResolution;

    if( var < uir || (1.0 - var) < uir )
    {
        bool fullCircle = false;
        MCAD_POINT p0( xStart, yStart, 0.0 );
        MCAD_POINT p1( xEnd, yEnd, 0.0 );

        if( PointMatches( p0, p1, uir ) )
            fullCircle = true;

        double dx = xStart - xCenter;
        double dy = yStart - yCenter;

        startAng = atan2( dy, dx );

        if( fullCircle )
        {
            endAng = startAng;
        }
        else
        {
            endAng = atan2( yEnd - yCenter, xEnd - xCenter );

            if( endAng < startAng )
                endAng += 2.0 * M_PI;

        }

        radius = sqrt( dx*dx + dy*dy );
    }

    double ang = (endAng - startAng) * var + startAng;
    double dx = xCenter + cos(ang) * radius;
    double dy = yCenter + sin(ang) * radius;

    MCAD_POINT pt0( dx, dy, zOffset );

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt0;
    else
        pt = pt0;

    return true;
}
