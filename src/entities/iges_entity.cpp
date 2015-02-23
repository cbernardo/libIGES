/*
 * file: iges_entity.cpp
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

// Note: This base class must never be instantiated.

#include <iomanip>
#include <error_macros.h>
#include <iges.h>
#include <all_entities.h>
#include <iges_io.h>


using namespace std;


IGES_ENTITY::IGES_ENTITY(IGES* aParent)
{
    // master IGES object; contains globals and manages entity I/O
    parent = aParent;

    // first sequence number of this Directory Entry (0: default = invalid)
    sequenceNumber = 0;

    // Entity Type, default = NULL Entity
    entityType = ENT_NULL;

    // first sequence number of associated parameterData (0: default = invalid)
    parameterData = 0;

    // index to DirEnt of the definition entity which specifies this entity's meaning
    structure = 0;

    // line font pattern number, or index to Line Font Definition (304), default = 0
    lineFontPattern = 0;

    // level number for this entity, or index to Definition Levels Property (406-1), default = 0
    level = 0;

    // 0 (default) or index to DirEnt for one of (a) View Entity (410)
    // or (b) Views Visible Associativity Instance (402-3/4/19)
    view = 0;

    // 0 (default) or index to Transormation Matrix (124)
    transform = 0;

    // 0 (default) or index to label Display Associativity (402-5)
    labelAssoc = 0;

    // Status Number: Blank Status (default 0: visible == true)
    visible = true;

    // Status Number: Subordinate Entity Switch (default 0 = independent)
    depends = STAT_INDEPENDENT;

    // Status Number: Entity Use (default 0 = Geometry)
    use = STAT_USE_GEOMETRY;

    // Status Number: Hierarchy (default 0 = all DE attributes apply to subordinates)
    hierarchy = STAT_HIER_ALL_SUB;

    // system line width thickness, 0 .. Global::maxLinewidthGrad
    // (default: 0 = use Receiving System's default)
    lineWeightNum = 1;

    // 0 (default = NO COLOR), Color ID (IGES_COLOR), or index to Color Definition (314)
    colorNum = 0;

    // number of associated Parameter Lines, 0 = invalid
    paramLineCount = 0;

    // 0 or Form Number for entities with more than one form
    form = -1;

    // 1..8 digit unsigned int associated with the label
    entitySubscript = 0;

    // pointers which may be linked to other entities:
    pStructure = NULL;
    pLineFontPattern = NULL;
    pLevel = NULL;
    pView = NULL;
    pTransform = NULL;
    pLabelAssoc = NULL;
    pColor = NULL;

    return;
}   // IGES_ENTITY::IGES_ENTITY(IGES*)


IGES_ENTITY::~IGES_ENTITY()
{
    if( !refs.empty() )
    {
        std::list<IGES_ENTITY*>::iterator rbeg = refs.begin();
        std::list<IGES_ENTITY*>::iterator rend = refs.end();

        while( rbeg != rend )
        {
            if( !(*rbeg)->Unlink( this ) )
            {
                ERRMSG << "\n + [BUG] could not unlink a parent entity\n";
            }

            ++rbeg;
        }

        refs.clear();
    }

    if( pStructure )
    {
        pStructure->DelReference( this );
        pStructure = NULL;
    }

    if( pLineFontPattern )
    {
        pLineFontPattern->DelReference( this );
        pLineFontPattern = NULL;
    }

    if( pLevel )
    {
        pLevel->DelReference( this );
        pLevel = NULL;
    }

    if( pView )
    {
        pView->DelReference( this );
        pView = NULL;
    }

    if( pTransform )
    {
        pTransform->DelReference( this );
        pTransform = NULL;
    }

    if( pLabelAssoc )
    {
        pLabelAssoc->DelReference( this );
        pLabelAssoc = NULL;
    }

    if( pColor )
    {
        pColor->DelReference( this );
        pColor = NULL;
    }

    return;
}   // IGES_ENTITY::~IGES_ENTITY()


bool IGES_ENTITY::Unlink( IGES_ENTITY* aChild )
{
    // unlink and return true if the child matches
    // one of:
    // pStructure;
    // pLineFontPattern;
    // pLevel;
    // pView;
    // pTransform;
    // pLabelAssoc;
    // pColor;

    if( !aChild )
    {
        ERRMSG << "\n + [BUG] Unlink() invoked with NULL argument\n";
        return false;
    }

    if( aChild == pStructure )
    {
        pStructure = NULL;
        return true;
    }

    if( aChild == pLineFontPattern )
    {
        pLineFontPattern = NULL;
        return true;
    }

    if( aChild == pLevel )
    {
        pLevel = NULL;
        return true;
    }

    if( aChild == pView )
    {
        pView = NULL;
        return true;
    }

    if( aChild == pTransform )
    {
        pTransform = NULL;
        return true;
    }

    if( aChild == pLabelAssoc )
    {
        pLabelAssoc = NULL;
        return true;
    }

    if( aChild == pColor )
    {
        pColor = NULL;
        return true;
    }

    return false;
}


bool IGES_ENTITY::AddReference( IGES_ENTITY* aParentEntity )
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for aParentEntity\n";
        return false;
    }

    // Ensure we have no direct circular references
    if( aParentEntity == pStructure || aParentEntity == pLineFontPattern
        || aParentEntity == pLevel || aParentEntity == pView
        || aParentEntity == pTransform || aParentEntity == pLabelAssoc
        || aParentEntity == pColor )
    {
        ERRMSG << "\n + [BUG] requested a circular dependency on a DE item\n";
        return false;
    }

    std::list<IGES_ENTITY*>::iterator bref = refs.begin();
    std::list<IGES_ENTITY*>::iterator eref = refs.end();

    while( bref != eref )
    {
        if( aParentEntity == *bref )
            return true;

        ++bref;
    }

    refs.push_back( aParentEntity );
    return true;
}


bool IGES_ENTITY::DelReference( IGES_ENTITY* aParentEntity )
{
    std::list<IGES_ENTITY*>::iterator bref = refs.begin();
    std::list<IGES_ENTITY*>::iterator eref = refs.end();

    while( bref != eref )
    {
        if( aParentEntity == *bref )
        {
            refs.erase( bref );
            return true;
        }

        ++bref;
    }

    return false;
}


bool IGES_ENTITY::associate(std::vector<IGES_ENTITY*>* entities)
{
    // All entities must read in the following:
    //
    // structure
    // *lineFontPattern
    // *level
    // view
    // transform
    // labelAssoc
    // *colorNum
    //
    // Note: it is the responsibility of the individual
    // entities to ensure that there is no data entry for
    // parameters which do not apply. For example, most
    // entities must have 0 for the 'structure' parameter.

    bool ok = true;
    int idx;
    int tEnt;

    if( structure > 0 )
    {
        idx = structure >> 1;

        if( idx < (int)entities->size() )
        {
            pStructure = (*entities)[idx];

            if( pStructure == this )
            {
                pStructure = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by structure (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pStructure->AddReference( this ) )
            {
                if( pStructure->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pStructure)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pStructure = NULL;
            }

        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by structure (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    if( lineFontPattern < 0 )
    {
        idx = (-lineFontPattern) >> 1;

        if( idx < (int)entities->size() )
        {
            pLineFontPattern = (*entities)[idx];

            if( pLineFontPattern == this )
            {
                pLineFontPattern = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by Line Font Pattern (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pLineFontPattern->AddReference( this ) )
            {
                if( pLineFontPattern->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pLineFontPattern)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pLineFontPattern = NULL;
            }
            else
            {
                int eType = pLineFontPattern->GetEntityType();

                if( eType != 304 && eType != 0 )
                {
                    ERRMSG << "\n + [CORRUPT FILE] Entity[DE" << (idx >> 1) + 1 << "] (Type ";
                    cerr << pLineFontPattern->GetEntityType();
                    cerr << ") is not a LineFont Pattern (304)\n";
                    pLineFontPattern = NULL;
                    ok = false;
                }
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by Line Font Pattern (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    if( level < 0 )
    {
        idx = (-level) >> 1;

        if( idx < (int)entities->size() )
        {
            pLevel = (*entities)[idx];

            if( pLevel == this )
            {
                pLevel = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by Level (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pLevel->AddReference( this ) )
            {
                if( pLevel->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pLevel)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pLevel = NULL;
            }
            else
            {
                int eType = pLevel->GetEntityType();
                int eForm = pLevel->GetEntityForm();

                if( eType != 0 && (eType != 406 || eForm != 1) )
                {
                    ERRMSG << "\n + [CORRUPT FILE] Entity[DE" << (idx >> 1) + 1 << "] (Type " << eType;
                    cerr << "-" << eForm << ") is not a Definition Levels Property (406-1)\n";
                    pLevel = NULL;
                    ok = false;
                }
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by Level (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    if( view > 0 )
    {
        idx = view >> 1;

        if( idx < (int)entities->size() )
        {
            pView = (*entities)[idx];

            if( pView == this )
            {
                pView = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by View (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pView->AddReference( this ) )
            {
                if( pView->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pView)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pView = NULL;
            }
            else
            {
                int eType = pView->GetEntityType();
                int eForm = pView->GetEntityForm();

                if( eType != 0 && ((eType != 410 && eType != 402)
                    || (eType == 402 && eForm != 3 && eForm != 4 && eForm != 19 )) )
                {
                    ERRMSG << "\n + [CORRUPT FILE] Entity[DE" << (idx >> 1) + 1 << "] (Type " << eType;
                    cerr << "-" << eForm << ") is not a View or Views Visible Associativity Instance (410 or 402-3/4/19)\n";
                    pView = NULL;
                    ok = false;
                }
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by View (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    if( transform > 0 )
    {
        idx = transform >> 1;

        if( idx < (int)entities->size() )
        {
            pTransform = (*entities)[idx];

            if( pTransform == this )
            {
                pTransform = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by Transform (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pTransform->AddReference( this ) )
            {
                if( pTransform->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pTransform)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pTransform = NULL;
            }
            else
            {
                int eType = pTransform->GetEntityType();

                if( eType != 124 && eType != 0 )
                {
                    ERRMSG << "\n + [CORRUPT FILE] Entity[DE" << (idx >> 1) + 1 << "] (Type " << eType;
                    cerr << ") is not a Transform entity (124)\n";
                    pTransform = NULL;
                    ok = false;
                }
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by Transform (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    if( labelAssoc > 0 )
    {
        idx = labelAssoc >> 1;

        if( idx < (int)entities->size() )
        {
            pLabelAssoc = (*entities)[idx];

            if( pLabelAssoc == this )
            {
                pLabelAssoc = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by Transform (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pLabelAssoc->AddReference( this ) )
            {
                if( pLabelAssoc->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pLabelAssoc)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pLabelAssoc = NULL;
            }
            else
            {
                int eType = pLabelAssoc->GetEntityType();
                int eForm = pLabelAssoc->GetEntityForm();

                if( (eType != 0 && eType != 402) || (eType == 402 && eForm != 5) )
                {
                    ERRMSG << "\n + [CORRUPT FILE] Entity[DE" << (idx >> 1) + 1 << "] (Type " << eType;
                    cerr << "-" << eForm << ") is not a Label Display Associativity (402-5)\n";
                    pLabelAssoc = NULL;
                    ok = false;
                }
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by Label Display Association (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    if( colorNum < 0 )
    {
        idx = (-colorNum) >> 1;

        if( idx < (int)entities->size() )
        {
            pColor = (*entities)[idx];

            if( pColor == this )
            {
                pColor = NULL;
                ERRMSG << "\n + [CORRUPT FILE] self-referential entity referenced by Color Number (DE";
                cerr << (idx >> 1) + 1 << ")\n";
                ok = false;
            }

            if( !pColor->AddReference( this ) )
            {
                if( pColor->GetEntityType() != 0 )
                {
                    ERRMSG << "\n + [INFO] could not add a reference to a non-NULL entity\n";
                    ok = false;
                }
                else
                {
                    tEnt = ((IGES_ENTITY_NULL*)pColor)->GetTrueEntityType();

                    ERRMSG << "\n + [WARNING] inconsistent data file; entity [DE";
                    cerr << sequenceNumber << "] contains a reference to ";

                    if( !tEnt )
                    {
                        cerr << "a NULL";
                    }
                    else
                    {
                        cerr << "an unsupported (Type " << tEnt << ")";
                    }

                    cerr << " entity [DE" << (idx << 1) + 1 << "]\n";
                }

                pColor = NULL;
            }
            else
            {
                int eType = pColor->GetEntityType();

                if( eType != 0 && eType != 314 )
                {
                    ERRMSG << "\n + [CORRUPT FILE] Entity[DE" << (idx >> 1) + 1 << "] (Type " << eType;
                    cerr << ") is not a Color Definition (314)\n";
                    pColor = NULL;
                    ok = false;
                }
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] entity referenced by Color Number (DE";
            cerr << (idx >> 1) + 1 << ") does not exist\n";
            ok = false;
        }
    }

    return ok;
}


bool IGES_ENTITY::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    // Read in the basic DE data only; it is the responsibility of
    // the individual entities to impose any further checks on
    // data integrity

    if( !aRecord )
    {
        ERRMSG << "\n + [BUG]: NULL pointer passed for parameter 'aRecord'\n";
        return false;
    }

    // DE10: Ensure type is 'D' and sequence number is odd
    if( aRecord->section_type != 'D' )
    {
        ERRMSG << "\n + [BUG]: Expecting Directory Entry marker 'D' (0x44), got 0x";
        cerr << hex << setw(2) << setfill('0') << ((unsigned int)aRecord->section_type) << "\n";
        cerr << dec << setfill(' ');
        return false;
    }

    if( aRecord->index <= 0 || aRecord->index >= 9999997 )
    {
        ERRMSG << "\n + [BUG]: invalid sequence number: " << aRecord->index << "\n";
        return false;
    }

    if( 0 == (aRecord->index & 1) )
    {
        ERRMSG << "\n + [BUG]: expecting an odd sequence number in DE Record 1, got " << aRecord->index << "\n";
        return false;
    }

    sequenceNumber = aRecord->index;

    // DE1: Entity Type Number
    int tmpInt;

    if( !DEItemToInt(aRecord->data, 0, tmpInt, NULL))
    {
        ERRMSG << "\n + could not extract Entity Type number\n";
        return false;
    }

    if( tmpInt != entityType )
    {
        ERRMSG << "\n + [BUG] retrieved entity type (" << tmpInt;
        cerr << ") does not match internal type (" << entityType << ")\n";
        return false;
    }

    // DE2: Parameter Data Sequence Number
    if( !DEItemToInt(aRecord->data, 1, tmpInt, NULL))
    {
        ERRMSG << "\n + could not extract Parameter Data sequence number\n";
        return false;
    }

    if( tmpInt <= 0 )
    {
        ERRMSG << "\n + invalid Parameter Data sequence number: " << tmpInt << "\n";
        return false;
    }

    parameterData = tmpInt;

    // DE3: Structure (normally 0 - not applicable)
    int defInt = 0;

    if( !DEItemToInt(aRecord->data, 2, tmpInt, &defInt))
    {
        ERRMSG << "\n + could not extract Structure pointer\n";
        return false;
    }

    if( tmpInt > 0 )
    {
        ERRMSG << "\n + invalid Structure pointer (" << tmpInt << "); must be <= 0\n";
        return false;
    }

    structure = tmpInt;

    // DE4: Line Font Pattern (IGES_LINEFONT_PATTERN or negative)
    if( !DEItemToInt(aRecord->data, 3, tmpInt, &defInt))
    {
        ERRMSG << "\n + could not extract Line Font Pattern\n";
        return false;
    }

    if( tmpInt >= (int)LINEFONT_END )
    {
        ERRMSG << "\n + invalid Line Font Pattern (" << tmpInt << "); must be < " << LINEFONT_END << "\n";
        return false;
    }

    lineFontPattern = tmpInt;

    // DE5: Level
    if( !DEItemToInt(aRecord->data, 4, tmpInt, &defInt))
    {
        ERRMSG << "\n + could not extract Level value\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Level value (" << tmpInt << "); must be >= 0\n";
        return false;
    }

    level = tmpInt;

    // DE6: View
    if( !DEItemToInt(aRecord->data, 5, tmpInt, &defInt))
    {
        ERRMSG << "\n + could not extract View value\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid View value (" << tmpInt << "); must be >= 0\n";
        return false;
    }

    view = tmpInt;

    // DE7: Transformation Matrix
    if( !DEItemToInt(aRecord->data, 6, tmpInt, &defInt))
    {
        ERRMSG << "\n + could not extract Transformation Matrix\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Transformation Matrix pointer (" << tmpInt << "); must be >= 0\n";
        return false;
    }

    transform = tmpInt;

    // DE8: Label Display Associativity
    if( !DEItemToInt(aRecord->data, 7, tmpInt, &defInt))
    {
        ERRMSG << "\n + could not extract Label Display Associativity\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Label Display Associativity pointer (" << tmpInt << "); must be >= 0\n";
        return false;
    }

    labelAssoc = tmpInt;

    // DE9: Status Number
    if( !DEItemToInt(aRecord->data, 8, tmpInt, NULL))
    {
        ERRMSG << "\n + could not extract Status Number\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Status Number (" << tmpInt << "); must be >= 0\n";
        return false;
    }

    // DE9: Status Number: Hierarchy Flag
    int tmpInt2 = tmpInt % 100;

    if( tmpInt2 > 2 )
    {
        ERRMSG << "\n + invalid Status Number::Hierarchy Flag (" << tmpInt2 << "); must be 0..2\n";
        return false;
    }

    hierarchy = (IGES_STAT_HIER)tmpInt2;

    // DE9: Status Number: Entity Use Flag
    tmpInt2 = (tmpInt / 100) % 100;

    if( tmpInt2 > 6 )
    {
        ERRMSG << "\n + invalid Status Number::Entity Use Flag (" << tmpInt2 << "); must be 0..6\n";
        return false;
    }

    use = (IGES_STAT_USE)tmpInt2;

    // DE9: Status Number: Dependency Flag (Subordinate Entity Switch)
    tmpInt2 = (tmpInt / 10000) % 100;

    if( tmpInt2 > 3 )
    {
        ERRMSG << "\n + invalid Status Number::Dependency Flag (" << tmpInt2 << "); must be 0..3\n";
        return false;
    }

    depends = (IGES_STAT_DEPENDS)tmpInt2;

    // DE9: Status Number: Blank Status Flag
    tmpInt2 = (tmpInt / 1000000) % 100;

    if( tmpInt2 > 1 )
    {
        ERRMSG << "\n + invalid Status Number::Blank Status Flag (" << tmpInt2 << "); must be 0,1\n";
        return false;
    }

    if( tmpInt2 == 0 )
        visible = true;
    else
        visible = false;

    // DE10: Type flag and Sequence Number (already checked)

    // Load and check the next part of the DE record
    IGES_RECORD rec;

    if( ! ReadIGESRecord(&rec, aFile) )
    {
        ERRMSG << "\n + could not read second DE line\n";
        return false;
    }

    if( rec.section_type != 'D' )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected section type 'D', got '";
        cerr << rec.section_type << "'\n";
        cerr << " + DE #" << (aRecord->index + 1) << "\n";
        return false;
    }

    if( rec.index != (aRecord->index + 1) )
    {
        ERRMSG << "\n + [CORRUPT FILE] expected DE index '" << (aRecord->index + 1);
        cerr << "', got '" << rec.index << "'\n";
        return false;
    }

    // DE11: Must match current type
    if( !DEItemToInt(rec.data, 0, tmpInt, NULL) )
    {
        ERRMSG << "\n + [CORRUPT FILE] could not extract Entity Type number from DE #";
        cerr << rec.index << "'\n";
        return false;
    }

    if( tmpInt != entityType )
    {
        ERRMSG << "\n + [CORRUPT FILE] retrieved entity type (" << tmpInt;
        cerr << ") does not match internal type (" << entityType << ")\n";
        cerr << " + DE #" << rec.index << "\n";
        return false;
    }

    // DE12: Line Weight Number (note: in the spec there is no default for this)
    if( !DEItemToInt(rec.data, 1, tmpInt, NULL) )
    {
        ERRMSG << "\n + could not extract Line Weight Number\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Line Weight Number: " << tmpInt << "\n";
        return false;
    }

    if( parent && tmpInt > parent->globalData.maxLinewidthGrad )
    {
        ERRMSG << "\n + [WARNING] DEFECTIVE FILE, DE #" << rec.index << "\n";
        cerr << " + Line Weight Number (" << tmpInt;
        cerr << ") exceeds global maximum (" << parent->globalData.maxLinewidthGrad << ")\n";
        tmpInt = parent->globalData.maxLinewidthGrad;
    }

    lineWeightNum = tmpInt;

    // DE13: Color Number
    if( !DEItemToInt(rec.data, 2, tmpInt, &defInt) )
    {
        ERRMSG << "\n + could not extract Color Number\n";
        return false;
    }

    if( tmpInt >= COLOR_END )
    {
        ERRMSG << "\n + invalid Color Number (" << tmpInt << ")\n";
        return false;
    }

    colorNum = tmpInt;

    // DE14: Parameter Line Count
    if( !DEItemToInt(rec.data, 3, tmpInt, NULL) )
    {
        ERRMSG << "\n + could not extract Parameter Line Count\n";
        return false;
    }

    if( tmpInt < 1 )
    {
        ERRMSG << "\n + invalid Parameter Line Count: " << tmpInt << "\n";
        return false;
    }

    paramLineCount = tmpInt;

    // DE15: Form Number
    if( !DEItemToInt(rec.data, 4, tmpInt, &defInt) )
    {
        ERRMSG << "\n + could not extract Form Number\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Form Number (" << tmpInt << ")\n";
        return false;
    }

    form = tmpInt;

    // DE16: Not Used
    // DE17: Not Used

    // DE18: Entity Label
    string tmpStr;

    if( !DEItemToStr(rec.data, 7, tmpStr) )
    {
        ERRMSG << "\n + could not extract Entity Label\n";
        return false;
    }

    label = tmpStr;

    // DE19: Entity Subscript Number
    if( !DEItemToInt(rec.data, 8, tmpInt, &defInt) )
    {
        ERRMSG << "\n + could not extract Entity Subscript Number\n";
        return false;
    }

    if( tmpInt < 0 )
    {
        ERRMSG << "\n + invalid Entity Subscript Number (" << tmpInt << ")\n";
        return false;
    }

    entitySubscript = tmpInt;
    aSequenceVar += 2;

    return true;
}


bool IGES_ENTITY::SetParentIGES(IGES* aParent)
{
    if( !aParent)
        return false;

    parent = aParent;
    return true;
}


IGES* IGES_ENTITY::GetParentIGES(void)
{
    return parent;
}


size_t IGES_ENTITY::GetNRefs(void)
{
    return refs.size();
}


int IGES_ENTITY::GetEntityType(void)
{
    return entityType;
}


int IGES_ENTITY::GetEntityForm(void)
{
    return form;
}


bool IGES_ENTITY::SetStructure(IGES_ENTITY* aStructure)
{
    return false;
}


bool IGES_ENTITY::GetStructure(IGES_ENTITY** aStructure)
{
    *aStructure = NULL;
    return false;
}


bool IGES_ENTITY::GetLineFontPattern(IGES_LINEFONT_PATTERN& aPattern)
{
    if( lineFontPattern & 0x80000000 )
    {
        aPattern = LINEFONT_NONE;
        return false;
    }

    aPattern = (IGES_LINEFONT_PATTERN)lineFontPattern;
    return true;
}


bool IGES_ENTITY::GetLineFontPatternEntity(IGES_ENTITY** aPattern)
{
    *aPattern = NULL;

    if( 0 == (lineFontPattern & 0x80000000) )
    {
        if( 0 == lineFontPattern )
            return true;
        else
            return false;

    }

    if( pLineFontPattern == NULL )
    {
        ERRMSG << "\n + [BAD DATA] invalid line pattern\n";
        return false;
    }

    *aPattern = pLineFontPattern;
    return true;
}


bool IGES_ENTITY::GetLevel(int aLevel)
{
    if( level & 0x80000000 )
    {
        aLevel = 0;
        return false;
    }

    aLevel = level;
    return true;
}


bool IGES_ENTITY::GetLevelEntity(IGES_ENTITY** aLevel)
{
    *aLevel = NULL;

    if( 0 == (level & 0x80000000) )
    {
        if( level == 0 )
            return true;
        else
            return false;

    }

    if( !pLevel )
    {
        ERRMSG << "\n + [BAD DATA] invalid level pointer\n";
        return false;
    }

    *aLevel = pLevel;
    return true;
}


bool IGES_ENTITY::GetView(IGES_ENTITY** aView)
{
    *aView = NULL;

    if( 0 == (view & 0x80000000) )
        return true;

    if( !pView )
    {
        ERRMSG << "\n + [BAD DATA] invalid view pointer\n";
        return false;
    }

    *aView = pView;
    return true;
}


bool IGES_ENTITY::GetTransform(IGES_ENTITY** aTransform)
{
    *aTransform = NULL;

    if( 0 == (transform & 0x80000000) )
        return true;

    if( !pTransform )
    {
        ERRMSG << "\n + [BAD DATA] invalid transform pointer\n";
        return false;
    }

    *aTransform = pTransform;
    return true;
}


bool IGES_ENTITY::GetLabelAssoc(IGES_ENTITY** aLabelAssoc)
{
    *aLabelAssoc = NULL;

    if( 0 == (labelAssoc & 0x80000000) )
        return true;

    if( !pLabelAssoc )
    {
        ERRMSG << "\n + [BAD DATA] invalid Label Association pointer\n";
        return false;
    }

    *aLabelAssoc = pLabelAssoc;
    return true;
}


bool IGES_ENTITY::GetColor(IGES_COLOR& aColor)
{
    if( colorNum & 0x80000000 )
    {
        aColor = COLOR_NONE;
        return false;
    }

    aColor = (IGES_COLOR)colorNum;
    return true;
}


bool IGES_ENTITY::GetColorEntity(IGES_ENTITY** aColor)
{
    *aColor = NULL;

    if( 0 == (colorNum & 0x80000000) )
    {
        if( 0 == colorNum )
            return true;
        else
            return false;

    }

    if( pColor == NULL )
    {
        ERRMSG << "\n + [BAD DATA] invalid color entity\n";
        return false;
    }

    *aColor = pColor;
    return true;
}


bool IGES_ENTITY::GetLineWeightNum(int& aLineWeight)
{
    aLineWeight = lineWeightNum;
    return true;
}


bool IGES_ENTITY::SetLabel(const std::string aLabel)
{
    label = aLabel.substr(0, 8);
    return true;
}


void IGES_ENTITY::GetLabel(std::string& aLabel)
{
    aLabel = label;
    return;
}


bool IGES_ENTITY::SetEntitySubscript(int aSubscript)
{
    if( aSubscript >= 0 && aSubscript <= 99999999 )
    {
        entitySubscript = aSubscript;
        return true;
    }

    return false;
}


bool IGES_ENTITY::GetEntitySubscript(int& aSubscript)
{
    aSubscript = entitySubscript;
    return true;
}


bool IGES_ENTITY::SetVisibility(bool isVisible)
{
    visible = isVisible;
    return true;
}


bool IGES_ENTITY::GetVisibility(bool& isVisible)
{
    isVisible = visible;
    return true;
}


bool IGES_ENTITY::GetDependency(IGES_STAT_DEPENDS& aDependency)
{
    aDependency = depends;
    return true;
}


bool IGES_ENTITY::GetEntityUse(IGES_STAT_USE& aUseCase)
{
    aUseCase = use;
    return true;
}


bool IGES_ENTITY::GetHierarchy(IGES_STAT_HIER& aHierarchy)
{
    aHierarchy = hierarchy;
    return true;
}
