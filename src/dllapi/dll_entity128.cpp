/*
 * file: dll_entity128.cpp
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


#include <dll_entity128.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity128.h>


DLL_IGES_ENTITY_128::DLL_IGES_ENTITY_128( IGES* aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_NURBS_SURFACE;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_NURBS_SURFACE, &m_entity );
        else
            m_entity = new IGES_ENTITY_128( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_128::DLL_IGES_ENTITY_128( DLL_IGES& aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_NURBS_SURFACE;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_NURBS_SURFACE, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_128::~DLL_IGES_ENTITY_128()
{
    return;
}


bool DLL_IGES_ENTITY_128::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_NURBS_SURFACE, &m_entity );
    else
        m_entity = new IGES_ENTITY_128( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag(&m_valid);
        return true;
    }

    return false;
}


bool DLL_IGES_ENTITY_128::GetNURBSData( int& nCoeff1, int& nCoeff2,
    int& order1, int& order2, double** knot1, double** knot2,
    double** coeff, bool& isRational, bool& isClosed1, bool& isClosed2,
    bool& isPeriodic1, bool& isPeriodic2,
    double& u0, double& u1, double& v0, double& v1 )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_128*)m_entity)->GetNURBSData( nCoeff1, nCoeff2,
        order1, order2, knot1, knot2, coeff, isRational, isClosed1,
        isClosed2, isPeriodic1, isPeriodic2, u0, u1, v0, v1 );
}


bool DLL_IGES_ENTITY_128::SetNURBSData( int nCoeff1, int nCoeff2,
    int order1, int order2, const double* knot1, const double* knot2,
    const double* coeff, bool isRational, bool isPeriodic1, bool isPeriodic2,
    double u0, double u1, double v0, double v1 )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_128*)m_entity)->SetNURBSData( nCoeff1, nCoeff2,
        order1, order2, knot1, knot2, coeff, isRational, isPeriodic1, isPeriodic2,
        u0, u1, v0, v1 );
}


bool DLL_IGES_ENTITY_128::IsRational( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_128*)m_entity)->IsRational();
    return true;
}


bool DLL_IGES_ENTITY_128::isClosed1( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_128*)m_entity)->isClosed1();
    return true;
}


bool DLL_IGES_ENTITY_128::isClosed2( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_128*)m_entity)->isClosed2();
    return true;
}


bool DLL_IGES_ENTITY_128::isPeriodic1( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_128*)m_entity)->isPeriodic1();
    return true;
}


bool DLL_IGES_ENTITY_128::isPeriodic2( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_128*)m_entity)->isPeriodic2();
    return true;
}
