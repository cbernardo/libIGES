/*
 * file: dll_entity104.cpp
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

#include <api/dll_iges.h>
#include <api/dll_entity104.h>
#include <core/iges.h>
#include <core/entity104.h>


DLL_IGES_ENTITY_104::DLL_IGES_ENTITY_104( IGES* aParent, bool create ) : DLL_IGES_CURVE( aParent )
{
    m_type = ENT_CONIC_ARC;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_CONIC_ARC, &m_entity );
        else
            m_entity = new IGES_ENTITY_104( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_104::DLL_IGES_ENTITY_104( DLL_IGES& aParent, bool create ) : DLL_IGES_CURVE( aParent )
{
    m_type = ENT_CONIC_ARC;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_CONIC_ARC, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_104::~DLL_IGES_ENTITY_104()
{
    return;
}


bool DLL_IGES_ENTITY_104::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_CONIC_ARC, &m_entity );
    else
        m_entity = new IGES_ENTITY_104( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag(&m_valid);
        return true;
    }

    return false;
}


bool DLL_IGES_ENTITY_104::GetConicStart( MCAD_POINT& pt )
{
    if( !m_valid || NULL == m_entity )
        return false;

    pt.x = ((IGES_ENTITY_104*)m_entity)->X1;
    pt.y = ((IGES_ENTITY_104*)m_entity)->Y1;
    pt.z = ((IGES_ENTITY_104*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_104::GetConicStart( double& aX, double& aY, double& aZ )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aX = ((IGES_ENTITY_104*)m_entity)->X1;
    aY = ((IGES_ENTITY_104*)m_entity)->Y1;
    aZ = ((IGES_ENTITY_104*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_104::GetConicEnd( MCAD_POINT& pt )
{
    if( !m_valid || NULL == m_entity )
        return false;

    pt.x = ((IGES_ENTITY_104*)m_entity)->X2;
    pt.y = ((IGES_ENTITY_104*)m_entity)->Y2;
    pt.z = ((IGES_ENTITY_104*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_104::GetConicEnd( double& aX, double& aY, double& aZ )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aX = ((IGES_ENTITY_104*)m_entity)->X2;
    aY = ((IGES_ENTITY_104*)m_entity)->Y2;
    aZ = ((IGES_ENTITY_104*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_104::GetConicParams( double& P1, double& P2, double& P3,
    double& P4, double& P5, double& P6 )
{
    if( !m_valid || NULL == m_entity )
        return false;

    P1 = ((IGES_ENTITY_104*)m_entity)->A;
    P2 = ((IGES_ENTITY_104*)m_entity)->B;
    P3 = ((IGES_ENTITY_104*)m_entity)->C;
    P4 = ((IGES_ENTITY_104*)m_entity)->D;
    P5 = ((IGES_ENTITY_104*)m_entity)->E;
    P6 = ((IGES_ENTITY_104*)m_entity)->F;
    return true;
}


bool DLL_IGES_ENTITY_104::SetConicStart( MCAD_POINT pt )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_104*)m_entity)->X1 = pt.x;
    ((IGES_ENTITY_104*)m_entity)->Y1 = pt.y;
    ((IGES_ENTITY_104*)m_entity)->ZT = pt.z;
    return true;
}


bool DLL_IGES_ENTITY_104::SetConicStart( double aX, double aY, double aZ )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_104*)m_entity)->X1 = aX;
    ((IGES_ENTITY_104*)m_entity)->Y1 = aY;
    ((IGES_ENTITY_104*)m_entity)->ZT = aZ;
    return true;
}


bool DLL_IGES_ENTITY_104::SetConicEnd( MCAD_POINT pt )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_104*)m_entity)->X2 = pt.x;
    ((IGES_ENTITY_104*)m_entity)->Y2 = pt.y;
    return true;
}


bool DLL_IGES_ENTITY_104::SetConicEnd( double aX, double aY )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_104*)m_entity)->X2 = aX;
    ((IGES_ENTITY_104*)m_entity)->Y2 = aY;
    return true;
}


bool DLL_IGES_ENTITY_104::SetConicParams( double P1, double P2, double P3,
    double P4, double P5, double P6 )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_104*)m_entity)->A = P1;
    ((IGES_ENTITY_104*)m_entity)->B = P2;
    ((IGES_ENTITY_104*)m_entity)->C = P3;
    ((IGES_ENTITY_104*)m_entity)->D = P4;
    ((IGES_ENTITY_104*)m_entity)->E = P5;
    ((IGES_ENTITY_104*)m_entity)->F = P6;
    return true;
}
