/*
 * file: iges.h
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

#ifndef IGES_H
#define IGES_H

#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <libigesconf.h>
#include <iges_base.h>
#include <iges_entity.h>

class IGES_ENTITY_308;

/**
 * Struct IGES_GLOBAL
 * contains the data read from or to be written to the IGES
 * file Global Section.
 */
struct IGES_GLOBAL
{
    char        pdelim;                     //< parameter delimeter; RD ','
    char        rdelim;                     //< record delimeter; RD ';'
    std::string productIDSS;                //< Product Identification used by Sending System (RN)
    std::string fileName;                   //< Name of this file (must match in order to support external refs.) (RN)
    std::string nativeSystemID;             //< Name of the software which created the IGES model (RN)
    std::string preprocessorVersion;        //< Version of the Preprocessor which created the IGES model (RN)
    int         nIntegerBits;               //< number of significant bits in an integer on the Sending System (RN)
    int         floatMaxExp;                //< max. exponent of a Float type (RN)
    int         floatMaxSig;                //< max. significant digits of a Float type (RN)
    int         doubleMaxExp;               //< max. exponent of a Double type (RN)
    int         doubleMaxSig;               //< max. significant digits of a Double type (RN)
    std::string productIDRS;                //< Product Identification used by Receiving System (RD: product IDSS)
    double      modelScale;                 //< ModelUnits/RealWorldUnits : Ex. a 1:8 model must multiply units by 8 to get RWU (RD: 1.0)
    IGES_UNIT   unitsFlag;                  //< Internal unit representation of model on disk (RD: 1 (inch))
    std::string unitsName;                  //< String symbolizing units described by unitsFlag (RD: must match unitsFlag)
    int         maxLinewidthGrad;           //< Max. linewidth gradations (RD: 1; min. = 1)
    double      maxLinewidth;               //< Max. linewidth in terms of internal units (RN)
    std::string creationDate;               //< [YY]YYMMDD.HHNNSS date of file creation (enforce UTC) (RN)
    double      minResolution;              //< Min. user intended resolution (RN)
    double      maxCoordinateValue;         //< Max. used coordinate value (normally set to '0.0' = not determined) (RD: 0.0)
    std::string author;                     //< name of author (RD: "")
    std::string organization;               //< name of author's organization (RD: "")
    int         igesVersion;                //< flag indicating IGES version of the file; 3..11 (RD: 3)
    IGES_DRAFTING_STANDARD draftStandard;   //< flag indicating drafting standard (if any) (RD: 0)
    std::string modificationDate;           //< [YY]YYMMDD.HHNNSS date of file creation/modification (RD: creationDate)
    std::string applicationNote;            //< Application Protocol, Application Subset, MIL-STD-SPEC, User Protocol, etc (RD: "")

    // derived scale to be applied to quantities to derive mm units with model scale 1.0
    double      cf;                         //< conversion factor for normalizing input when model scale != 1.0
    bool        convert;                    //< true if we need to rescale upon reading; all files are normalized to 1.0 model scale in mm
};


/**
 * Class IGES
 * is the high level object for manipulating IGES data
 */
class IGES
{
private:
    static int idxPartNum;                  //< index used to create Part Name
    static int idxAssyNum;                  //< index used to create Assembly Name
    std::list< bool* > m_validFlags;        //< DLL layer validation flags
    std::vector< const char* > vStartSection;   //< temp. vector table for DLL access

    std::list<std::string> startSection;    //< text from the Start section
    int                    nGlobSecLines;   //< number of lines in the Global section
    int                    nDESecLines;     //< number of lines in the Directory Entry section
    int                    nPDSecLines;     //< number of lines in the Parameter Data section

    std::vector<IGES_ENTITY*> entities;     //< all existing IGES entities and their data

    // initialize internal data structures
    bool init(void);

    // read IGES Global Section data
    bool readGlobals( IGES_RECORD& rec, std::ifstream& file );
    // read all Directory Entries (when a Parameter Data Entry is encountered. rewind to the start of that line)
    bool readDE( IGES_RECORD& rec, std::ifstream& file );
    // read data based on existing entities' record on number of associated Parameter Data lines
    bool readPD( IGES_RECORD& rec, std::ifstream& file );
    // read the TERMINATE section and verify data
    bool readTS( IGES_RECORD& rec, std::ifstream& file );
    // cull orphaned entities; setting the 'vicious' flag will result in the culling
    // of most entity types which do not have a parent; only Type 408 entities are
    // allowed to exist without a parent.
    void cull( bool vicious = false );

    // write out the START SECTION
    bool writeStart( std::ofstream& file );
    // write out the GLOBAL SECTION
    bool writeGlobals( std::ofstream& file );

public:
    IGES();
    ~IGES();

    IGES_GLOBAL globalData;    //< Global Section data

    /**
     * Function AttachValidFlag
     * sets a pointer to the boolean used to signal an
     * API layer upon destruction or change of ownership
     *
     * @param aFlag is a pointer to an API layer's internal
     * validation flag. Note that if the flag is not NULL
     * when this function is invoked, the previous pointer's
     * content is set to false before the new pointer is
     * accepted; this can result in another API layer object
     * being invalidated.
     */
    void AttachValidFlag( bool* aFlag );
    void DetachValidFlag( bool* aFlag );


    /**
     * Function Compact
     * iterates through all entities and deletes any temporary data
     * which may have been created for the convenience of passing
     * data across the DLL boundary.
     */
    void Compact( void );


    /**
     * Function Cull
     * culls all orphaned entities and top-level entities which
     * are not Type 408 (Singular Subfigure Instance)
     */
    void Cull( void );


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
    bool Export( IGES* newParent, IGES_ENTITY_308** packagedEntity );


    /**
     * Function GetNewPartName
     * creates a new, and hopefully unique, part name. The part name may not be
     * unique if a sub-assembly happens to have a sub-part with the same name,
     * in which case a more comprehensive naming code must be developed.
     *
     * @param name = variable to store the part name
     */
    void GetNewPartName( std::string& name );

    // create a new (hopefully unique) assembly name; the name may not be unique
    // if a sub-assembly happens to have the same name
    /**
     * Function GetNewAssemblyName
     * creates a new, and hopefully unique, assembly name. The assembly name may
     * not be unique if a sub-assembly happens to have a sub-assembly with the
     * same name, in which case a more comprehensive naming code must be developed.
     *
     * @param name = variable to store the assembly name
     */
    void GetNewAssemblyName( std::string& name );


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
    bool NewEntity( int aEntityType, IGES_ENTITY** aEntityPointer );


    /**
     * Function AddEntity
     * adds an entity from another IGES object or an entity created
     * without the aid of NewEntity() to the list of entities manages by
     * this IGES object and returns true on success.
     *
     * @param aEntity = pointer to an entity to be owned by this IGES object
     */
    bool AddEntity( IGES_ENTITY* aEntity );


    /**
     * Function DelEntity
     * removes the entity specified by the pointer, frees associated
     * entity data, and returns true on success.
     *
     * @param aEntity = pointer to the entity to be destroyed
     */
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
    bool GetHeaders( size_t aListSize, char const**& aHeaderList );


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
    bool AddToHeader( const char*& comments );

    const char* GetUnitName( void );
};


#endif  // IGES_H
