/*
 * file: dll_entity406.cpp
 *
 * Copyright 2017, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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

#include <api/dll_entity406.h>
#include <api/dll_iges.h>
#include <core/iges.h>
#include <core/entity406.h>


DLL_IGES_ENTITY_406::DLL_IGES_ENTITY_406( IGES* aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_PROPERTY;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_PROPERTY, &m_entity );
        else
            m_entity = new IGES_ENTITY_406( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_406::DLL_IGES_ENTITY_406( DLL_IGES& aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_PROPERTY;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_PROPERTY, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_406::~DLL_IGES_ENTITY_406()
{
    return;
}


bool DLL_IGES_ENTITY_406::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_PROPERTY, &m_entity );
    else
        m_entity = new IGES_ENTITY_406( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag(&m_valid);
        return true;
    }

    return false;
}


const char * DLL_IGES_ENTITY_406::GetProperty_Name( void )
{
    const char *result = NULL;
    IGES_ENTITY_406* ep = (IGES_ENTITY_406 *)m_entity;

    if( ep->GetEntityForm() == 15 )
    {
        std::string *sp = (std::string *)ep->GetData();
        result = sp->c_str();
    }

    return result;
}


bool DLL_IGES_ENTITY_406::SetProperty_Name( const char * aName )
{
    bool result = false;

    IGES_ENTITY_406* ep = (IGES_ENTITY_406 *)m_entity;

    if( (ep->GetEntityForm() == 15) && (aName != NULL) )
    {
        result = ep->SetData((void *) aName);
    }

    return result;
}
