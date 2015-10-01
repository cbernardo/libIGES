/*
 * file: dll_iges_entity.h
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


#ifndef DLL_IGES_ENTITY_H
#define DLL_IGES_ENTITY_H

#include <libigesconf.h>
#include <iges_base.h>
#include <dll_iges.h>

class  IGES;
class  DLL_IGES;            // Overarching data structure and parent to all entities
class  DLL_IGES_ENTITY_124; // Transform entity
class  IGES_ENTITY;
class DLL_IGES_ENTITY_314;


/**
 * Class IGES_ENTITY
 * base class for all IGES entities.
 */
class MCAD_API DLL_IGES_ENTITY
{
protected:
    IGES* m_parent;
    IGES_ENTITY* m_entity;
    bool         m_valid;       // set to false if m_entity is deleted
    bool         m_hasParent;   // set to false if no parent or m_parent is deleted
    IGES_ENTITY_TYPE m_type;    // IGES type exposed by this class

public:
    // note: all implementations must have constructors which:
    // a. takes a IGES* and creation flag
    // b. takes a DLL_IGES& and creation flag
    // If the creation flag is 'true' then a new IGES_ENTITY
    // of the specified type is created; otherwise the DLL entity
    // serves as a convenient manipulator of an IGES_ENTITY to
    // be specified by the user.
    DLL_IGES_ENTITY( IGES* aParent );
    DLL_IGES_ENTITY( DLL_IGES& aParent );
    virtual ~DLL_IGES_ENTITY();

    virtual bool NewEntity( void ) = 0;
    bool SetAPIParentIGES( IGES* aParent );
    bool SetAPIParentIGES( DLL_IGES& aParent );

    /**
     * Function GetEntityType
     * returns the type of IGES entity exposed by this object
     */
    IGES_ENTITY_TYPE GetEntityType() const;

    /**
     * Function GetRawPtr()
     * returns the internal IGES_ENTITY pointer
     */
    IGES_ENTITY* GetRawPtr( void );

    /**
     * Function IsValid
     * returns true if the object holds a valid IGES_ENTITY pointer
     */
    bool IsValid( void );


    /**
     * Function Detach
     * detaches the DLL_IGES_ENTITY from the IGES_ENTITY pointer which it
     * holds and returns the value of that pointer. This is useful in
     * situations where we wish to delete the DLL_IGES_ENTITY instance
     * but preserve the related entity object.
     */
    IGES_ENTITY* Detach( void );
    void DelEntity( void );

    /**
     * Function Attach
     * associates the DLL_IGES_ENTITY with the given IGES_ENTITY.
     *
     * @param aEntity is a pointer to the entity to be associated
     * with this API wrapper
     * @return true if the operation succeeds
     */
    bool Attach( IGES_ENTITY* aEntity );

    // Routines for manipulating extra entity list

    /**
     * Function GetNOptionalEntities
     * retrieves the number of optional (extra) entities associated
     * with this entity.
     */
    bool GetNOptionalEntities( int& aNOptEnt );


    /**
     * Function GetOptionalEntities
     * retrieves a pointer to the internal list of optional (extra)
     * entities associated with this entity.
     *
     * @return false if there is no valid entity or the underlying operation failed.
     * Note that if the return value is true but there were no optional entities or
     * the index was invalid, the pointer value will be set to NULL.
     */
    bool GetOptionalEntities( size_t& aListSize, IGES_ENTITY**& aEntityList );

    /**
     * Function AddOptionalEntity
     * adds the given IGES entity to the list of optional (extra)
     * entities associated with this entity and returns true on success.
     *
     * @param = a pointer to the optional IGES entity to be associated
     * @return false if there is no valid entity or the underlying operation failed.
     */
    bool AddOptionalEntity( IGES_ENTITY* aEntity );
    bool AddOptionalEntity( DLL_IGES_ENTITY*& aEntity );


    /**
     * Function DelOptionalEntity
     * removes the given IGES entity from the list of optional (extra)
     * entities associated with this entity and returns true on success.
     * The entity is not destroyed, only removed from the internal list
     * of optional entities.
     *
     * @param aEntity = a pointer to the optional IGES entity to be disassociated
     * @return false if there is no valid entity or the underlying operation failed.
     */
    bool DelOptionalEntity( IGES_ENTITY* aEntity );
    bool DelOptionalEntity( DLL_IGES_ENTITY*& aEntity );


    // Routines for manipulating the optional comments

    /**
     * Function GetNComments
     * retrieves the number of optional comments for this entity
     *
     * @param nComments will hold the number of comments
     * @return false if there is no valid entity
     */
    bool GetNComments( int& nComments );


    /**
     * Function GetComments
     * retrieves a pointer to the internal list of optional
     * comments associated with this entity.
     *
     * @param aListSize will hold the number of comment items
     * @param aCommentList will point to the array of comments
     * @return false if there is no valid entity
     */
    bool GetComments( size_t& aListSize, char const**& aCommentList );


    /**
     * Function AddComment
     * adds the given comment to the list of optional comments
     * associated with this entity and returns true on success.
     *
     * @param aComment = comment to be added
     * @return false if there is no valid entity or no data
     */
    bool AddComment( const char*& aComment );


    /**
     * Function DelComment
     * deletes the given optional comment with the specified index
     * and returns true on success.
     *
     * @param index = index to comment to be removed
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool DelComment( int index );


    /**
     * Function ClearComments
     * deletes all optional comments associated with this entity
     * @return false if there is no valid entity
     */
    bool ClearComments( void );


    /**
     * Function SetParentIGES
     * sets the parent object of the underlying entity; the parent must
     * be a valid instance of the IGES class; returns true on success.
     *
     * @param aParent = an instance of the IGES class
     * @return false if there is no valid entity
     */
    bool SetParentIGES( IGES* aParent );
    bool SetParentIGES( DLL_IGES*& aParent );


    /**
     * Function GetParentIGES
     * retrieves a pointer to the underlying entity's parent IGES object
     *
     * @param aParent [out] will hold a pointer to the parent IGES (if any)
     * @return false if there is no valid entity
     */
    bool GetParentIGES( IGES*& aParent );


    /**
     * Function GetEntityType
     * returns the enumerated value of the IGES Entity type;
     * a value of zero is a special case and may represent a
     * NULL Entity as per the IGES specification or an entity
     * which is currently not supported by the library.
     *
     * @param aType [out] will hold the underlying entity's type
     * enumerator; if a valid entity exists but the return value
     * is '0' then the underlying IGES Entity may not be cirrently
     * supported; in such a case invoke 'GetTrueEntityType()' to
     * retrieve the entity type -- if the return value is non-zero
     * then the entity is not currently supported but if the type
     * is zero then the item had been set to NULL ENTITY within an
     * input IGES file.
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetEntityType( int& aType );


    /**
     * Function GetEntityForm
     * retrieves the Form number of this entity.
     *
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetEntityForm( int& aForm );


    /**
     * Function SetEntityForm
     * sets the Form number of this entity and returns true on success.
     *
     * @param aForm = the value to assign to this entity's Form number
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetEntityForm( int aForm );


    /**
     * Function SetStructure
     * sets the entity referenced through the Directory Entry's
     * Structure parameter and returns true on success.
     *
     * @param aStructure = pointer to the entity to associate
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetStructure( IGES_ENTITY* aStructure );
    bool SetStructure( DLL_IGES_ENTITY*& aStructure );


    /**
     * Function GetStructure
     * retrieves the entity referenced by the Directory Entry's
     * Structure parameter and returns true if the entity
     * has a Structure entity.
     *
     * @param aStructure = a handle to store a pointer to the Structure object
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetStructure( IGES_ENTITY*& aStructure );


    /**
     * Function SetLineFontPattern
     * sets the LineFont Pattern according to the enumerated options
     * in the IGES specification and returns true on success.
     *
     * @param aPattern = an IGES_LINEFONT_PATTERN enumeration
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );


    /**
     * Function SetLineFontPattern
     * sets the LineFont Pattern via a LineFontPattern entity
     * and returns true on success.
     *
     * @param aPattern = a pointer to an IGES LineFontPattern entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLineFontPattern( DLL_IGES_ENTITY*& aPattern );
    bool SetLineFontPattern( IGES_ENTITY* aPattern );


    /**
     * Function GetLineFontPattern
     * retrieves an enumerated LineFontPattern and returns true on success;
     * if the LineFontPattern is specified by an entity rather than an
     * enumeration the return value is false.
     *
     * @param aPattern = variable to store the numeric LineFontPattern value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLineFontPattern( IGES_LINEFONT_PATTERN& aPattern );


    /**
     * Function GetLineFontPatternEntity
     * retrieves a pointer to the referenced LineFontPattern Entity and
     * returns true for success. In the case of the LineFontPattern being
     * an enumeration with the value 0, the return parameter is set to
     * NULL and the function returns true; if the line font pattern is
     * set via a non-zero enumeration then the function returns false.
     *
     * @param aPattern = handle to store the associated LineFontPattern entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLineFontPatternEntity( IGES_ENTITY*& aPattern );


    /**
     * Function SetLevel
     * sets the numeric Entity Level of this entity and return true on success;
     * all entities initially default to zero.
     *
     * @param aLevel = the numeric level to assign to this entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLevel( int aLevel );


    /**
     * Function SetLevel
     * sets the Entity Level via a referenced Property Entity (Type 406, Form 1)
     * returns true on success.
     *
     * @param aLevel = pointer to the Property Entity to be associated
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLevel( DLL_IGES_ENTITY*& aLevel );
    bool SetLevel( IGES_ENTITY* aLevel );


    /**
     * Function GetLevel
     * retrieves the numeric Entity Level of this entity and return true on success;
     * if the level is determined via association with a Property Entity then
     * this function returns false,
     *
     * @param aLevel = variable to store the numeric level value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLevel( int& aLevel );


    /**
     * Function GetLevelEntity
     * retrieves the Property Entity associated with this entity and returns
     * true on success; if the level is set numerically then this function
     * returns false.
     *
     * @param aLevel = handle to store a pointer to the associated Property Entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLevelEntity( IGES_ENTITY*& aLevel );


    /**
     * Function SetView
     * sets the associated VIEW or ASSOCIATIVITY INSTANCE entity
     * and returns true on success.
     *
     * @param aView = the VIEW or ASSOCIATIVITY INSTANCE to be associated
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetView( DLL_IGES_ENTITY*& aView );
    bool SetView( IGES_ENTITY* aView );


    /**
     * Function GetView
     * retrieves an associated VIEW or ASSOCIATIVITY INSTANCE entity
     * and returns true on success; if there is no such associated
     * entity the return value is false.
     *
     * @param aView = handle to store pointer to VIEW or ASSOCIATIVITY INSTANCE entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetView( IGES_ENTITY*& aView );


    /**
     * Function SetTransform
     * sets the associated Transformation Entity and returns true
     * on success; not all entities may accept a transform in which
     * case the return value will be false.
     *
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetTransform( DLL_IGES_ENTITY*& aTransform );
    bool SetTransform( IGES_ENTITY* aTransform );

    /**
     * Function GetTransform
     * retrieves an associated Transformation Entity and
     * returns true if such an entity exists; otherwise the
     * return parameter is set to NULL and the return value is
     * set to false.
     *
     * @param aTransform = handle to store a pointer to the object's Transformation Entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetTransform( IGES_ENTITY*& aTransform );


    /**
     * Function SetLabelAssoc
     * sets the ASSOCIATIVITY INSTANCE entity which refers to this entity
     * and returns true on success.
     *
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLabelAssoc( DLL_IGES_ENTITY*& aLabelAssoc );
    bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );


    /**
     * Function GetLabelAssoc
     * retrieves the ASSOCIATIVITY INSTANCE entity which refers to this entity
     * and returns true on success; if no such entity exists the return
     * parameter is set to NULL and the return value is false.
     *
     * @param aLabelAssoc = handle to store a pointer to an ASSOCIATIVITY INSTANCE entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLabelAssoc( IGES_ENTITY*& aLabelAssoc );


    /**
     * Function SetColor
     * sets the entity color to a preset value as enumerated in
     * the IGES specification and returns true on success.
     *
     * @param aColor = enumerated color definition as per IGES specification
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetColor( IGES_COLOR aColor );


    /**
     * Function SetColor
     * sets the entity color according to a Color Definition Entity
     * and returns true on success.
     *
     * @param aColor = pointer to a Color Definition entity which defines this entity's color.
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetColor( DLL_IGES_ENTITY_314*& aColor );
    bool SetColor( IGES_ENTITY* aColor );

    /**
     * Function GetColor
     * retrieves an enumerated value representing the color of this
     * entity and returns true on success; if the color is controlled
     * by a Color Definition Entity then this function returns false.
     * In the case of entities which do not support the color parameter
     * this function will return true but the color value will always
     * be zero.
     *
     * @param aColor = variable to store the enumerated color value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetColor( IGES_COLOR& aColor );


    /**
     * Function GetColorEntity
     * retrieves a pointer to the Color Entity referenced by this object,
     * if any, and returns true on success. If the color is set by a
     * numeric value or the entity does not support color then the
     * return parameter will be set to NULL and the return value will be
     * false.
     *
     * @param aColor = handle to store a pointer to the associated Color Definition Entity
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetColorEntity( IGES_ENTITY*& aColor );


    /**
     * Function SetLineWeightNum
     * sets the Line Weight Number of this entity; the number must be
     * an integer from 0 to the value of the Max Linewidth Gradation as
     * stored in the IGES file's Global Section or as set in the
     * parent IGES object's globalData.maxLinewidthGrad member. The
     * function returns true on success.
     *
     * @param aLineWeight = line weight to assign
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLineWeightNum( int aLineWeight );


    /**
     * Function GetLineWeightNum
     * retrieves the line weight value of this entity and
     * returns true on success.
     *
     * @param aLineWeight = variable to store the line weight value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLineWeightNum( int& aLineWeight );


    /**
     * Function SetLabel
     * sets the max, 8-character label which is stored in this
     * entity's Directory Entry and return true on success.
     * An excessively long label will be truncated to 8
     * characters.
     *
     * @param aLabel = the text to use in the label
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetLabel( const char* aLabel );


    /**
     * Function GetLabel
     * retrieves the 8-character optional label associated
     * with this entity.
     *
     * @param aLabel is a pointer to the label text
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetLabel( char const*& aLabel);


    /**
     * Function SetEntitySubscript
     * sets the subscript associated with the max. 8-character Entity Label
     * and returns true on success. The subscript must be a positive max.
     * 8-digit number (0 .. 99999999). The IGES specification intended for
     * the Entity Label in conjunction with the subscript to provide a
     * unique ID for each instance of an entity. In practice the 8-character
     * limit on the label is too restrictive and thus the ID scheme is
     * ignored by most contemporary MCAD systems.
     *
     * @param aSubscript = the value to assign to the entity subscript
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetEntitySubscript( int aSubscript );


    /**
     * Function GetEntitySubscript
     * retrieves the subscript associated with this entity's label and
     * returns true.
     *
     * @param aSubscript = variable to store the value of the entity subscript
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetEntitySubscript( int& aSubscript );


    /**
     * Function SetVisibility
     * sets whether the entity should be visible or not and returns true
     * on success. Not all entities support the visibility parameter in
     * which case the function will return false. The Visibility flag is
     * just one part of the Directory Entry Status Number.
     *
     * @param isVisible = set to true to set the entity's visibility flag
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetVisibility( bool isVisible );


    /**
     * Function GetVisibility
     * retrieves the visibility flag of this entity and returns true.
     *
     * @param isVisible = variable to store the state of the visibility flag
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetVisibility( bool& isVisible );


    /**
     * Function SetDependency
     * sets the dependency flag of the entity and returns true
     * on success; the function will return false if the provided
     * value is not allowed by the entity. The dependency flag is
     * just one part of the Directory Entry Status Number
     *
     * @param aDependency = the dependency value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetDependency( IGES_STAT_DEPENDS aDependency );


    /**
     * Function GetDependency
     * retrieves the entity's dependency value and returns true.
     *
     * @param aDependency = variable to store the dependency value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetDependency( IGES_STAT_DEPENDS& aDependency );


    /**
     * Function SetEntityUse
     * sets the Use Case flag of the entity and returns true on success.
     * Not all entities support all use case flags. The Use Case flag
     * is just one part of the Directory Entry Status Number
     *
     * @param aUseCase = Use Case flag as per IGES specification
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetEntityUse( IGES_STAT_USE aUseCase );

    /**
     * Function GetEntityUse
     * retrieves the value of the entity's Use Case flag and
     * returns true.
     *
     * @param aUseCase = variable to store the use case flag
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetEntityUse( IGES_STAT_USE& aUseCase );


    /**
     * Function SetHierarchy
     * sets the entity's Hierarchy Flag and returns true on success.
     * The Hierarchy flag is just one part of the Directory Entry
     * Status Number.  Not all entities support all hierarchy flag
     * values.
     *
     * @param aHierarchy = hierarchy flag value to use
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool SetHierarchy( IGES_STAT_HIER aHierarchy );


    /**
     * Function GetHierarchy
     * retrieves the value of the hierarchy flag and returns true.
     *
     * @param aHierarchy = variable to store the hierarchy flag value
     * @return false if there is no valid entity or the underlying
     * operation failed.
     */
    bool GetHierarchy( IGES_STAT_HIER& aHierarchy );
};

#endif  // DLL_IGES_ENTITY_H
