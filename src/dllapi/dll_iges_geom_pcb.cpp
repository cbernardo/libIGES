/*
 * file: dll_iges_geom_pcb.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: This class represents an outline which consists of
 * a list of segments. Once the segments form a closed loop no
 * more segments may be added and the internal segments are arranged
 * in a counterclockwise fashion.
 *
 * This file is part of libIGES.
 *
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <api/dll_mcad_outline.h>
#include <api/dll_iges_geom_pcb.h>
#include <geom/mcad_outline.h>
#include <geom/mcad_segment.h>
#include <geom/iges_geom_pcb.h>
#include <core/iges_curve.h>
#include <core/entity126.h>
#include <core/entity144.h>
#include <error_macros.h>

DLL_IGES_GEOM_PCB::DLL_IGES_GEOM_PCB( bool create ) : DLL_MCAD_OUTLINE( false )
{
    if( create )
        NewOutline();

    return;
}


DLL_IGES_GEOM_PCB::~DLL_IGES_GEOM_PCB()
{
    return;
}


bool DLL_IGES_GEOM_PCB::NewOutline( void )
{
    if( m_valid && m_outline )
        delete m_outline;

    m_outline = NULL;
    m_valid = false;

    m_outline = new IGES_GEOM_PCB;

    if( NULL != m_outline )
    {
        m_outline->AttachValidFlag( &m_valid );
        return true;
    }

    return false;
}


bool DLL_IGES_GEOM_PCB::Attach( MCAD_OUTLINE* aOutline )
{
    if( ( NULL != m_outline && m_valid ) || NULL == aOutline )
        return false;

    if( MCAD_OT_PCB != aOutline->GetOutlineType() )
    {
        ERRMSG << "\n + [INFO] wrong outline type: " << aOutline->GetOutlineType() << "\n";
        return false;
    }

    m_outline = aOutline;
    m_outline->AttachValidFlag( &m_valid );

    return true;
}


bool DLL_IGES_GEOM_PCB::GetVerticalSurface( IGES* aModel, bool& error,
    IGES_ENTITY_144**& aSurfaceList,
    int& nSurfaces, double aTopZ, double aBotZ )
{
    if( NULL == m_outline || !m_valid )
        return false;

    std::vector< IGES_ENTITY_144* > surfs;

    bool ok = ((IGES_GEOM_PCB*)m_outline)->GetVerticalSurface( aModel, error,
        surfs, aTopZ, aBotZ );

    if( ok && !surfs.empty() )
    {
        IGES_ENTITY_144** pl = NULL;
        int ts = (int)surfs.size();
        int i = 0;

        if( nSurfaces > 0 && NULL != aSurfaceList )
        {
            ts += nSurfaces;
            pl = new IGES_ENTITY_144*[ts];

            for( i = 0; i < nSurfaces; ++i )
                pl[i] = aSurfaceList[i];

            delete [] aSurfaceList;
        }
        else
        {
            pl = new IGES_ENTITY_144*[ts];
        }

        nSurfaces = ts;
        aSurfaceList = pl;
        std::vector< IGES_ENTITY_144* >::iterator sL = surfs.begin();
        std::vector< IGES_ENTITY_144* >::iterator eL = surfs.end();

        while( sL != eL )
        {
            pl[i] = *sL;
            ++i;
            ++sL;
        }

        return true;
    }

    return false;
}


bool DLL_IGES_GEOM_PCB::GetTrimmedPlane( IGES* aModel, bool& error,
    IGES_ENTITY_144**& aSurfaceList,
    int& nSurfaces, double aHeight )
{
    if( NULL == m_outline || !m_valid )
        return false;

    std::vector< IGES_ENTITY_144* > surfs;

    bool ok = ((IGES_GEOM_PCB*)m_outline)->GetTrimmedPlane( aModel, error,
    surfs, aHeight );

    if( ok && !surfs.empty() )
    {
        IGES_ENTITY_144** pl = NULL;
        int ts = (int)surfs.size();
        int i = 0;

        if( nSurfaces > 0 && NULL != aSurfaceList )
        {
            ts += nSurfaces;
            pl = new IGES_ENTITY_144*[ts];

            for( i = 0; i < nSurfaces; ++i )
                pl[i] = aSurfaceList[i];

            delete [] aSurfaceList;
        }
        else
        {
            pl = new IGES_ENTITY_144*[ts];
        }

        nSurfaces = ts;
        aSurfaceList = pl;
        std::vector< IGES_ENTITY_144* >::iterator sL = surfs.begin();
        std::vector< IGES_ENTITY_144* >::iterator eL = surfs.end();

        while( sL != eL )
        {
            pl[i] = *sL;
            ++i;
            ++sL;
        }

        return true;
    }

    return false;
}


bool DLL_IGES_GEOM_PCB::GetCurves( IGES* aModel, IGES_CURVE**& aCurveList, int& nCurves,
    double zHeight, MCAD_SEGMENT* aSegment )
{
    if( NULL == m_outline || !m_valid )
        return false;

    std::list< IGES_CURVE* > curves;

    bool ok = ((IGES_GEOM_PCB*)m_outline)->GetCurves( aModel, curves, zHeight, aSegment );

    if( ok && !curves.empty() )
    {
        IGES_CURVE** pl = NULL;
        int ts = (int)curves.size();
        int i = 0;

        if( nCurves > 0 && NULL != aCurveList )
        {
            ts += nCurves;
            pl = new IGES_CURVE*[ts];

            for( i = 0; i < nCurves; ++i )
                pl[i] = aCurveList[i];

            delete [] aCurveList;
        }
        else
        {
            pl = new IGES_CURVE*[ts];
        }

        nCurves = ts;
        aCurveList = pl;
        std::list< IGES_CURVE* >::iterator sC = curves.begin();
        std::list< IGES_CURVE* >::iterator eC = curves.begin();

        while( sC != eC )
        {
            pl[i] = *sC;
            ++i;
            ++sC;
        }

        return true;
    }

    return false;
}


bool DLL_IGES_GEOM_PCB::GetCurveOnPlane(  IGES* aModel,
    IGES_ENTITY_126**& aCurveList, int& nCurves,
    double aMinX, double aMaxX, double aMinY, double aMaxY,
    double zHeight, MCAD_SEGMENT* aSegment )
{
    if( NULL == m_outline && !m_valid )
        return false;

    std::list< IGES_ENTITY_126* > curves;

    bool ok = ((IGES_GEOM_PCB*)m_outline)->GetCurveOnPlane( aModel, curves, aMinX, aMaxX,
        aMinY, aMaxY, zHeight, aSegment );

    if( ok && !curves.empty() )
    {
        IGES_ENTITY_126** pl = NULL;
        int ts = (int)curves.size();
        int i = 0;

        if( nCurves > 0 && NULL != aCurveList )
        {
            ts+= nCurves;
            pl = new IGES_ENTITY_126*[ts];

            for( i = 0; i < nCurves; ++i )
                pl[i] = aCurveList[i];

            delete [] aCurveList;
        }
        else
        {
            pl = new IGES_ENTITY_126*[ts];
        }

        nCurves = ts;
        aCurveList = pl;
        std::list< IGES_ENTITY_126* >::iterator sC = curves.begin();
        std::list< IGES_ENTITY_126* >::iterator eC = curves.begin();

        while( sC != eC )
        {
            pl[i] = *sC;
            ++i;
            ++sC;
        }

        return true;
    }

    return false;
}


bool DLL_IGES_GEOM_PCB::GetSegmentWall( IGES* aModel,
    IGES_ENTITY_144**& aSurfaceList, int& nSurfaces,
    double aTopZ, double aBotZ, MCAD_SEGMENT* aSegment )
{
    if( NULL == m_outline || m_valid )
        return false;

    std::vector< IGES_ENTITY_144* > surfs;

    bool ok = ((IGES_GEOM_PCB*)m_outline)->GetSegmentWall( aModel,
        surfs, aTopZ, aBotZ, aSegment );

    if( ok && !surfs.empty() )
    {
        IGES_ENTITY_144** pl = NULL;
        int ts = (int)surfs.size();
        int i = 0;

        if( nSurfaces > 0 && NULL != aSurfaceList )
        {
            ts += nSurfaces;
            pl = new IGES_ENTITY_144*[ts];

            for( i = 0; i < nSurfaces; ++i )
                pl[i] = aSurfaceList[i];

            delete [] aSurfaceList;
        }
        else
        {
            pl = new IGES_ENTITY_144*[ts];
        }

        nSurfaces = ts;
        aSurfaceList = pl;
        std::vector< IGES_ENTITY_144* >::iterator sL = surfs.begin();
        std::vector< IGES_ENTITY_144* >::iterator eL = surfs.end();

        while( sL != eL )
        {
            pl[i] = *sL;
            ++i;
            ++sL;
        }

        return true;
    }

    return false;
}
