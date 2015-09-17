/*
 * file: iges_entity.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: Base entity of all IGES Entity classes.
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


#ifndef IGES_ENTITY_H
#define IGES_ENTITY_H

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include <libigesconf.h>
#include <iges_base.h>

class IGES;             // Overarching data structure and parent to all entities
struct IGES_RECORD;     // Partially parsed single line of data from an IGES file
class IGES_ENTITY_124;  // Transform entity

/**
 * Class IGES_ENTITY
 * base class for all IGES entities.
 */
class MCAD_API IGES_ENTITY
{
protected:
    IGES*               parent;             //< master IGES object; contains globals and manages entity I/O
    int                 entityType;         //< #, Entity Type (values are somewhat restricted but Implementor Macros require 'int' rather than 'enum')
    int                 parameterData;      //< P, first sequence number of associated parameterData
    int                 structure;          //< 0P, index to DirEnt of the definition entity which specifies this entity's meaning
    int                 lineFontPattern;    //< 0#P, 0 (def), Line font pattern number, or index to Line Font Definition (304)
    int                 level;              //< 0#P, Level number for this entity, else index to Definition Levels Property (406-1)
    int                 view;               //< 0P, 0 (def) or index to DirEnt for one of (a) View Entity (410) or (b) Views Visible Associativity Instance (402-3/4/19)
    int                 transform;          //< 0P, 0 (def) or index to Transformation Matrix (124)
    int                 labelAssoc;         //< 0P, 0 (def) or index to label Display Associativity (402-5)
    bool                visible;            //< Status Number: Blank Status (default 0: visible == true)
    IGES_STAT_DEPENDS   depends;            //< Status Number: Subordinate Entity Switch (default 0 = independent)
    IGES_STAT_USE       use;                //< Status Number: Entity Use (default 0 = Geometry)
    IGES_STAT_HIER      hierarchy;          //< Status Number: Hierarchy (default 0 = all DE attributes apply to subordinates)
    int                 lineWeightNum;      //< #, System line width thickness, 0 .. Global::maxLinewidthGrad
    int                 colorNum;           //< #P, 0 (def), Color ID, or index to Color Definition (314)
    int                 paramLineCount;     //< #, number of associated Parameter Lines
    int                 form;               //< 0#, 0 (def) or Form Number for entities with more than one form
    std::string         label;              //< max. 8 character alphanumeric label
    int                 entitySubscript;    //< #, 1..8 digit unsigned int associated with the label

    // pointers to be linked to other entities as necessary
    IGES_ENTITY*     pStructure;
    IGES_ENTITY*     pLineFontPattern;
    IGES_ENTITY*     pLevel;
    IGES_ENTITY*     pView;
    IGES_ENTITY_124* pTransform;
    IGES_ENTITY*     pLabelAssoc;
    IGES_ENTITY*     pColor;

    /// list of referring (parent) entities
    std::list<IGES_ENTITY*> refs;
    /// list of extra entities (optional PD entries)
    std::list<IGES_ENTITY*> extras;
    std::list<int> iExtras;
    /// list of optional comments
    std::list<std::string> comments;
    /// data formatted for output (also used for reading PDs from file)
    std::string pdout;

    friend class IGES;
    int sequenceNumber;     //< first sequence number of this entity's Directory Entry
    bool massoc;            //< set true after associate() is invoked


    /**
     * Function format
     * prepares data for writing; Parameter Data is formatted using @param index and
     * Directory Entry items are updated; each Entity must have been previously assigned
     * a correct DE Sequence Number before invoking this function. The function returns
     * true to indicate success.
     *
     * @param index = (I/O) current Parameter Data Index
     */
    virtual bool format( int &index ) = 0;


    /**
     * Function unformat
     * clears any Parameter Data which has been read in or prepared for output.
     */
    void         unformat( void );


    /**
     * Function readExtraParams
     * reads optional (extra) PD parameters and returns true on success.
     *
     * @param index = (I/O) current Parameter Data Index
     */
    bool readExtraParams( int& index );


    /**
     * Function readComments
     * reads optional comments associated with the entity and returns true on success.
     *
     * @param index = (I/O) current Parameter Data Index
     */
    bool readComments( int& index );


    /**
     * Function formatExtraParams
     * formats any optional extra entity parameters for output and returns true on success.
     *
     * @param fStr = current Parameter Data entry under construction
     * @param pdSeq = (I/O) current Parameter Data Index
     * @param pd = IGES Parameter Delimeter
     * @param rd = IGES Record Delimeter
     */
    bool formatExtraParams( std::string& fStr, int& pdSeq, char pd, char rd );


    /**
     * Function formatComments
     * formats any optional entity comments for output and returns true on success.
     *
     * @param pdSeq = (I/O) current Parameter Data Index
     */
    bool formatComments( int& pdSeq );


    /**
     * Function rescale
     * changes the internal scale; this routine may be invoked by the parent IGES object
     * to change the internal units or the Model Scale; returns true on success.
     *
     * @param sf = scaling factor to apply to data
     */
    virtual bool rescale( double sf ) = 0;

public:
    // public functions which must only be used internally by libIGES

    /**
     * Function getNRefs
     * returns the number of unique parent entities referring to this entity
     */
    size_t       getNRefs( void );


    /**
     * Function getDESequence
     * returns the first Directory Entry sequence associated with this entity;
     * the returned value is only guaranteed to be valid immediately after
     * reading or writing an IGES file.
     */
    int          getDESequence( void );


    /**
     * Function getFirstParentRef
     * returns a pointer to the first parent entity in this
     * entity's Reference List. If the entity has no parents
     * then NULL is returned.  This function is used internally
     * to decide how the parameters of a NURBS curve should be
     * scaled.
     */
    IGES_ENTITY* getFirstParentRef( void );


    /**
     * Function associate
     * associates DE pointers with other entities after reading all data;
     * retrictions on types must be enforced to ensure data integrity and
     * software stability. Returns true on success.
     *
     * @param entities = vector of all instantiated entities
     */
    virtual bool associate(std::vector<IGES_ENTITY *> *entities) = 0;


    // Routines to manage reference deletion

    /**
     * Function unlink
     * removes a child entity from the parent's list of children and
     * returns true on success.
     *
     * @param aChild = pointer to child entity to disassociate from
     * the parent.
     */
    virtual bool unlink(IGES_ENTITY *aChild) = 0;


    // Add/DelReference is needed for management of StatusNumber

    /**
     * Function addReference
     * adds a reference to a parent entity and returns true on
     * success.
     *
     * @param aParentEntity = pointer to the parent IGES entity to be registered
     * @param isDuplicate = set to true if this entity already has a reference to aParentEntity
     */
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate) = 0;


    /**
     * Function DelReference
     * deletes all references to a parent and returns true on success.
     *
     * @param aParentEntity = the parent entity to be disassociated
     */
    virtual bool delReference(IGES_ENTITY *aParentEntity) = 0;


    /**
     * Function IsOrphaned
     * returns true if the entity is orphaned and may be deleted
     * without affecting the integrity of the IGES file.
     */
    virtual bool isOrphaned( void ) = 0;


    /**
     * Function readDE
     * read the Directory Entry data starting at the given record;
     * return true on success.
     *
     * @param aRecord = first DE record for the entity
     * @param aFile = IGES input file
     * @param aSequenceVar = (I/O) current DE sequence number
     */
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar) = 0;


    /**
     * Function readPD
     * reads the Parameter Data from sequential records starting at
     * the current position in the input stream; returns true on
     * success.
     *
     * @param aFile = the IGES input file
     * @param aSequenceVar = (I/O) the current Parameter Data sequence number
     */
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar) = 0;


    /**
     * Function writeDE
     * writes out a Directory Entry for this entity and returns true
     * on success. Before invoking this function the sequenceNumber
     * member must be correctly set and the format() function must
     * have been called on every instantiated entity.
     *
     * @param aFile = IGES output file
     */
    virtual bool writeDE(std::ofstream &aFile);


    /**
     * Function writePD
     * writes out a Parameter Data block; prior to invoking this method a valid
     * Sequence Number must have been assigned and the format() method invoked;
     * returns true on success.
     *
     * @param aFile = IGES output file
     */
    virtual bool writePD(std::ofstream &aFile);


public:
    IGES_ENTITY(IGES* aParent);
    virtual ~IGES_ENTITY();

    // Routines for manipulating extra entity list

    /**
     * Function GetNOptionalEntities
     * returns the number of optional (extra) entities associated
     * with this entity.
     */
    int GetNOptionalEntities( void );


    /**
     * Function GetOptionalEntities
     * returns a pointer to the internal list of optional (extra)
     * entities associated with this entity.
     */
    std::list<IGES_ENTITY*>* GetOptionalEntities( void );


    /**
     * Function AddOptionalEntity
     * adds the given IGES entity to the list of optional (extra)
     * entities associated with this entity and returns true on success.
     *
     * @param = a pointer to the optional IGES entity to be associated
     */
    bool AddOptionalEntity( IGES_ENTITY* aEntity );


    /**
     * Function DelOptionalEntity
     * removes the given IGES entity from the list of optional (extra)
     * entities associated with this entity and returns true on success.
     *
     * @param aEntity = a pointer to the optional IGES entity to be disassociated
     */
    bool DelOptionalEntity( IGES_ENTITY* aEntity );


    // Routines for manipulating the optional comments

    /**
     * Function GetNComments
     * returns the number of optional comments for this entity
     */
    int GetNComments( void );


    /**
     * Function GetComments
     * returns a pointer to the internal list of optional
     * comments associated with this entity.
     */
    std::list<std::string>* GetComments( void );


    /**
     * Function AddComment
     * adds the given comment to the list of optional comments
     * associated with this entity and returns true on success.
     *
     * @param aComment = comment to be added
     */
    bool AddComment( const std::string& aComment );


    /**
     * Function DelComment
     * deletes the given optional comment with the specified index
     * and returns true on success.
     *
     * @param index = index to comment to be removed
     */
    bool DelComment( int index );


    /**
     * Function ClearComments
     * deletes all optional comments associated with this entity
     */
    bool ClearComments( void );


    /**
     * Function SetParentIGES
     * sets the parent object; the parent must be a valid instance
     * of the IGES class; returns true on success.
     *
     * @param aParent = an instance of the IGES class
     */
    bool SetParentIGES( IGES* aParent );


    /**
     * Function GetParentIGES
     * returns a pointer to the parent IGES object
     */
    IGES* GetParentIGES( void );


    /**
     * Function GetEntityType
     * returns the enumerated value of the IGES Entity type;
     * a value of zero is a special case and may represent a
     * NULL Entity as per the IGES specification or an entity
     * which is currently not supported by the library.
     */
    int          GetEntityType( void );


    /**
     * Function GetEntityForm
     * returns the Form number of this entity.
     */
    int          GetEntityForm( void );


    /**
     * Function SetEntityForm
     * sets the Form number of this entity and returns true on success.
     *
     * @param aForm = the value to assign to this entity's Form number
     */
    virtual bool SetEntityForm( int aForm ) = 0;


    /**
     * Function SetStructure
     * sets the entity referenced through the Directory Entry's
     * Structure parameter and returns true on success.
     *
     * @param aStructure = pointer to the entity to associate
     */
    virtual bool SetStructure( IGES_ENTITY* aStructure );


    /**
     * Function GetStructure
     * retrieves the entity referenced by the Directory Entry's
     * Structure parameter and returns true if the entity
     * has a Structure entity.
     *
     * @param aStructure = a handle to store a pointer to the Structure object
     */
    virtual bool GetStructure( IGES_ENTITY** aStructure );


    /**
     * Function SetLineFontPattern
     * sets the LineFont Pattern according to the enumerated options
     * in the IGES specification and returns true on success.
     *
     * @param aPattern = an IGES_LINEFONT_PATTERN enumeration
     */
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );


    /**
     * Function SetLineFontPattern
     * sets the LineFont Pattern via a LineFontPattern entity
     * and returns true on success.
     *
     * @param aPattern = a pointer to an IGES LineFontPattern entity
     */
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );


    /**
     * Function GetLineFontPattern
     * retrieves an enumerated LineFontPattern and returns true on success;
     * if the LineFontPattern is specified by an entity rather than an
     * enumeration the return value is false.
     *
     * @param aPattern = variable to store the numeric LineFontPattern value
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
     */
    bool GetLineFontPatternEntity( IGES_ENTITY** aPattern );


    /**
     * Function SetLevel
     * sets the numeric Entity Level of this entity and return true on success;
     * all entities initially default to zero.
     *
     * @param aLevel = the numeric level to assign to this entity
     */
    virtual bool SetLevel( int aLevel );


    /**
     * Function SetLevel
     * sets the Entity Level via a referenced Property Entity (Type 406, Form 1)
     * returns true on success.
     *
     * @param aLevel = pointer to the Property Entity to be associated
     */
    virtual bool SetLevel( IGES_ENTITY* aLevel );


    /**
     * Function GetLevel
     * retrieves the numeric Entity Level of this entity and return true on success;
     * if the level is determined via association with a Property Entity then
     * this function returns false,
     *
     * @param aLevel = variable to store the numeric level value
     */
    bool         GetLevel( int& aLevel );


    /**
     * Function GetLevelEntity
     * retrieves the Property Entity associated with this entity and returns
     * true on success; if the level is set numerically then this function
     * returns false.
     *
     * @param aLevel = handle to store a pointer to the associated Property Entity
     */
    bool         GetLevelEntity( IGES_ENTITY** aLevel );


    /**
     * Function SetView
     * sets the associated VIEW or ASSOCIATIVITY INSTANCE entity
     * and returns true on success.
     *
     * @param aView = the VIEW or ASSOCIATIVITY INSTANCE to be associated
     */
    virtual bool SetView( IGES_ENTITY* aView );


    /**
     * Function GetView
     * retrieves an associated VIEW or ASSOCIATIVITY INSTANCE entity
     * and returns true on success; if there is no such associated
     * entity the return value is false.
     *
     * @param aView = handle to store pointer to VIEW or ASSOCIATIVITY INSTANCE entity
     */
    bool         GetView( IGES_ENTITY** aView );


    /**
     * Function SetTransform
     * sets the associated Transformation Entity and returns true
     * on success; not all entities may accept a transform in which
     * case the return value will be false.
     */
    virtual bool SetTransform( IGES_ENTITY* aTransform );

    /**
     * Function GetTransform
     * retrieves an associated Transformation Entity and
     * returns true if such an entity exists; otherwise the
     * return parameter is set to NULL and the return value is
     * set to false.
     *
     * @param aTransform = handle to store a pointer to the object's Transformation Entity
     */
    bool         GetTransform( IGES_ENTITY** aTransform );


    /**
     * Function SetLabelAssoc
     * sets the ASSOCIATIVITY INSTANCE entity which refers to this entity
     * and returns true on success.
     */
    virtual bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );


    /**
     * Function GetLabelAssoc
     * retrieves the ASSOCIATIVITY INSTANCE entity which refers to this entity
     * and returns true on success; if no such entity exists the return
     * parameter is set to NULL and the return value is false.
     *
     * @param aLabelAssoc = handle to store a pointer to an ASSOCIATIVITY INSTANCE entity
     */
    bool         GetLabelAssoc( IGES_ENTITY** aLabelAssoc );


    /**
     * Function SetColor
     * sets the entity color to a preset value as enumerated in
     * the IGES specification and returns true on success.
     *
     * @param aColor = enumerated color definition as per IGES specification
     */
    virtual bool SetColor( IGES_COLOR aColor );


    /**
     * Function SetColor
     * sets the entity color according to a Color Definition Entity
     * and returns true on success.
     *
     * @param aColor = pointer to a Color Definition entity which defines this entity's color.
     */
    virtual bool SetColor( IGES_ENTITY* aColor );

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
     */
    bool         GetColor( IGES_COLOR& aColor );


    /**
     * Function GetColorEntity
     * retrieves a pointer to the Color Entity referenced by this object,
     * if any, and returns true on success. If the color is set by a
     * numeric value or the entity does not support color then the
     * return parameter will be set to NULL and the return value will be
     * false.
     *
     * @param aColor = handle to store a pointer to the associated Color Definition Entity
     */
    bool         GetColorEntity( IGES_ENTITY** aColor );


    /**
     * Function SetLineWeightNum
     * sets the Line Weight Number of this entity; the number must be
     * an integer from 0 to the value of the Max Linewidth Gradation as
     * stored in the IGES file's Global Section or as set in the
     * parent IGES object's globalData.maxLinewidthGrad member. The
     * function returns true on success.
     *
     * @param aLineWeight = line weight to assign
     */
    virtual bool SetLineWeightNum( int aLineWeight );


    /**
     * Function GetLineWeightNum
     * retrieves the line weight value of this entity and
     * returns true on success.
     *
     * @param aLineWeight = variable to store the line weight value
     */
    bool         GetLineWeightNum( int& aLineWeight );


    /**
     * Function SetLabel
     * sets the max, 8-character label which is stored in this
     * entity's Directory Entry and return true on success.
     * An excessively long label will be truncated to 8
     * characters.
     *
     * @param aLabel = the text to use in the label
     */
    bool SetLabel( const std::string aLabel );


    /**
     * Function GetLabel
     * retrieves the 8-character optional label associated
     * with this entity and returns true on success. If there
     * is no label text then the function returns false.
     *
     * @param aLabel = variable to store the label value.
     */
    void GetLabel( std::string& aLabel );


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
     */
    bool SetEntitySubscript( int aSubscript );


    /**
     * Function GetEntitySubscript
     * retrieves the subscript associated with this entity's label and
     * returns true.
     *
     * @param aSubscript = variable to store the value of the entity subscript
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
     */
    virtual bool SetVisibility( bool isVisible );


    /**
     * Function GetVisibility
     * retrieves the visibility flag of this entity and returns true.
     *
     * @param isVisible = variable to store the state of the visibility flag
     */
    bool         GetVisibility( bool& isVisible );


    /**
     * Function SetDependency
     * sets the dependency flag of the entity and returns true
     * on success; the function will return false if the provided
     * value is not allowed by the entity. The dependency flag is
     * just one part of the Directory Entry Status Number
     *
     * @param aDependency = the dependency value
     */
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );


    /**
     * Function GetDependency
     * retrieves the entity's dependency value and returns true.
     *
     * @param aDependency = variable to store the dependency value
     */
    bool         GetDependency( IGES_STAT_DEPENDS& aDependency );


    /**
     * Function SetEntityUse
     * sets the Use Case flag of the entity and returns true on success.
     * Not all entities support all use case flags. The Use Case flag
     * is just one part of the Directory Entry Status Number
     *
     * @param aUseCase = Use Case flag as per IGES specification
     */
    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );

    /**
     * Function GetEntityUse
     * retrieves the value of the entity's Use Case flag and
     * returns true.
     *
     * @param aUseCase = variable to store the use case flag
     */
    bool         GetEntityUse( IGES_STAT_USE& aUseCase );


    /**
     * Function SetHierarchy
     * sets the entity's Hierarchy Flag and returns true on success.
     * The Hierarchy flag is just one part of the Directory Entry
     * Status Number.  Not all entities support all hierarchy flag
     * values.
     *
     * @param aHierarchy = hierarchy flag value to use
     */
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );


    /**
     * Function GetHierarchy
     * retrieves the value of the hierarchy flag and returns true.
     *
     * @param aHierarchy = variable to store the hierarchy flag value
     */
    bool         GetHierarchy( IGES_STAT_HIER& aHierarchy );
};

#endif  // IGES_ENTITY_H
