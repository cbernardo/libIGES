/*
 * file: dll_mcad_outline.cpp
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

#include <list>
#include <libigesconf.h>
#include <mcad_elements.h>
#include <dll_mcad_segment.h>
#include <dll_mcad_outline.h>
#include <mcad_segment.h>
#include <mcad_outline.h>
#include <error_macros.h>

DLL_MCAD_OUTLINE::DLL_MCAD_OUTLINE( bool create )
{
    m_valid = false;
    m_outline = NULL;

    if( create )
        NewOutline();

    return;
}


DLL_MCAD_OUTLINE::~DLL_MCAD_OUTLINE()
{
    if( NULL != m_outline && m_valid )
    {
        m_outline->DetachValidFlag( &m_valid );
        delete m_outline;
    }

    return;
}


bool DLL_MCAD_OUTLINE::IsValid( void )
{
    return m_valid;
}


bool DLL_MCAD_OUTLINE::NewOutline( void )
{
    if( m_valid && m_outline )
        delete m_outline;

    m_outline = NULL;
    m_valid = false;

    m_outline = new MCAD_OUTLINE;

    if( NULL != m_outline )
    {
        m_outline->AttachValidFlag( &m_valid );
        return true;
    }

    return false;
}


void DLL_MCAD_OUTLINE::DelOutline( void )
{
    if( NULL == m_outline || !m_valid )
        return;

    m_outline->DetachValidFlag( &m_valid );
    delete m_outline;
    m_outline = NULL;

    return;
}


void DLL_MCAD_OUTLINE::Detach( void )
{
    if( NULL != m_outline && m_valid )
        m_outline->DetachValidFlag( &m_valid );

    m_valid = false;
    m_outline = NULL;
    return;
}


bool DLL_MCAD_OUTLINE::Attach( MCAD_OUTLINE* aOutline )
{
    if( ( NULL != m_outline && m_valid ) || NULL == aOutline )
        return false;

    m_outline = aOutline;
    m_outline->AttachValidFlag( &m_valid );

    return true;
}


MCAD_OUTLINE* DLL_MCAD_OUTLINE::GetRawPtr()
{
    if( !m_valid )
        m_outline = NULL;

    return m_outline;
}


bool DLL_MCAD_OUTLINE::GetSegments( MCAD_SEGMENT**& aSegmentList, int& aListSize )
{
    aSegmentList = NULL;
    aListSize = 0;

    if( NULL == m_outline || !m_valid )
        return false;

    std::list<MCAD_SEGMENT*>* lp = m_outline->GetSegments();

    if( NULL == lp || 0 == lp->size() )
        return false;

    aListSize = (int)lp->size();
    aSegmentList = new MCAD_SEGMENT*[aListSize];
    std::list<MCAD_SEGMENT*>::iterator sL = lp->begin();
    std::list<MCAD_SEGMENT*>::iterator eL = lp->end();
    int i = 0;

    while( sL != eL )
    {
        aSegmentList[i] = *sL;
        ++i;
        ++sL;
    }

    return true;
}


bool DLL_MCAD_OUTLINE::GetCutouts( MCAD_OUTLINE**& aCutoutList, int& aListSize )
{
    aCutoutList = NULL;
    aListSize = 0;

    if( NULL == m_outline || !m_valid )
        return false;

    std::list<MCAD_OUTLINE*>* lp = m_outline->GetCutouts();

    if( NULL == lp || 0 == lp->size() )
        return false;

    aListSize = (int)lp->size();
    aCutoutList = new MCAD_OUTLINE*[aListSize];
    std::list<MCAD_OUTLINE*>::iterator sL = lp->begin();
    std::list<MCAD_OUTLINE*>::iterator eL = lp->end();
    int i = 0;

    while( sL != eL )
    {
        aCutoutList[i] = *sL;
        ++i;
        ++sL;
    }

    return true;
}


bool DLL_MCAD_OUTLINE::GetDrillHoles( MCAD_SEGMENT**& aDrillHoleList, int& aListSize )
{
    aDrillHoleList = NULL;
    aListSize = 0;

    if( NULL == m_outline || !m_valid )
        return false;

    std::list<MCAD_SEGMENT*>* lp = m_outline->GetDrillHoles();

    if( NULL == lp || 0 == lp->size() )
        return false;

    aListSize = (int)lp->size();
    aDrillHoleList = new MCAD_SEGMENT*[aListSize];
    std::list<MCAD_SEGMENT*>::iterator sL = lp->begin();
    std::list<MCAD_SEGMENT*>::iterator eL = lp->end();
    int i = 0;

    while( sL != eL )
    {
        aDrillHoleList[i] = *sL;
        ++i;
        ++sL;
    }

    return true;
}


bool DLL_MCAD_OUTLINE::GetErrors( char const**& anErrorList, int& aListSize )
{
    anErrorList = NULL;
    aListSize = 0;

    if( NULL == m_outline || !m_valid )
        return false;

    const std::list< std::string >* lp = m_outline->GetErrors();

    if( NULL == lp || 0 == lp->size() )
        return false;

    aListSize = (int)lp->size();
    anErrorList = new char const*[aListSize];
    std::list< std::string >::const_iterator sL = lp->begin();
    std::list< std::string >::const_iterator eL = lp->end();
    int i = 0;

    while( sL != eL )
    {
        anErrorList[i] = sL->c_str();
        ++i;
        ++sL;
    }

    return true;
}


void DLL_MCAD_OUTLINE::ClearErrors( void )
{
    if( NULL == m_outline || !m_valid )
        return;

    m_outline->ClearErrors();
    return;
}


bool DLL_MCAD_OUTLINE::IsClosed( bool& aResult )
{
    if( NULL == m_outline || !m_valid )
        return false;

    aResult = m_outline->IsClosed();
    return true;
}


bool DLL_MCAD_OUTLINE::IsContiguous( bool& aResult )
{
    if( NULL == m_outline || !m_valid )
        return false;

    aResult = m_outline->IsContiguous();
    return true;
}


bool DLL_MCAD_OUTLINE::IsInside( MCAD_POINT aPoint, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->IsInside( aPoint, error );
}


bool DLL_MCAD_OUTLINE::AddSegment( MCAD_SEGMENT* aSegment, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->AddSegment( aSegment, error );
}


bool DLL_MCAD_OUTLINE::AddSegment( DLL_MCAD_SEGMENT& aSegment, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_SEGMENT* sp = aSegment.GetRawPtr();

    if( NULL != sp && m_outline->AddSegment( sp, error ) )
    {
        aSegment.Detach();
        return true;
    }

    return false;
}


bool DLL_MCAD_OUTLINE::AddOutline( MCAD_OUTLINE* aOutline, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->AddOutline( aOutline, error );
}


bool DLL_MCAD_OUTLINE::AddOutline( DLL_MCAD_OUTLINE& aOutline, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_OUTLINE* op = aOutline.GetRawPtr();

    if( NULL != op && m_outline->AddOutline( op, error ) )
    {
        aOutline.Detach();
        return true;
    }

    return false;
}


bool DLL_MCAD_OUTLINE::AddOutline( MCAD_SEGMENT* aCircle, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->AddOutline( aCircle, error );
}


bool DLL_MCAD_OUTLINE::AddOutline( DLL_MCAD_SEGMENT& aCircle, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_SEGMENT* sp = aCircle.GetRawPtr();

    if( NULL != sp && m_outline->AddOutline( sp, error ) )
    {
        aCircle.Detach();
        return true;
    }

    return false;
}


bool DLL_MCAD_OUTLINE::SubOutline( MCAD_OUTLINE* aOutline, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->SubOutline( aOutline, error );
}


bool DLL_MCAD_OUTLINE::SubOutline( DLL_MCAD_OUTLINE& aOutline, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_OUTLINE* op = aOutline.GetRawPtr();

    if( NULL != op && m_outline->SubOutline( op, error ) )
    {
        aOutline.Detach();
        return true;
    }

    return false;
}


bool DLL_MCAD_OUTLINE::SubOutline( MCAD_SEGMENT* aCircle, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->SubOutline( aCircle, error );
}


bool DLL_MCAD_OUTLINE::SubOutline( DLL_MCAD_SEGMENT& aCircle, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_SEGMENT* sp = aCircle.GetRawPtr();

    if( NULL != sp && m_outline->SubOutline( sp, error ) )
    {
        aCircle.Detach();
        return true;
    }

    return false;
}


bool DLL_MCAD_OUTLINE::AddCutout( MCAD_OUTLINE* aCutout, bool overlaps, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->AddCutout( aCutout, overlaps, error );
}


bool DLL_MCAD_OUTLINE::AddCutout( DLL_MCAD_OUTLINE& aCutout, bool overlaps, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_OUTLINE* op = aCutout.GetRawPtr();

    if( NULL != op && m_outline->AddCutout( op, overlaps, error ) )
    {
        aCutout.Detach();
        return true;
    }

    return false;
}


bool DLL_MCAD_OUTLINE::AddCutout( MCAD_SEGMENT* aCircle, bool overlaps, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    return m_outline->AddCutout( aCircle, overlaps, error );
}


bool DLL_MCAD_OUTLINE::AddCutout( DLL_MCAD_SEGMENT& aCircle, bool overlaps, bool& error )
{
    if( NULL == m_outline || !m_valid )
        return false;

    MCAD_SEGMENT* sp = aCircle.GetRawPtr();

    if( NULL != sp && m_outline->AddCutout( sp, overlaps, error ) )
    {
        aCircle.Detach();
        return true;
    }

    return false;
}
