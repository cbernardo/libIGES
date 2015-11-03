/*
 * file: dll_entity314.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 314: Color, Section 4.77, p.386 (414)
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


#include <error_macros.h>
#include <api/dll_entity314.h>
#include <api/dll_iges.h>
#include <core/iges.h>
#include <core/entity314.h>

using namespace std;


DLL_IGES_ENTITY_314::DLL_IGES_ENTITY_314( IGES* aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_COLOR_DEFINITION;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_COLOR_DEFINITION, &m_entity );
        else
            m_entity = new IGES_ENTITY_314( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_314::DLL_IGES_ENTITY_314( DLL_IGES& aParent, bool create ) : DLL_IGES_ENTITY( aParent )
{
    m_type = ENT_COLOR_DEFINITION;

    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_COLOR_DEFINITION, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_314::~DLL_IGES_ENTITY_314()
{
    return;
}


bool DLL_IGES_ENTITY_314::NewEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    if( NULL != m_parent && m_hasParent )
        m_parent->NewEntity( ENT_COLOR_DEFINITION, &m_entity );
    else
        m_entity = new IGES_ENTITY_314( NULL );

    if( NULL != m_entity )
    {
        m_entity->AttachValidFlag( &m_valid );
        return true;
    }

    return false;
}


bool DLL_IGES_ENTITY_314::GetColor( double& aRed, double& aGreen, double& aBlue )
{
    if( !m_valid || NULL == m_entity )
        return false;

    aRed = ((IGES_ENTITY_314*)m_entity)->red;
    aGreen = ((IGES_ENTITY_314*)m_entity)->green;
    aBlue = ((IGES_ENTITY_314*)m_entity)->blue;

    return true;
}


bool DLL_IGES_ENTITY_314::SetColor( double aRed, double aGreen, double aBlue )
{
    if( !m_valid || NULL == m_entity )
        return false;

    if( aRed < 0.0 || aRed > 100.0
        || aGreen < 0.0 || aGreen > 100.0
        || aBlue < 0.0 || aBlue > 100.0 )
    {
        ERRMSG << "\n + [BUG] invalid RGB value; range is 0.0 .. 100.0 for each component\n";
        cerr << " + [INFO] RGB = (" << aRed << ", " << aGreen << ", " << aBlue << ")\n";
        return false;
    }

    ((IGES_ENTITY_314*)m_entity)->red = aRed;
    ((IGES_ENTITY_314*)m_entity)->green = aGreen;
    ((IGES_ENTITY_314*)m_entity)->blue = aBlue;

    return true;
}


bool DLL_IGES_ENTITY_314::GetName( const char*& aName )
{
    if( !m_valid || NULL == m_entity )
        return false;

    string* sp = &((IGES_ENTITY_314*)m_entity)->cname;

    if( sp->empty() )
    {
        aName = NULL;
        return false;
    }

    aName = sp->c_str();
    return true;
}


bool DLL_IGES_ENTITY_314::SetName( const char* aName )
{
    if( !m_valid || NULL == m_entity )
        return false;

    if( NULL != aName )
        ((IGES_ENTITY_314*)m_entity)->cname = aName;
    else
        ((IGES_ENTITY_314*)m_entity)->cname.clear();

    return true;
}
