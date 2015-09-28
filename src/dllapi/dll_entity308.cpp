/*
 * file: dll_entity308.cpp
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

#include <dll_entity308.h>
#include <dll_iges.h>

#include <iges.h>
#include <entity308.h>


DLL_IGES_ENTITY_308::DLL_IGES_ENTITY_308( IGES* aParent, bool create )
{
    m_type = ENT_SUBFIGURE_DEFINITION;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_SUBFIGURE_DEFINITION, &m_entity );
        else
            m_entity = new IGES_ENTITY_308( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_308::DLL_IGES_ENTITY_308( DLL_IGES& aParent, bool create )
{
    m_type = ENT_SUBFIGURE_DEFINITION;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_SUBFIGURE_DEFINITION, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_308::~DLL_IGES_ENTITY_308()
{
    return;
}


bool DLL_IGES_ENTITY_308::GetNumDE( size_t& aDESize )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aDESize = ((IGES_ENTITY_308*)m_entity)->DE.size();
    return true;
}


bool DLL_IGES_ENTITY_308::GetDEList( size_t& aDESize, IGES_ENTITY**& aDEList )
{
    if( !m_valid || NULL == m_entity )
    {
        aDESize = 0;
        aDEList = NULL;
        return false;
    }

    std::list< IGES_ENTITY* >* pDE = &((IGES_ENTITY_308*)m_entity)->DE;
    std::vector< IGES_ENTITY* >* pvDE = &((IGES_ENTITY_308*)m_entity)->m_DE;

    if( pDE->size() != pvDE->size() )
    {
        pvDE->clear();

        std::list< IGES_ENTITY* >::iterator sDL = pDE->begin();
        std::list< IGES_ENTITY* >::iterator eDL = pDE->end();

        while( sDL != eDL )
        {
            pvDE->push_back( *sDL );
            ++sDL;
        }
    }

    aDESize = pvDE->size();
    aDEList = &((*pvDE)[0]);
    return true;
}


bool DLL_IGES_ENTITY_308::AddDE(IGES_ENTITY* aPtr)
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_308*)m_valid)->AddDE( aPtr );
}

bool DLL_IGES_ENTITY_308::AddDE(DLL_IGES_ENTITY*& aPtr)
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_308*)m_valid)->AddDE( aPtr->GetRawPtr() );
}


bool DLL_IGES_ENTITY_308::DelDE( IGES_ENTITY* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_308*)m_valid)->DelDE( aPtr );
}


bool DLL_IGES_ENTITY_308::DelDE(DLL_IGES_ENTITY*& aPtr)
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_308*)m_valid)->DelDE( aPtr->GetRawPtr() );
}


bool DLL_IGES_ENTITY_308::GetNestDepth( int& aNestDepth )
{
    if( !m_valid || NULL == m_entity )
    {
        aNestDepth = 0;
        return false;
    }

    aNestDepth = ((IGES_ENTITY_308*)m_valid)->getDepthLevel();
    return true;
}


bool DLL_IGES_ENTITY_308::GetName( const char*& aName )
{
    if( !m_valid || NULL == m_entity )
        return false;

    std::string* sp = &((IGES_ENTITY_308*)m_valid)->NAME;

    if( sp->empty() )
    {
        aName = NULL;
        return false;
    }

    aName = sp->c_str();
    return true;
}


bool DLL_IGES_ENTITY_308::SetName( const char*& aName )
{
    if( !m_valid || NULL == m_entity )
        return false;

    ((IGES_ENTITY_308*)m_valid)->NAME = aName;
    return true;
}
