/*
 * file: dll_entity126.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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


#include <dll_entity126.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity126.h>

DLL_IGES_ENTITY_126::DLL_IGES_ENTITY_126( IGES* aParent, bool create )
{
    m_type = ENT_NURBS_CURVE;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_NURBS_CURVE, &m_entity );
        else
            m_entity = new IGES_ENTITY_126( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_126::DLL_IGES_ENTITY_126( DLL_IGES& aParent, bool create )
{
    m_type = ENT_NURBS_CURVE;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_NURBS_CURVE, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_126::~DLL_IGES_ENTITY_126()
{
    return;
}


bool DLL_IGES_ENTITY_126::GetNURBSData( int& nCoeff, int& order, double** knot,
    double** coeff, bool& isRational, bool& isClosed, bool& isPeriodic )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_126*)m_entity)->GetNURBSData( nCoeff, order, knot,
        coeff, isRational, isClosed, isPeriodic );
}


bool DLL_IGES_ENTITY_126::SetNURBSData( int nCoeff, int order, const double* knot,
    const double* coeff, bool isRational )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_126*)m_entity)->SetNURBSData( nCoeff, order, knot,
        coeff, isRational );
}


bool DLL_IGES_ENTITY_126::IsPlanar( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_126*)m_entity)->IsPlanar();
    return true;
}


bool DLL_IGES_ENTITY_126::IsRational( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_126*)m_entity)->IsRational();
    return true;
}


bool DLL_IGES_ENTITY_126::isPeriodic( bool& aResult )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aResult = ((IGES_ENTITY_126*)m_entity)->isPeriodic();
    return true;
}


bool DLL_IGES_ENTITY_126::GetNormal( MCAD_POINT& aNorm )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_126*)m_entity)->GetNormal( aNorm );
}

