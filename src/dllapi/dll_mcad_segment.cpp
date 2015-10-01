/*
 * file: dll_mcad_segment.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: intermediate ECAD representation of a boundary
 * segment. This class aids in the creation of an IGES model
 * for the top and bottom surfaces of a PCB.
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


#include <list>
#include <libigesconf.h>
#include <mcad_elements.h>
#include <dll_mcad_segment.h>
#include <mcad_segment.h>
#include <error_macros.h>


DLL_MCAD_SEGMENT::DLL_MCAD_SEGMENT( bool create )
{
    m_valid = false;
    m_segment = NULL;

    if( create )
        NewSegment();

    return;
}


DLL_MCAD_SEGMENT::~DLL_MCAD_SEGMENT()
{
    if( NULL != m_segment && m_valid )
    {
        m_segment->DetachValidFlag( &m_valid );
        delete m_segment;
    }

    return;
}


bool DLL_MCAD_SEGMENT::IsValid( void )
{
    return m_valid;
}


bool DLL_MCAD_SEGMENT::NewSegment( void )
{
    if( m_valid && m_segment )
        delete m_segment;

    m_segment = NULL;
    m_valid = false;

    m_segment = new MCAD_SEGMENT;

    if( NULL != m_segment )
    {
        m_segment->AttachValidFlag( &m_valid );
        return true;
    }

    return false;
}


void DLL_MCAD_SEGMENT::DelSegment( void )
{
    if( NULL == m_segment || !m_valid )
        return;

    m_segment->DetachValidFlag( &m_valid );
    delete m_segment;
    m_segment = NULL;

    return;
}


void DLL_MCAD_SEGMENT::Detach( void )
{
    if( NULL != m_segment && m_valid )
        m_segment->DetachValidFlag( &m_valid );

    m_valid = false;
    m_segment = NULL;
    return;
}


bool DLL_MCAD_SEGMENT::Attach( MCAD_SEGMENT* aSegment )
{
    if( NULL != m_segment && m_valid )
        return false;

    if( NULL == aSegment )
        return false;

    m_segment = aSegment;
    m_segment->AttachValidFlag( &m_valid );

    return true;
}


MCAD_SEGMENT* DLL_MCAD_SEGMENT::GetRawPtr()
{
    if( !m_valid )
        m_segment = NULL;

    return m_segment;
}


bool DLL_MCAD_SEGMENT::GetSegType( MCAD_SEGTYPE& aSegType ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aSegType = m_segment->GetSegType();
    return true;
}


bool DLL_MCAD_SEGMENT::GetRadius( double& aRadius ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aRadius = m_segment->GetRadius();
    return true;
}


bool DLL_MCAD_SEGMENT::GetStartAngle( double& aStartAngle ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aStartAngle = m_segment->GetStartAngle();
    return true;
}


bool DLL_MCAD_SEGMENT::GetEndAngle( double& aEndAngle ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aEndAngle = m_segment->GetEndAngle();
    return true;
}


bool DLL_MCAD_SEGMENT::GetFirstAngle( double& aFirstAngle ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aFirstAngle = m_segment->GetMSAngle();
    return true;
}


bool DLL_MCAD_SEGMENT::GetLastAngle( double& aLastAngle ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aLastAngle = m_segment->GetMEAngle();
    return true;
}


bool DLL_MCAD_SEGMENT::GetCenter( MCAD_POINT& aPoint ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aPoint = m_segment->GetCenter();
    return true;
}


bool DLL_MCAD_SEGMENT::GetStart( MCAD_POINT& aPoint ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aPoint = m_segment->GetStart();
    return true;
}


bool DLL_MCAD_SEGMENT::GetEnd( MCAD_POINT& aPoint ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aPoint = m_segment->GetEnd();
    return true;
}


bool DLL_MCAD_SEGMENT::GetFirstPoint( MCAD_POINT& aPoint ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aPoint = m_segment->GetMStart();
    return true;
}


bool DLL_MCAD_SEGMENT::GetLastPoint( MCAD_POINT& aPoint ) const
{
    if( NULL == m_segment || !m_valid )
        return false;

    aPoint = m_segment->GetMEnd();
    return true;
}


bool DLL_MCAD_SEGMENT::SetParams( MCAD_POINT aStart, MCAD_POINT aEnd )
{
    if( ( NULL == m_segment || !m_valid ) && !NewSegment() )
        return false;

    return m_segment->SetParams( aStart, aEnd );
}


bool DLL_MCAD_SEGMENT::SetParams( MCAD_POINT aCenter, MCAD_POINT aStart,
    MCAD_POINT aEnd, bool isCW )
{
    if( ( NULL == m_segment || !m_valid ) && !NewSegment() )
        return false;

    return m_segment->SetParams( aCenter, aStart, aEnd, isCW );
}


bool DLL_MCAD_SEGMENT::GetLength( double& aLength )
{
    if( NULL == m_segment || !m_valid )
        return false;

    aLength = m_segment->GetLength();
    return true;
}


bool DLL_MCAD_SEGMENT::IsCW( bool& aResult )
{
    if( NULL == m_segment || !m_valid )
    {
        aResult = false;
        return false;
    }

    aResult = m_segment->IsCW();
    return true;
}


bool DLL_MCAD_SEGMENT::GetIntersections( MCAD_SEGMENT const* aSegment,
    MCAD_POINT*& aIntersectList, int& aNumIntersections, MCAD_INTERSECT_FLAG& flags )
{
    flags = MCAD_IFLAG_NONE;

    if( NULL == m_segment || !m_valid )
    {
        ERRMSG << "\n + [BUG] invalid segment\n";
        return false;
    }

    if( NULL != aIntersectList || 0 != aNumIntersections )
    {
        ERRMSG << "\n + [BUG] aIntersectList is not NULL or aNumIntersections is not 0\n";
        return false;
    }

    if( NULL == aSegment )
    {
        ERRMSG << "\n + [BUG] invoked with NULL pointer for aSegment\n";
        return false;
    }

    std::list<MCAD_POINT> ilist;

    if( !m_segment->GetIntersections( *aSegment, ilist, flags ) )
        return false;

    aNumIntersections = (int)ilist.size();
    aIntersectList = new MCAD_POINT[aNumIntersections];
    std::list<MCAD_POINT>::iterator sL = ilist.begin();
    std::list<MCAD_POINT>::iterator eL = ilist.end();
    int idx = 0;

    while( sL != eL )
    {
        aIntersectList[idx++] = *sL;
        ++sL;
    }

    return true;
}


bool DLL_MCAD_SEGMENT::GetIntersections( DLL_MCAD_SEGMENT& aSegment,
    MCAD_POINT*& aIntersectList, int& aNumIntersections, MCAD_INTERSECT_FLAG& flags )
{
    flags = MCAD_IFLAG_NONE;

    if( NULL == m_segment || !m_valid )
    {
        ERRMSG << "\n + [BUG] invalid segment\n";
        return false;
    }

    if( NULL != aIntersectList || 0 != aNumIntersections || NULL == aSegment.GetRawPtr() )
    {
        ERRMSG << "\n + [BUG] aIntersectList is not NULL or aNumIntersections is not 0\n";
        return false;
    }

    std::list<MCAD_POINT> ilist;

    if( !m_segment->GetIntersections( *aSegment.GetRawPtr(), ilist, flags ) )
        return false;

    aNumIntersections = (int)ilist.size();
    aIntersectList = new MCAD_POINT[aNumIntersections];
    std::list<MCAD_POINT>::iterator sL = ilist.begin();
    std::list<MCAD_POINT>::iterator eL = ilist.end();
    int idx = 0;

    while( sL != eL )
    {
        aIntersectList[idx++] = *sL;
        ++sL;
    }

    return true;
}


bool DLL_MCAD_SEGMENT::GetBoundingBox( MCAD_POINT& p0, MCAD_POINT& p1 )
{
    if( NULL == m_segment || !m_valid )
        return false;

    return m_segment->GetBoundingBox( p0, p1 );
}


bool DLL_MCAD_SEGMENT::GetMidpoint( MCAD_POINT& p0 )
{
    if( NULL == m_segment || !m_valid )
        return false;

    return m_segment->GetMidpoint( p0 );
}

bool DLL_MCAD_SEGMENT::Split( MCAD_POINT** aIntersectList, int aNumIntersects,
    MCAD_SEGMENT**& aNewSegmentList, int& aNumNewSegs )
{
    if( NULL == m_segment || !m_valid )
        return false;

    if( aNumIntersects == 0 )
        return true;

    if( NULL == aIntersectList || 0 >= aNumIntersects )
    {
        ERRMSG << "\n + [BUG] aIntersectList is NULL or aNumIntersections is <0\n";
        return false;
    }

    std::list<MCAD_POINT> ilist;

    for( int i = 0; i < aNumIntersects; ++i )
        ilist.push_back( *aIntersectList[i] );

    std::list<MCAD_SEGMENT*> slist;

    if( !m_segment->Split( ilist, slist ) )
        return false;

    int idx = 0;
    MCAD_SEGMENT** sp = NULL;

    if( aNumNewSegs > 0 && NULL != aNewSegmentList )
    {
        sp = new MCAD_SEGMENT*[ aNumNewSegs + slist.size() ];

        for( idx = 0; idx < aNumNewSegs; ++idx )
            sp[idx] = aNewSegmentList[idx];

        delete [] aNewSegmentList;
    }
    else
    {
        sp = new MCAD_SEGMENT*[ slist.size() ];
    }

    int j = (int) slist.size();
    std::list<MCAD_SEGMENT*>::iterator sL = slist.begin();

    for( int i = 0; i < j; ++i )
    {
        sp[idx++] = *sL;
        ++sL;
    }

    aNewSegmentList = sp;
    aNumNewSegs += (int)slist.size();
    return true;
}
