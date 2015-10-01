/*
 * file: dll_iges_curve.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * This file is part of libIGES.
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


#include <dll_iges_curve.h>
#include <error_macros.h>

#include <iges_curve.h>


DLL_IGES_CURVE::DLL_IGES_CURVE( IGES* aParent ) : DLL_IGES_ENTITY( aParent )
{
    return;
}

DLL_IGES_CURVE::DLL_IGES_CURVE( DLL_IGES& aParent ) : DLL_IGES_ENTITY( aParent )
{
    return;
}

DLL_IGES_CURVE::~DLL_IGES_CURVE()
{
    return;
}


bool DLL_IGES_CURVE::IsClosed( void )
{
    if( !m_valid || NULL == m_entity )
    {
        ERRMSG << "\n + [BUG] invalid IGES_ENTITY object\n";
        return false;
    }

    return ((IGES_CURVE*)m_entity)->IsClosed();
}


int DLL_IGES_CURVE::GetNCurves( void )
{
    if( !m_valid || NULL == m_entity )
    {
        ERRMSG << "\n + [BUG] invalid IGES_ENTITY object\n";
        return 0;
    }

    return ((IGES_CURVE*)m_entity)->GetNCurves();
}


IGES_CURVE* DLL_IGES_CURVE::GetCurve( int index )
{
    if( !m_valid || NULL == m_entity )
    {
        ERRMSG << "\n + [BUG] invalid IGES_ENTITY object\n";
        return NULL;
    }

    return ((IGES_CURVE*)m_entity)->GetCurve( index );
}


bool DLL_IGES_CURVE::GetStartPoint( MCAD_POINT& pt, bool xform )
{
    if( !m_valid || NULL == m_entity )
    {
        ERRMSG << "\n + [BUG] invalid IGES_ENTITY object\n";
        return false;
    }

    return ((IGES_CURVE*)m_entity)->GetStartPoint( pt, xform );
}


bool DLL_IGES_CURVE::GetEndPoint( MCAD_POINT& pt, bool xform )
{
    if( !m_valid || NULL == m_entity )
    {
        ERRMSG << "\n + [BUG] invalid IGES_ENTITY object\n";
        return false;
    }

    return ((IGES_CURVE*)m_entity)->GetEndPoint( pt, xform );
}


int DLL_IGES_CURVE::GetNSegments( void )
{
    if( !m_valid || NULL == m_entity )
    {
        ERRMSG << "\n + [BUG] invalid IGES_ENTITY object\n";
        return 0;
    }

    return ((IGES_CURVE*)m_entity)->GetNSegments();
}
