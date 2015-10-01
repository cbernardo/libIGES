/*
 * file: dll_entity142.cpp
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


#include <dll_entity142.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity142.h>


DLL_IGES_ENTITY_142::DLL_IGES_ENTITY_142( IGES* aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_CURVE_ON_PARAMETRIC_SURFACE;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_CURVE_ON_PARAMETRIC_SURFACE, &m_entity );
        else
            m_entity = new IGES_ENTITY_142( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_142::DLL_IGES_ENTITY_142( DLL_IGES& aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_CURVE_ON_PARAMETRIC_SURFACE;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_CURVE_ON_PARAMETRIC_SURFACE, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_142::~DLL_IGES_ENTITY_142()
{
    return;
}


bool DLL_IGES_ENTITY_142::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_CURVE_ON_PARAMETRIC_SURFACE, &m_entity );
    else
        m_entity = new IGES_ENTITY_142( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag(&m_valid);
        return true;
    }

    return false;
}


bool DLL_IGES_ENTITY_142::GetSurface( IGES_ENTITY** aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_142*)m_entity)->GetSPTR( aPtr );
}


bool DLL_IGES_ENTITY_142::SetSurface( IGES_ENTITY* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_142*)m_entity)->SetSPTR( aPtr );
}


bool DLL_IGES_ENTITY_142::GetParameterSpaceBound( IGES_ENTITY** aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_142*)m_entity)->GetBPTR( aPtr );
}


bool DLL_IGES_ENTITY_142::SetParameterSpaceBound( IGES_ENTITY* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_142*)m_entity)->SetBPTR( aPtr );
}


bool DLL_IGES_ENTITY_142::GetModelSpaceBound( IGES_ENTITY** aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_142*)m_entity)->GetCPTR( aPtr );
}


bool DLL_IGES_ENTITY_142::SetModelSpaceBound( IGES_ENTITY* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_142*)m_entity)->SetCPTR( aPtr );
}


bool DLL_IGES_ENTITY_142::GetCurvePreference( BOUND_CURVE_PREF& aPref )
{
    if( !m_valid || NULL == m_entity )
        return false;

    int tmp = ((IGES_ENTITY_142*)m_entity)->PREF;

    if( tmp < 0 || tmp > (int)BOUND_PREF_ANY )
        aPref = BOUND_PREF_UNSPECIFIED;
    else
        aPref = (BOUND_CURVE_PREF)tmp;

    return true;
}


bool DLL_IGES_ENTITY_142::SetCurvePreference( BOUND_CURVE_PREF aPref )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_142*)m_entity)->PREF = (int)aPref;
    return true;
}


bool DLL_IGES_ENTITY_142::GetCurveCreationFlag( CURVE_CREATION& aFlag )
{
    if( !m_valid || NULL == m_entity )
        return false;

    int tmp = ((IGES_ENTITY_142*)m_entity)->CRTN;

    if( tmp < 0 || tmp > (int)CURVE_CREATE_PARAMETRIC )
        aFlag = CURVE_CREATE_UNSPECIFIED;
    else
        aFlag = (CURVE_CREATION)tmp;

    return true;
}


bool DLL_IGES_ENTITY_142::SetCurveCreationFlag( CURVE_CREATION aFlag )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_142*)m_entity)->CRTN = (int)aFlag;
    return true;
}
