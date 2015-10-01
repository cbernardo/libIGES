/*
 * file: dll_entity144.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 144: Trimmed Parametric Surface, Section 4.34, p.181 (209+)
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


#include <dll_entity144.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity144.h>
#include <entity142.h>


DLL_IGES_ENTITY_144::DLL_IGES_ENTITY_144( IGES* aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_TRIMMED_PARAMETRIC_SURFACE;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_TRIMMED_PARAMETRIC_SURFACE, &m_entity );
        else
            m_entity = new IGES_ENTITY_144( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_144::DLL_IGES_ENTITY_144( DLL_IGES& aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_TRIMMED_PARAMETRIC_SURFACE;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_TRIMMED_PARAMETRIC_SURFACE, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_144::~DLL_IGES_ENTITY_144()
{
    return;
}


bool DLL_IGES_ENTITY_144::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_TRIMMED_PARAMETRIC_SURFACE, &m_entity );
    else
        m_entity = new IGES_ENTITY_144( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag(&m_valid);
        return true;
    }

    return false;
}


bool DLL_IGES_ENTITY_144::GetSurface( IGES_ENTITY*& aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->GetPTS( aPtr );
}


bool DLL_IGES_ENTITY_144::SetSurface( IGES_ENTITY* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->SetPTS( aPtr );
}


bool DLL_IGES_ENTITY_144::GetBoundCurve( IGES_ENTITY_142*& aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->GetPTO( aPtr );
}


bool DLL_IGES_ENTITY_144::SetBoundCurve( IGES_ENTITY_142* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->SetPTO( aPtr );
}


bool DLL_IGES_ENTITY_144::GetNCutouts( int& aNCutouts )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aNCutouts = ((IGES_ENTITY_144*)m_entity)->GetNPTI();
    return true;
}


bool DLL_IGES_ENTITY_144::GetCutouts( size_t& aListSize, IGES_ENTITY_142**& aCutoutList )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->GetPTIList( aListSize, aCutoutList );
}


bool DLL_IGES_ENTITY_144::AddCutout( IGES_ENTITY_142* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->AddPTI( aPtr );
}


bool DLL_IGES_ENTITY_144::DelCutout( IGES_ENTITY_142* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_144*)m_entity)->DelPTI( aPtr );
}
