/*
 * file: dll_entity120.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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


#include <api/dll_entity120.h>
#include <api/dll_iges.h>
#include <core/entity120.h>
#include <core/iges_entity.h>
#include <core/iges.h>


DLL_IGES_ENTITY_120::DLL_IGES_ENTITY_120( IGES* aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_SURFACE_OF_REVOLUTION;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_SURFACE_OF_REVOLUTION, &m_entity );
        else
            m_entity = new IGES_ENTITY_120( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_120::DLL_IGES_ENTITY_120( DLL_IGES& aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_SURFACE_OF_REVOLUTION;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_SURFACE_OF_REVOLUTION, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_120::~DLL_IGES_ENTITY_120()
{
    return;
}


bool DLL_IGES_ENTITY_120::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_SURFACE_OF_REVOLUTION, &m_entity );
    else
        m_entity = new IGES_ENTITY_120( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag(&m_valid);
        return true;
    }

    return false;
}


bool DLL_IGES_ENTITY_120::GetAxis( IGES_CURVE** aCurve )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_120*)m_entity)->GetAxis( aCurve );
}


bool DLL_IGES_ENTITY_120::SetAxis( IGES_CURVE* aCurve )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_120*)m_entity)->SetAxis( aCurve );
}


bool DLL_IGES_ENTITY_120::SetAxis( DLL_IGES_CURVE& aCurve )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_120*)m_entity)->SetAxis( (IGES_CURVE*) aCurve.GetRawPtr() );
}


bool DLL_IGES_ENTITY_120::GetGeneratrix( IGES_CURVE** aCurve )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_120*)m_entity)->GetGeneratrix( aCurve );
}


bool DLL_IGES_ENTITY_120::SetGeneratrix( IGES_CURVE* aCurve )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_120*)m_entity)->SetGeneratrix( aCurve );
}


bool DLL_IGES_ENTITY_120::SetGeneratrix( DLL_IGES_CURVE& aCurve )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_120*)m_entity)->SetGeneratrix( (IGES_CURVE*) aCurve.GetRawPtr() );
}


bool DLL_IGES_ENTITY_120::GetAngles( double& aStartAngle, double& aEndAngle )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aStartAngle = ((IGES_ENTITY_120*)m_entity)->startAngle;
    aEndAngle = ((IGES_ENTITY_120*)m_entity)->endAngle;
    return true;
}


bool DLL_IGES_ENTITY_120::SetAngles( double aStartAngle, double aEndAngle )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_120*)m_entity)->startAngle = aStartAngle;
    ((IGES_ENTITY_120*)m_entity)->endAngle = aEndAngle;
    return true;
}
