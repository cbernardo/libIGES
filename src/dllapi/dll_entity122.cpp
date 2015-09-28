/*
 * file: dll_entity122.cpp
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

#include <dll_entity122.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity122.h>

DLL_IGES_ENTITY_122::DLL_IGES_ENTITY_122( IGES* aParent, bool create )
{
    m_type = ENT_TABULATED_CYLINDER;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_TABULATED_CYLINDER, &m_entity );
        else
            m_entity = new IGES_ENTITY_122( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_122::DLL_IGES_ENTITY_122( DLL_IGES& aParent, bool create )
{
    m_type = ENT_TABULATED_CYLINDER;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_TABULATED_CYLINDER, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_122::~DLL_IGES_ENTITY_122()
{
    return;
}


bool DLL_IGES_ENTITY_122::GetGeneratrixEnd( MCAD_POINT& pt )
{
    if( !m_valid || NULL == m_entity )
        return false;

    pt.x = ((IGES_ENTITY_122*)m_entity)->LX;
    pt.y = ((IGES_ENTITY_122*)m_entity)->LY;
    pt.z = ((IGES_ENTITY_122*)m_entity)->LZ;
    return true;
}


bool DLL_IGES_ENTITY_122::GetGeneratrixEnd( double& aX, double& aY, double& aZ )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aX = ((IGES_ENTITY_122*)m_entity)->LX;
    aY = ((IGES_ENTITY_122*)m_entity)->LY;
    aZ = ((IGES_ENTITY_122*)m_entity)->LZ;
    return true;
}


bool DLL_IGES_ENTITY_122::GetDirectrix( IGES_CURVE*& aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_122*)m_entity)->GetDE( aPtr );
}


bool DLL_IGES_ENTITY_122::SetGeneratrixEnd( MCAD_POINT pt )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_122*)m_entity)->LX = pt.x;
    ((IGES_ENTITY_122*)m_entity)->LX = pt.y;
    ((IGES_ENTITY_122*)m_entity)->LX = pt.z;
    return true;
}


bool DLL_IGES_ENTITY_122::SetGeneratrixEnd( double aX, double aY, double aZ )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_122*)m_entity)->LX = aX;
    ((IGES_ENTITY_122*)m_entity)->LX = aY;
    ((IGES_ENTITY_122*)m_entity)->LX = aZ;
    return true;
}


bool DLL_IGES_ENTITY_122::SetDirectrix( IGES_CURVE* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_122*)m_entity)->SetDE( aPtr );
}
