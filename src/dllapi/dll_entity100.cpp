/*
 * file: dll_entity100.cpp
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


#include <dll_entity100.h>
#include <dll_iges.h>

#include <iges.h>
#include <entity100.h>


DLL_IGES_ENTITY_100::DLL_IGES_ENTITY_100( IGES* aParent, bool create )
{
    m_type = ENT_CIRCULAR_ARC;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_CIRCULAR_ARC, &m_entity );
        else
            m_entity = new IGES_ENTITY_100( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_100::DLL_IGES_ENTITY_100( DLL_IGES& aParent, bool create )
{
    m_type = ENT_CIRCULAR_ARC;

    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_CIRCULAR_ARC, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_100::~DLL_IGES_ENTITY_100()
{
    return;
}


bool DLL_IGES_ENTITY_100::GetCircleCenter( double& aX, double& aY, double& aZ ) const
{
    if( !m_valid || NULL == m_entity )
        return false;

    aX = ((IGES_ENTITY_100*)m_entity)->X1;
    aY = ((IGES_ENTITY_100*)m_entity)->Y1;
    aZ = ((IGES_ENTITY_100*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_100::GetCircleCenter( MCAD_POINT& aPoint ) const
{
    if( !m_valid || NULL == m_entity )
        return false;

    aPoint.x = ((IGES_ENTITY_100*)m_entity)->X1;
    aPoint.y = ((IGES_ENTITY_100*)m_entity)->Y1;
    aPoint.z = ((IGES_ENTITY_100*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_100::GetCircleStart( double& aX, double& aY, double& aZ ) const
{
    if( !m_valid || NULL == m_entity )
        return false;

    aX = ((IGES_ENTITY_100*)m_entity)->X2;
    aY = ((IGES_ENTITY_100*)m_entity)->Y2;
    aZ = ((IGES_ENTITY_100*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_100::GetCircleStart( MCAD_POINT& aPoint ) const
{
    if( !m_valid || NULL == m_entity )
        return false;

    aPoint.x = ((IGES_ENTITY_100*)m_entity)->X2;
    aPoint.y = ((IGES_ENTITY_100*)m_entity)->Y2;
    aPoint.z = ((IGES_ENTITY_100*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_100::GetCircleEnd( double& aX, double& aY, double& aZ ) const
{
    if( !m_valid || NULL == m_entity )
        return false;

    aX = ((IGES_ENTITY_100*)m_entity)->X3;
    aY = ((IGES_ENTITY_100*)m_entity)->Y3;
    aZ = ((IGES_ENTITY_100*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_100::GetCircleEnd( MCAD_POINT& aPoint ) const
{
    if( !m_valid || NULL == m_entity )
        return false;

    aPoint.x = ((IGES_ENTITY_100*)m_entity)->X3;
    aPoint.y = ((IGES_ENTITY_100*)m_entity)->Y3;
    aPoint.z = ((IGES_ENTITY_100*)m_entity)->ZT;
    return true;
}


bool DLL_IGES_ENTITY_100::SetCircleCenter( double aX, double aY, double aZ )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_100*)m_entity)->X1 = aX;
    ((IGES_ENTITY_100*)m_entity)->Y1 = aY;
    ((IGES_ENTITY_100*)m_entity)->ZT = aZ;
    return true;
}


bool DLL_IGES_ENTITY_100::SetCircleCenter( const MCAD_POINT& aPoint )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_100*)m_entity)->X1 = aPoint.x;
    ((IGES_ENTITY_100*)m_entity)->Y1 = aPoint.y;
    ((IGES_ENTITY_100*)m_entity)->ZT = aPoint.z;
    return true;
}


bool DLL_IGES_ENTITY_100::SetCircleStart( double aX, double aY )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_100*)m_entity)->X2 = aX;
    ((IGES_ENTITY_100*)m_entity)->Y2 = aY;
    return true;
}


bool DLL_IGES_ENTITY_100::SetCircleStart( const MCAD_POINT& aPoint )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_100*)m_entity)->X2 = aPoint.x;
    ((IGES_ENTITY_100*)m_entity)->Y2 = aPoint.y;
    return true;
}


bool DLL_IGES_ENTITY_100::SetCircleEnd( double aX, double aY )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_100*)m_entity)->X3 = aX;
    ((IGES_ENTITY_100*)m_entity)->Y3 = aY;
    return true;
}


bool DLL_IGES_ENTITY_100::SetCircleEnd( const MCAD_POINT& aPoint )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_100*)m_entity)->X3 = aPoint.x;
    ((IGES_ENTITY_100*)m_entity)->Y3 = aPoint.y;
    return true;
}
