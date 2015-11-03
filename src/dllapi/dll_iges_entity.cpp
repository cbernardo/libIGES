/*
 * file: dll_iges_entity.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Base entity of all DLL_IGES Entity classes.
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
#include <api/dll_iges_entity.h>
#include <api/dll_entity314.h>
#include <core/iges.h>
#include <core/entity124.h>
#include <error_macros.h>

#define CHECK_VALID_RETURN_FALSE do { \
     if( !m_valid || NULL == m_entity ) { \
        ERRMSG << "\n + [BUG] invalid entity object\n"; \
        return false; \
     } } while( 0 )


DLL_IGES_ENTITY::DLL_IGES_ENTITY( IGES* aParent )
{
    m_parent = aParent;

    if( NULL != aParent )
        m_parent->AttachValidFlag( &m_hasParent );

    m_entity = NULL;
    m_valid = false;
    m_type = ENT_NULL;
    return;
}


DLL_IGES_ENTITY::DLL_IGES_ENTITY( DLL_IGES& aParent )
{
    m_parent = aParent.GetRawPtr();

    if( NULL != m_parent )
        m_parent->AttachValidFlag( &m_hasParent );

    m_entity = NULL;
    m_valid = false;
    m_type = ENT_NULL;
    return;
}


DLL_IGES_ENTITY::~DLL_IGES_ENTITY()
{
    if( m_parent && m_hasParent )
        m_parent->DetachValidFlag( &m_hasParent );

    if( m_valid && NULL != m_entity )
        m_entity->DetachValidFlag( &m_valid );

    return;
}


bool DLL_IGES_ENTITY::SetAPIParentIGES( IGES* aParent )
{
    if( m_hasParent && NULL != m_parent )
        m_parent->DetachValidFlag( &m_hasParent );

    if( aParent )
    {
        m_parent = aParent;
        aParent->AttachValidFlag( &m_hasParent );
    }
    else
    {
        m_parent = NULL;
        m_hasParent = false;
    }

    return true;
}


bool DLL_IGES_ENTITY::SetAPIParentIGES( DLL_IGES& aParent )
{
    if( m_hasParent && NULL != m_parent )
        m_parent->DetachValidFlag( &m_hasParent );

    IGES* ap = aParent.GetRawPtr();

    if( ap )
    {
        m_parent = ap;
        ap->AttachValidFlag( &m_hasParent );
    }
    else
    {
        m_parent = NULL;
        m_hasParent = false;
    }

    return true;
}



IGES_ENTITY_TYPE DLL_IGES_ENTITY::GetEntityType() const
{
    return m_type;
}


IGES_ENTITY* DLL_IGES_ENTITY::GetRawPtr( void )
{
    if( m_entity && m_valid )
        return m_entity;

    m_valid = false;
    m_entity = NULL;
    return NULL;
}


bool DLL_IGES_ENTITY::IsValid( void )
{
    return m_valid;
}


IGES_ENTITY* DLL_IGES_ENTITY::Detach( void )
{
    if( m_valid && NULL != m_entity )
    {
        IGES_ENTITY* tmp = m_entity;
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
        m_valid = false;
        return tmp;
    }

    m_entity = NULL;
    m_valid = false;
    return NULL;
}


void DLL_IGES_ENTITY::DelEntity( void )
{
    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        delete m_entity;
    }

    m_entity = NULL;
    m_valid = false;
    return;
}


bool DLL_IGES_ENTITY::Attach( IGES_ENTITY* aEntity )
{
    if( NULL == aEntity )
        return false;

    if( m_type != aEntity->GetEntityType() )
        return false;

    if( m_valid && NULL != m_entity )
    {
        m_entity->DetachValidFlag( &m_valid );
        m_entity = NULL;
    }

    m_entity = aEntity;
    m_entity->AttachValidFlag( &m_valid );
    return true;
}


bool DLL_IGES_ENTITY::GetNOptionalEntities( int& aNOptEnt )
{
    if( !m_valid || NULL == m_entity )
    {
        aNOptEnt = 0;
        return false;
    }

    aNOptEnt = m_entity->GetNOptionalEntities();
    return true;
}


bool DLL_IGES_ENTITY::GetOptionalEntities( size_t& aListSize, IGES_ENTITY**& aEntityList )
{
    if( !m_valid || NULL == m_entity )
    {
        aEntityList = NULL;
        return false;
    }

    return m_entity->GetOptionalEntities( aListSize, aEntityList );
}


bool DLL_IGES_ENTITY::AddOptionalEntity( IGES_ENTITY* aEntity )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->AddOptionalEntity( aEntity );
}


bool DLL_IGES_ENTITY::AddOptionalEntity( DLL_IGES_ENTITY*& aEntity )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->AddOptionalEntity( aEntity->GetRawPtr() );
}


bool DLL_IGES_ENTITY::DelOptionalEntity( IGES_ENTITY* aEntity )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->DelOptionalEntity( aEntity );
}


bool DLL_IGES_ENTITY::DelOptionalEntity( DLL_IGES_ENTITY*& aEntity )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->DelOptionalEntity( aEntity->GetRawPtr() );
}


bool DLL_IGES_ENTITY::GetNComments( int& nComments )
{
    CHECK_VALID_RETURN_FALSE;

    nComments = m_entity->GetNComments();
    return true;
}


bool DLL_IGES_ENTITY::GetComments( size_t& aListSize, char const**& aCommentList )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetComments( aListSize, aCommentList );
}


bool DLL_IGES_ENTITY::AddComment( const char*& aComment )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->AddComment( aComment );
}


bool DLL_IGES_ENTITY::DelComment( int index )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->DelComment( index );
}


bool DLL_IGES_ENTITY::ClearComments( void )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->ClearComments();
}


bool DLL_IGES_ENTITY::SetParentIGES( IGES* aParent )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetParentIGES( aParent );
}


bool DLL_IGES_ENTITY::SetParentIGES( DLL_IGES*& aParent )
{
    CHECK_VALID_RETURN_FALSE;

    // note: no check for validity of aParent->GetRawPtr()
    // since NULL is a valid argument for this function
    return m_entity->SetParentIGES( aParent->GetRawPtr() );
}


bool DLL_IGES_ENTITY::GetParentIGES( IGES*& aParent )
{
    CHECK_VALID_RETURN_FALSE;

    aParent = m_entity->GetParentIGES();
    return true;
}


bool DLL_IGES_ENTITY::GetEntityType( int& aType )
{
    CHECK_VALID_RETURN_FALSE;

    aType = m_entity->GetEntityType();
    return true;
}


bool DLL_IGES_ENTITY::GetEntityForm( int& aForm )
{
    CHECK_VALID_RETURN_FALSE;

    aForm = m_entity->GetEntityForm();
    return true;
}


bool DLL_IGES_ENTITY::SetEntityForm( int aForm )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetEntityForm( aForm );
}


bool DLL_IGES_ENTITY::SetStructure( IGES_ENTITY* aStructure )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetStructure( aStructure );
}


bool DLL_IGES_ENTITY::SetStructure( DLL_IGES_ENTITY*& aStructure )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetStructure( aStructure->GetRawPtr() );
}


bool DLL_IGES_ENTITY::GetStructure( IGES_ENTITY*& aStructure )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetStructure( &aStructure );
}


bool DLL_IGES_ENTITY::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLineFontPattern( aPattern );
}


bool DLL_IGES_ENTITY::SetLineFontPattern( DLL_IGES_ENTITY*& aPattern )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLineFontPattern( aPattern->GetRawPtr() );
}


bool DLL_IGES_ENTITY::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLineFontPattern( aPattern );
}


bool DLL_IGES_ENTITY::GetLineFontPattern( IGES_LINEFONT_PATTERN& aPattern )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetLineFontPattern( aPattern );
}


bool DLL_IGES_ENTITY::GetLineFontPatternEntity( IGES_ENTITY*& aPattern )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetLineFontPatternEntity( &aPattern );
}


bool DLL_IGES_ENTITY::SetLevel( int aLevel )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLevel( aLevel );
}


bool DLL_IGES_ENTITY::SetLevel( DLL_IGES_ENTITY*& aLevel )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLevel( aLevel->GetRawPtr() );
}


bool DLL_IGES_ENTITY::SetLevel( IGES_ENTITY* aLevel )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLevel( aLevel );
}


bool DLL_IGES_ENTITY::GetLevel( int& aLevel )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetLevel( aLevel );
}


bool DLL_IGES_ENTITY::GetLevelEntity( IGES_ENTITY*& aLevel )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetLevelEntity( &aLevel );
}


bool DLL_IGES_ENTITY::SetView( DLL_IGES_ENTITY*& aView )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetView( aView->GetRawPtr() );
}


bool DLL_IGES_ENTITY::SetView( IGES_ENTITY* aView )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetView( aView );
}


bool DLL_IGES_ENTITY::GetView( IGES_ENTITY*& aView )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetView( &aView );
}


bool DLL_IGES_ENTITY::SetTransform( DLL_IGES_ENTITY*& aTransform )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetTransform( aTransform->GetRawPtr() );
}


bool DLL_IGES_ENTITY::SetTransform( IGES_ENTITY* aTransform )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetTransform( aTransform );
}


bool DLL_IGES_ENTITY::GetTransform( IGES_ENTITY*& aTransform )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetTransform( &aTransform );
}


bool DLL_IGES_ENTITY::SetLabelAssoc( DLL_IGES_ENTITY*& aLabelAssoc )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLabelAssoc( aLabelAssoc->GetRawPtr() );
}


bool DLL_IGES_ENTITY::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLabelAssoc( aLabelAssoc );
}


bool DLL_IGES_ENTITY::GetLabelAssoc( IGES_ENTITY*& aLabelAssoc )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetLabelAssoc( &aLabelAssoc );
}


bool DLL_IGES_ENTITY::SetColor( IGES_COLOR aColor )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetColor( aColor );
}


bool DLL_IGES_ENTITY::SetColor( DLL_IGES_ENTITY_314*& aColor )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetColor( aColor->GetRawPtr() );
}


bool DLL_IGES_ENTITY::SetColor( IGES_ENTITY* aColor )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetColor( aColor );
}


bool DLL_IGES_ENTITY::GetColor( IGES_COLOR& aColor )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetColor( aColor );
}


bool DLL_IGES_ENTITY::GetColorEntity( IGES_ENTITY*& aColor )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetColorEntity( &aColor );
}


bool DLL_IGES_ENTITY::SetLineWeightNum( int aLineWeight )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetLineWeightNum( aLineWeight );
}


bool DLL_IGES_ENTITY::GetLineWeightNum( int& aLineWeight )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetLineWeightNum( aLineWeight );
}


bool DLL_IGES_ENTITY::SetLabel( const char* aLabel )
{
    CHECK_VALID_RETURN_FALSE;

    std::string tl;

    if( NULL == aLabel )
        tl = "";
    else
        tl = aLabel;

    return m_entity->SetLabel( tl );
}


bool DLL_IGES_ENTITY::GetLabel( char const*& aLabel )
{
    CHECK_VALID_RETURN_FALSE;

    aLabel = m_entity->GetLabel();

    if( NULL == aLabel || 0 == aLabel[0] )
    {
        aLabel = NULL;
        return false;
    }

    return true;
}


bool DLL_IGES_ENTITY::SetEntitySubscript( int aSubscript )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetEntitySubscript( aSubscript );
}


bool DLL_IGES_ENTITY::GetEntitySubscript( int& aSubscript )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetEntitySubscript( aSubscript );
}


bool DLL_IGES_ENTITY::SetVisibility( bool isVisible )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetVisibility( isVisible );
}


bool DLL_IGES_ENTITY::GetVisibility( bool& isVisible )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetVisibility( isVisible );
}


bool DLL_IGES_ENTITY::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetDependency( aDependency );
}


bool DLL_IGES_ENTITY::GetDependency( IGES_STAT_DEPENDS& aDependency )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetDependency( aDependency );
}


bool DLL_IGES_ENTITY::SetEntityUse( IGES_STAT_USE aUseCase )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetEntityUse( aUseCase );
}


bool DLL_IGES_ENTITY::GetEntityUse( IGES_STAT_USE& aUseCase )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetEntityUse( aUseCase );
}


bool DLL_IGES_ENTITY::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->SetHierarchy( aHierarchy );
}


bool DLL_IGES_ENTITY::GetHierarchy( IGES_STAT_HIER& aHierarchy )
{
    CHECK_VALID_RETURN_FALSE;

    return m_entity->GetHierarchy( aHierarchy );
}
