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

#include "iges_base.h"

class IGES;             // Overarching data structure and parent to all entities
struct IGES_RECORD;     // Partially parsed single line of data from an IGES file

// Base class for all IGES entities
class IGES_ENTITY
{
protected:
    IGES*               parent;             // master IGES object; contains globals and manages entity I/O
    int                 entityType;         // #, Entity Type (values are somewhat restricted but Implementor Macros require 'int' rather than 'enum')
    int                 parameterData;      // P, first sequence number of associated parameterData
    int                 structure;          // 0P, index to DirEnt of the definition entity which specifies this entity's meaning
    int                 lineFontPattern;    // 0#P, 0 (def), Line font pattern number, or index to Line Font Definition (304)
    int                 level;              // 0#P, Level number for this entity, else index to Definition Levels Property (406-1)
    int                 view;               // 0P, 0 (def) or index to DirEnt for one of (a) View Entity (410) or (b) Views Visible Associativity Instance (402-3/4/19)
    int                 transform;          // 0P, 0 (def) or index to Transformation Matrix (124)
    int                 labelAssoc;         // 0P, 0 (def) or index to label Display Associativity (402-5)
    bool                visible;            // Status Number: Blank Status (default 0: visible == true)
    IGES_STAT_DEPENDS   depends;            // Status Number: Subordinate Entity Switch (default 0 = independent)
    IGES_STAT_USE       use;                // Status Number: Entity Use (default 0 = Geometry)
    IGES_STAT_HIER      hierarchy;          // Status Number: Hierarchy (default 0 = all DE attributes apply to subordinates)
    int                 lineWeightNum;      // #, System line width thickness, 0 .. Global::maxLinewidthGrad
    int                 colorNum;           // #P, 0 (def), Color ID, or index to Color Definition (314)
    int                 paramLineCount;     // #, number of associated Parameter Lines
    int                 form;               // 0#, 0 (def) or Form Number for entities with more than one form
    std::string         label;              // max. 8 character alphanumeric label
    int                 entitySubscript;    // #, 1..8 digit unsigned int associated with the label

    // pointers to be linked to other entities as necessary
    IGES_ENTITY* pStructure;
    IGES_ENTITY* pLineFontPattern;
    IGES_ENTITY* pLevel;
    IGES_ENTITY* pView;
    IGES_ENTITY* pTransform;
    IGES_ENTITY* pLabelAssoc;
    IGES_ENTITY* pColor;

    // list of referring (superior) entities
    std::list<IGES_ENTITY*> refs;
    // list of extra entities (optional PD entries)
    std::list<IGES_ENTITY*> extras;
    std::list<int> iExtras;
    // list of optional comments
    std::list<std::string> comments;
    // data formatted for output (also used for reading PDs from file)
    std::string pdout;

    friend class IGES;
    int sequenceNumber;     // first sequence number of this Directory Entry

    // associate: associate pointers with other entities after reading all data; retrictions on types
    //            must be enforced to ensure data integrity and software stability
    virtual bool associate(std::vector<IGES_ENTITY*>* entities) = 0;

    // format(&index): prepare data for writing; Parameter Data is formatted using the given index
    //                 and DE items are updated; each Entity must have been previously assigned a
    //                 correct Sequence Number
    virtual bool format( int &index ) = 0;
    void         unformat( void );

    // read optional (extra) PD parameters
    bool readExtraParams( int& index );
    // read optional (extra) PD comments
    bool readComments( int& index );
    // format optional (extra) PD parameters for output
    bool formatExtraParams( std::string& fStr, int& pdSeq, char pd, char rd );
    // format optional (extra) PD comments for output
    bool formatComments( int& pdSeq );

    // change the internal scale; this routine may be invoked by teh parent IGES object
    // to change the internal units or the Model Scale.
    virtual bool rescale( double sf ) = 0;

public:
    IGES_ENTITY(IGES* aParent);
    virtual ~IGES_ENTITY();

    // Routines to manage reference deletion

    /// remove a child entity from the parent's list
    virtual bool Unlink( IGES_ENTITY* aChild ) = 0;
    /// return true if the entity is invalidated and can be deleted
    virtual bool IsOrphaned( void ) = 0;

    // Add/DelReference is needed for management of StatusNumber
    virtual bool AddReference( IGES_ENTITY* aParentEntity ) = 0;
    virtual bool DelReference( IGES_ENTITY* aParentEntity ) = 0;
    size_t       GetNRefs( void );

    // Routines for manipulating extra entity list
    int GetNOptionalEntities( void );
    std::list<IGES_ENTITY*>* GetOptionalEntities( void );
    bool AddOptionalEntity( IGES_ENTITY* );
    bool DelOptionalEntity( IGES_ENTITY* );

    // Routines for manipulating the optional comments
    int GetNComments( void );
    std::list<std::string>* GetComments( void );
    bool AddComment( const std::string& aComment );
    bool DelComment( int index );
    bool ClearComments( void );

    // Read the DE data from the given records
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar ) = 0;
    // virtual bool Read: read Parameter Data
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar ) = 0;

    // Write out a Directory Entry; prior to invoking this method a valid
    // Sequence Number must have been assigned and the format() method invoked
    virtual bool WriteDE( std::ofstream& aFile );
    // Write out a Parameter Data block; prior to invoking this method a valid
    // Sequence Number must have been assigned and the format() method invoked
    virtual bool WritePD( std::ofstream& aFile );

    // Set the parent object; this is required for operations such as 'Import'
    bool SetParentIGES( IGES* aParent );
    // Retrieve the parent object
    IGES* GetParentIGES( void );

    int          GetEntityType( void );
    int          GetEntityForm( void );
    // Set the entity form. This can only succeed if a form has not yet been set;
    // some entities have only one form and default to that form on creation.
    virtual bool SetEntityForm( int aForm ) = 0;

    // Set/Get the Structure entity; this is unused in most entity types
    // and the defaults return false
    virtual bool SetStructure( IGES_ENTITY* aStructure );
    virtual bool GetStructure( IGES_ENTITY** aStructure );

    // Get/Set the LineFont Pattern; this may be an IGES_LINEFONT_PATTERN
    // or a LinefontPattern Entity
    virtual bool SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern );
    virtual bool SetLineFontPattern( IGES_ENTITY* aPattern );
    // The Get functions return false if the argument does not match
    // the internal LineFontPattern type; in the case of type 0,
    // the GetLineFontPatternEntity() function sets the argument
    // to NULL and returns true.
    bool GetLineFontPattern( IGES_LINEFONT_PATTERN& aPattern );
    bool GetLineFontPatternEntity( IGES_ENTITY** aPattern );

    // Get/Set the Entity Level; this may be an int or a Definition Level Entity;
    // default level is 0
    virtual bool SetLevel( int aLevel );
    virtual bool SetLevel( IGES_ENTITY* aLevel );
    bool         GetLevel( int& aLevel );
    bool         GetLevelEntity( IGES_ENTITY** aLevel );

    virtual bool SetView( IGES_ENTITY* aView );
    bool         GetView( IGES_ENTITY** aView );

    // XXX - may require other forms to retrieve compound transforms
    // XXX - may also require a Transform(some point) to transform individual points
    virtual bool SetTransform( IGES_ENTITY* aTransform );
    bool         GetTransform( IGES_ENTITY** aTransform );

    virtual bool SetLabelAssoc( IGES_ENTITY* aLabelAssoc );
    bool         GetLabelAssoc( IGES_ENTITY** aLabelAssoc );

    // Get/Set the entity Color; this may be an IGES_COLOR or a
    // Color Definition Entity
    virtual bool SetColor( IGES_COLOR aColor );
    virtual bool SetColor( IGES_ENTITY* aColor );
    bool         GetColor( IGES_COLOR& aColor );
    bool         GetColorEntity( IGES_ENTITY** aColor );

    virtual bool SetLineWeightNum( int aLineWeight );
    bool         GetLineWeightNum( int& aLineWeight );

    bool SetLabel( const std::string aLabel );
    void GetLabel( std::string& aLabel );

    bool SetEntitySubscript( int aSubscript );
    bool GetEntitySubscript( int& aSubscript );

    // StatusNum is a flag for 4 different things so we shall use separate Set/Get functions for:
    // Visibility: 0/1, 0 (default) = visible, 1 = invisible
    // Depedency: 0 (default, independent) to 3. See p.27/55+
    // EntityUse: 0..6 (Geometry, etc. See p.29/57+)
    // Hierarchy: int (0..99999), 0=default
    bool SetVisibility( bool isVisible );
    bool GetVisibility( bool& isVisible );

    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    bool         GetDependency( IGES_STAT_DEPENDS& aDependency );

    virtual bool SetEntityUse( IGES_STAT_USE aUseCase );
    bool         GetEntityUse( IGES_STAT_USE& aUseCase );

    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );
    bool         GetHierarchy( IGES_STAT_HIER& aHierarchy );
};

#endif  // IGES_ENTITY_H
