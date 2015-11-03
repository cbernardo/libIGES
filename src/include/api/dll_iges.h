/*
 * file: dll_iges.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES top level object for input, output and manipulation
 * of IGES entity data.
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


#ifndef DLL_IGES_H
#define DLL_IGES_H

#include <cstddef>

#include <libigesconf.h>
#include <core/iges_base.h>

class IGES;
class DLL_IGES_ENTITY;
class DLL_IGES_ENTITY_308;
class IGES_ENTITY;
class IGES_ENTITY_308;


/**
 * Class DLL_IGES
 * is the high level object for manipulating IGES data
 */
class MCAD_API DLL_IGES
{
private:
    IGES* m_iges;
    bool  m_valid;  // set to false if the m_iges object is deleted

public:
     DLL_IGES();
     ~DLL_IGES();

    // return a pointer to the internal IGES object
    IGES* GetRawPtr( void );

    /**
     * Function Compact
     * iterates over the IGES entities managed by this object and
     * removes any redundant data which may have been created for
     * the user's convenience.
     */
    void Compact( void );

    /**
     * Function Cull
     * culls all orphaned entities; if vicious = true then
     * all top-level entities which are not Type 408
     * (Singular Subfigure Instance) are also culled
     */
    void Cull( bool vicious = false );

    /**
     * Function Clear
     * deletes all entities and reinitializes global data
     */
    bool Clear( void );

    /**
     * Function Read
     * opens and processes the file with the given name;
     * returns true on success.
     *
     * @param aFileName = path to file to be processed
     */
    bool Read( const char* aFileName );

    /**
     * Function Write
     * opens a file and writes out IGES data; returns true on success
     *
     * @param aFileName = path to file to be written
     * @param fOverwrite = set to true if an existing file should be overwritten
     */
    bool Write( const char* aFileName, bool fOverwrite = false );

    /**
     * Function Export
     * transfers all entities within the current IGES object into
     * the @param newParent IGES object as a step in the creation
     * of a solid assembly. Returns true on success.
     *
     * @param newParent = pointer to the IGES object to receive the exported entities
     * @param packagedEntity = handle to the Subfigure Definition Entity to be used
     * to refer to the contents of this IGES file as a part or subassembly within an
     * assembly.
     */
    bool Export( DLL_IGES* newParent, IGES_ENTITY_308** packagedEntity );
    bool Export( IGES* newParent, IGES_ENTITY_308** packagedEntity );

    /**
     * Function GetNewPartName
     * creates a new, and hopefully unique, part name. The part name may not be
     * unique if a sub-assembly happens to have a sub-part with the same name,
     * in which case a more comprehensive naming code must be developed.
     *
     * @return a pointer to the new part name; the user must delete [] this pointer
     * when it is no longer required.
     */
    char* GetNewPartName( void );

    // create a new (hopefully unique) assembly name; the name may not be unique
    // if a sub-assembly happens to have the same name
    /**
     * Function GetNewAssemblyName
     * creates a new, and hopefully unique, assembly name. The assembly name may
     * not be unique if a sub-assembly happens to have a sub-assembly with the
     * same name, in which case a more comprehensive naming code must be developed.
     *
     * @return a pointer to the new assembly name; the user must delete [] this
     * pointer when it is no longer required.
     */
    char* GetNewAssemblyName( char* );

    /**
     * Function NewEntity
     * creates a new IGES entity with the specified type and returns
     * true on success; for entity types see the IGES_ENTITY_TYPE
     * enumeration in iges_base.h.  If the entity type does not
     * exist in the specification or the type is not yet supported
     * the function returns a pointer to a NULL Entity.
     *
     * @param aEntityType = entity type to instantiate; the value must
     * be a valid value of the IGES_ENTITY_TYPE enumeration.
     * @param aEntityPointer = handle to store the pointer to the new entity.
     */
    bool NewAPIEntity( IGES_ENTITY_TYPE aEntityType, DLL_IGES_ENTITY*& aEntityPointer );
    bool NewIGESEntity( IGES_ENTITY_TYPE aEntityType, IGES_ENTITY** aEntityPointer );

    /**
     * Function AddEntity
     * adds an entity from another IGES object or an entity created
     * without the aid of NewEntity() to the list of entities managed by
     * this IGES object and returns true on success.
     *
     * @param aEntity = pointer to an entity to be owned by this IGES object
     */
    bool AddEntity( DLL_IGES_ENTITY* aEntity );
    bool AddEntity( IGES_ENTITY* aEntity );

    /**
     * Function DelEntity
     * removes the entity specified by the pointer, frees associated
     * entity data, and returns true on success.
     *
     * @param aEntity = pointer to the entity to be destroyed
     */
    bool DelEntity( DLL_IGES_ENTITY* aEntity );
    bool DelEntity( IGES_ENTITY* aEntity );

    /**
     * Function UnlinkEntity
     * removes this IGES object's reference to the given entity
     * but leaves the entity data untouched; returns true on success.
     * This function is useful when transferring data between IGES
     * entities during an Export() operation. The transferred entity
     * may retain links to other entities in this IGES object and it
     * is the user's duty to ensure that such linkages are severed
     * if appropriate.
     *
     * @param aEntity = pointer to the entity to be disassociated
     */
    bool UnlinkEntity( DLL_IGES_ENTITY* aEntity );
    bool UnlinkEntity( IGES_ENTITY* aEntity );

    /**
     * Function ConvertUnits
     * scales all entities owned by this IGES object to conform to
     * the new unit specified and returns true on success. This
     * function will fail if either the internal units or @param newUnit
     * are equal to IGES_UNIT::UNIT_EXTERN.
     *
     * @param newUnit = a unit as specified by the enumeration
     * IGES_UNIT, except for UNIT_EXTERN.
     */
    bool ConvertUnits( IGES_UNIT newUnit );

    /**
     * Function ChangeModelScale
     * changes the model scale to the specified value and returns
     * true on success. This is useful for scaling models which
     * do not use a Model Scale of 1.0. To ensure the greatest
     * possible acceptance of user-generated models within different
     * MCAD packages, users should never specify a model scale other
     * than 1.0.
     *
     * @param aScale = the new Model Scale to be applied to the data
     */
    bool ChangeModelScale( double aScale );

    /**
     * Function GetHeaders
     * returns a pointer to the list of strings read from or to be
     * written to the Start Section of the IGES file.
     */
    bool GetHeaders( size_t& aListSize, char const**& aHeaderList );

    /**
     * Function GetNHeaderLines
     * returns the number of strings read from or to be written to
     * the Start Section of an IGES file.
     */
    size_t GetNHeaderLines(void);

    /**
     * Function AddToHeader
     * adds a text line to the data to be written to the Start Section
     * of an IGES file and returns true on success.
     *
     * @param comments = text to be appended to the Start Section.
     */
    bool AddToHeader( char const*& comment );

    // Accessors for IGES global header
    // ===
    // A. Parameter Delimeter: shall not be exposed to the user
    // B. Record Delimeter: shall not be exposed to the user
    // C. Product IDSS
    bool GetProductID_SendingSystem( char const*& aProductIDSS );
    bool SetProductID_SendingSystem( char const* aProductIDSS );

    // D. File Name: shall not be exposed to the user
    // E. Native System ID
    bool GetNativeSystemID( char const*& aNativeSoftwareName );
    bool SetNativeSystemID( char const* aNativeSoftwareName );

    // F. IGES Preprocessor Version: [no SET accessor]
    bool GetPreprocessorVersion( char const*& aIGES_PreprocessorVersion );

    // G. Number of significant bits in an integer: not exposed
    // H. Max exponent of a float type: not exposed
    // I. Max sig. digits of a float type: not exposed
    // J. Max exponent of a double type: not exposed
    // K. Max sig. digits of a double type: not exposed
    // L. Product ID, Receiving System
    bool GetProductID_ReceivingSystem( char const*& aProductIDRS );
    bool SetProductID_ReceivingSystem( char const* aProductIDRS );

    // M. modelScale: everyone uses 1.0 so this will not be exposed to the user
    // N. Units Flag: in Set routine UNIT_EXTERN shall not be allowed
    bool GetUnitsFlag( IGES_UNIT& aUnitFlag );
    bool SetUnitsFlag( IGES_UNIT aUnitFlag );

    // O. Units name: read only
    bool GetUnitsName( char const*& aUnitName );

    // P. Max Linewidth Gradations
    bool GetMaxLineWidthGrad( int& aMaxLWGrad );
    bool SetMaxLineWidthGrad( int aMaxLWGrad );

    // Q. Max. line width
    bool GetMaxLineWidth( double& aMaxLineWidth );
    bool SetMaxLineWidth( double aMaxLineWidth );

    // R. Creation Date: read only
    bool GetCreationDate( char const*& aDate );

    // S. Min. Intended Resolution
    bool GetMinResolution( double& aMinRes );
    bool SetMinResolution( double aMinRes );

    // T. Max. coordinate value: not exposed; everyone uses default 0.0
    // U. Author
    bool GetAuthor( char const*& anAuthor );
    bool SetAuthor( char const* anAuthor );

    // V. Organization
    bool GetOrganization( char const*& anOrganization );
    bool SetOrganization( char const* anOrganization );

    // W. IGES Version: read only
    bool GetIGESVersionFlag( int& aIGESVersion );

    // X. Drafting Standard
    bool GetDraftingStandard( IGES_DRAFTING_STANDARD& aDraftingStd );
    bool SetDraftingStandard( IGES_DRAFTING_STANDARD aDraftingStd );

    // Y. Modification Date: read only
    bool GetModificationDate( char const*& aDate );

    // Z. Application Note
    bool GetApplicationNote( char const*& aNote );
    bool SetApplicationNote( char const* aNote );
};

#endif  // DLL_IGES_H
