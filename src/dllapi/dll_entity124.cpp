/*
 * file: dll_entity124.cpp
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

#include <dll_entity124.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity124.h>


DLL_IGES_ENTITY_124::DLL_IGES_ENTITY_124( IGES* aParent, bool create )
{
    m_type = ENT_TRANSFORMATION_MATRIX;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_TRANSFORMATION_MATRIX, &m_entity );
        else
            m_entity = new IGES_ENTITY_124( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}

DLL_IGES_ENTITY_124::DLL_IGES_ENTITY_124( DLL_IGES& aParent, bool create )
{
    m_type = ENT_TRANSFORMATION_MATRIX;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_TRANSFORMATION_MATRIX, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_124::~DLL_IGES_ENTITY_124()
{
    return;
}


bool DLL_IGES_ENTITY_124::GetOverallTransform( MCAD_TRANSFORM& aTX )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aTX = ((IGES_ENTITY_124*)m_entity)->GetTransformMatrix();
    return true;
}


bool DLL_IGES_ENTITY_124::GetRootTransform( MCAD_TRANSFORM& aTX )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aTX = ((IGES_ENTITY_124*)m_entity)->T;
    return true;
}


bool DLL_IGES_ENTITY_124::SetRootTransform( MCAD_TRANSFORM*& aTX )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_124*)m_entity)->T = *aTX;
    return true;
}
