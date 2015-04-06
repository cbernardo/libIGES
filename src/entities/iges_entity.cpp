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
#include <sstream>
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
    comments.clear();

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

    if( !extras.empty() )
    {
        std::list<IGES_ENTITY*>::iterator rbeg = extras.begin();
        std::list<IGES_ENTITY*>::iterator rend = extras.end();

        while( rbeg != rend )
        {
            if( (*rbeg)->GetEntityType() == 402 )
            {
                // If the entity type is 402 then the reference is a 'back pointer'
                // and the item behaves as a parent, not a child, so Unlink() is used.
                if( !(*rbeg)->Unlink( this ) )
                {
                    ERRMSG << "\n + [BUG] could not unlink a parent entity\n";
                }
            }
            else
            {
                // All other entities are children so DelReference() must be called.
                if( !(*rbeg)->DelReference( this ) )
                {
                    ERRMSG << "\n + [BUG] could not delete reference from a child entity\n";
                }
            }

            ++rbeg;
        }

        extras.clear();
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
    // extras<>

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

    if( !extras.empty() )
    {
        std::list<IGES_ENTITY*>::iterator rbeg = extras.begin();
        std::list<IGES_ENTITY*>::iterator rend = extras.end();

        while( rbeg != rend )
        {
            if( *rbeg == aChild )
            {
                extras.erase( rbeg );
                return true;
            }

            ++rbeg;
        }
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

    // check if the entity is a child in extras<>
    bref = extras.begin();
    eref = extras.end();

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
    if( NULL == aParentEntity )
    {
        ERRMSG << "\n + [BUG] parent entity is a NULL pointer\n";
        return false;
    }

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

    bref = extras.begin();
    eref = extras.end();

    while( bref != eref )
    {
        if( aParentEntity == *bref )
        {
            int eType = (*bref)->GetEntityType();

            if( eType != 402 )
                (*bref)->DelReference( this );

            extras.erase( bref );
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
    // (optional) extras<>
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
            pTransform = dynamic_cast<IGES_ENTITY_124*>((*entities)[idx]);

            if( !pTransform )
            {
                ERRMSG << "\n + [BUG] could not cast IGES_ENTITY* (Type ";
                cerr << ((*entities)[idx]->GetEntityType()) << ") to IGES_ENTITY_124*\n";
                return false;
            }

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

    // associate the extras<>
    std::list<int>::iterator bext = iExtras.begin();
    std::list<int>::iterator eext = iExtras.end();
    int sEnt = (int)entities->size();
    int iEnt;

    // Note: the Associativity Instance is a back-pointer so we do not
    // request that the entity add a reference since one must already
    // exist. All other allowed entities are children and a reference
    // must be added.
    while( bext != eext )
    {
        iEnt = *bext >> 1;

        if( iEnt >= 0 && iEnt < sEnt )
        {
            tEnt = (*entities)[iEnt]->GetEntityType();

            switch( tEnt )
            {
                case ENT_GENERAL_NOTE:
                case ENT_TEXT_DISPLAY_TEMPLATE:
                    if( !(*entities)[iEnt]->AddReference( this ) )
                    {
                        ERRMSG << "\n + [INFO] failed to add reference to child\n";
                        ok = false;
                    }
                    else
                    {
                        extras.push_back( (*entities)[iEnt] );
                    }

                    break;

                case ENT_ASSOCIATIVITY_INSTANCE:
                    extras.push_back( (*entities)[iEnt] );
                    break;

                default:
                    ERRMSG << "\n + [CORRUPT FILE] invalid entity (" << tEnt;
                    cerr << ") in Optional Parameters section for Entity DE ";
                    cerr << sequenceNumber << "\n";
                    ok = false;
                    break;
            }
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] optional entity referenced does not exist\n";
            ok = false;
        }

        ++bext;
    }

    return ok;
}   // associate()


void IGES_ENTITY::unformat( void )
{
    pdout.clear();
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

    if( tmpInt < 0 )
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

    // note: As per spec. (Sec. 2.2.4.4.3) this field may contain a positive number
    // in which case it is simply ignored. In versions of the spec. earlier than 3.0
    // positive numbers were occasionally used to designate version numbers.
    if( tmpInt > 0 )
        tmpInt = 0;

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

    // Note: a NULL entity (but no other entity) may specify
    // 0 for the Parameter Count; however as per spec the
    // Parameter Sequence must be > 0.
    if( tmpInt < 0 || (tmpInt == 0 && entityType != 0) )
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

bool IGES_ENTITY::ReadPD(std::ifstream& aFile, int& aSequenceVar)
{
    pdout.clear();

    // As per IGES specification, a NULL Entity may
    // specify 0 lines of parameter data
    if( parameterData == 0 && entityType == 0 )
        return true;

    if( parameterData < 1 || parameterData > 9999999 )
    {
        ERRMSG << "\n + [BUG] invalid Parameter Data Index (" << parameterData << ")\n";
        return false;
    }

    if( paramLineCount < 1 || (parameterData + paramLineCount) > 10000000 )
    {
        ERRMSG << "\n + [BUG] invalid Parameter Line Count (" << paramLineCount << ")\n";
        cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
        return false;
    }

    if( !parent )
    {
        ERRMSG << "\n + [BUG] ReadPD invoked with no parent IGES object\n";
        return false;
    }

    IGES_RECORD rec;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

#ifdef DEBUG
    cout << "[INFO] Entity(" << entityType;
    cout << ") Parameter Data Record for entity at DE " << sequenceNumber << "\n";
#endif

    bool first = true;
    int tmpInt;

    for(int i = 0; i < paramLineCount; ++i)
    {
        if( !ReadIGESRecord( &rec, aFile ) )
        {
            ERRMSG << "\n + could not read Parameter Data\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            cerr << " + [INFO] Parameter Line # (" << (parameterData + i) << ")\n";
            pdout.clear();
            return false;
        }

#ifdef DEBUG
        cout << "    " << rec.data << "\n";
#endif

        if( rec.section_type != 'P' )
        {
            ERRMSG << "\n + [BUG or BAD FILE] not a Parameter Data section (type: '";
            cerr << rec.section_type << "'\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            cerr << " + [INFO] Parameter Line # (" << (parameterData + i) << ")\n";
            pdout.clear();
            return false;
        }

        if( rec.index != (parameterData + i) )
        {
            ERRMSG << "\n + [BAD FILE] incorrect Parameter Line # (" << rec.index << ")\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            cerr << " + [INFO] Expected Parameter Line # (" << (parameterData + i) << ")\n";
            pdout.clear();
            return false;
        }

        // check the DE sequence number
        if( rec.data[64] != ' ' )
        {
            ERRMSG << "\n + [BAD FILE] invalid Parameter Data line; col[64] is not blank\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            pdout.clear();
            return false;
        }

        if( !DEItemToInt( rec.data, 8, tmpInt, NULL ) )
        {
            ERRMSG << "\n + [BAD FILE] invalid Parameter Data line; could not read DE number\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            pdout.clear();
            return false;
        }
        else if( tmpInt != sequenceNumber )
        {
            ERRMSG << "\n + [BAD FILE] Parameter Data DE Sequence (" << tmpInt;
            cerr << ") does not match parent (" << sequenceNumber << ")\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            pdout.clear();
            return false;
        }

        if( first )
        {
            first = false;
            int idx = 0;
            bool eor = false;

            // check EntityID
            if( !ParseInt( rec.data, idx, tmpInt, eor, pd, rd ) )
            {
                ERRMSG << "\n + [BAD FILE] No Entity Number in Parameter Data\n";
                cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
                cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
                pdout.clear();
                return false;
            }
            else if( tmpInt != entityType )
            {
                ERRMSG << "\n + [BAD FILE] Parameter Data Entity ID (" << tmpInt;
                cerr << ") does not match parent (" << entityType << ")\n";
                cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
                cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
                pdout.clear();
                return false;
            }
            else if( eor )
            {
                ERRMSG << "\n + [BAD FILE] Parameter Data, premature end of record\n";
                cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
                cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
                pdout.clear();
                return false;
            }
        }

        pdout += rec.data.substr( 0, 64 );
        ++aSequenceVar;
    }

#ifdef DEBUG
    cout << "-----\n";
#endif

    return true;
}   // ReadPD()


bool IGES_ENTITY::WriteDE( std::ofstream& aFile )
{
    std::string oln1;   // DE Line 1
    std::string oln2;   // DE Line 2
    std::string tstr;   // temporary string for formatting

    // DE1:  ENTITY TYPE NUMBER
    // DE11: ENTITY TYPE NUMBER
    if( !FormatDEInt( tstr, entityType ) )
    {
        ERRMSG << "\n + [BUG] cannot write Entity Type Number to Directory Entry\n";
        return false;
    }

    oln1 = tstr;
    oln2 = tstr;

    // DE2:  PARAMETER DATA (max. 7 digits)
    if( parameterData + paramLineCount > 10000000 )
    {
        ERRMSG << "\n + [ERROR] cannot write Parameter Data Line to Directory Entry\n";
        cerr << " + [VIOLATION] capacity of IGES specification has been exceeded\n";
        return false;
    }

    if( !FormatDEInt( tstr, parameterData ) )
    {
        ERRMSG << "\n + [BUG] cannot write Entity Type Number to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE3:  STRUCTURE (0 or -P)
    bool fStat;

    if( !pStructure )
        fStat = FormatDEInt( tstr, 0 );
    else
        fStat = FormatDEInt( tstr, -pStructure->sequenceNumber );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Structure Pointer to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE4:  LINE FONT PATTERN (Num, 0, or -P)
    if( pLineFontPattern )
        fStat = FormatDEInt( tstr, -pLineFontPattern->sequenceNumber );
    else if( lineFontPattern >= LINEFONT_NONE && lineFontPattern < LINEFONT_END )
        fStat = FormatDEInt( tstr, lineFontPattern );
    else
        fStat = FormatDEInt( tstr, 0 );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Line Font Pattern to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE5:  LEVEL (Num, 0, or -P)
    if( pLevel )
        fStat = FormatDEInt( tstr, -pLevel->sequenceNumber );
    else if( level >= 0 )
        fStat = FormatDEInt( tstr, level );
    else
        fStat = FormatDEInt( tstr, 0 );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Level to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE6:  VIEW (0 or +P)
    if( pView )
        fStat = FormatDEInt( tstr, pView->sequenceNumber );
    else
        fStat = FormatDEInt( tstr, 0 );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Line Font Pattern to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE7:  TRANSFORM (0 or +P)
    if( pTransform )
        fStat = FormatDEInt( tstr, pTransform->sequenceNumber );
    else
        fStat = FormatDEInt( tstr, 0 );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Transform to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE8:  LABEL DISPLAY ASSOCIATIVITY (0 or +P)
    if( pLabelAssoc )
        fStat = FormatDEInt( tstr, pLabelAssoc->sequenceNumber );
    else
        fStat = FormatDEInt( tstr, 0 );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Label Display Associativity to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE9:  STATUS NUMBER
    int tmpInt;
    // Blank Status
    if( visible )
        tmpInt = 0;
    else
        tmpInt = 100;
    // Dependency (Subordinate Entity Switch)
    tmpInt += depends;
    tmpInt *= 100;
    // Entity Use Flag
    tmpInt += use;
    tmpInt *= 100;
    // Hierarchy
    tmpInt += hierarchy;

    if( !FormatDEInt( tstr, tmpInt ) )
    {
        ERRMSG << "\n + [BUG] cannot write Status Number to Directory Entry\n";
        return false;
    }

    oln1 += tstr;

    // DE10: SECTION CODE AND SEQUENCE
    if( !FormatDEInt( tstr, sequenceNumber ) || tstr[0] != ' ' )
    {
        ERRMSG << "\n + [BUG] cannot write Section Code and Sequence Number to Directory Entry\n";
        return false;
    }

    tstr[0] = 'D';
    oln1 += tstr;

    if( oln1.length() != 80 )
    {
        ERRMSG << "\n + [BUG] Directory Entry was incorrectly formatted (length = ";
        cerr << oln1.length() << ")\n";
        return false;
    }

    oln1 += "\n";

    // DE12: LINE WEIGHT NUMBER (0 .. globalData.maxLinewidthGrad)
    if( !FormatDEInt( tstr, lineWeightNum ) )
    {
        ERRMSG << "\n + [BUG] cannot write Line Weight Number to Directory Entry\n";
        return false;
    }

    oln2 += tstr;

    // DE13: COLOR NUMBER (Num, 0, or -P)
    if( pColor )
        fStat = FormatDEInt( tstr, -pColor->sequenceNumber );
    else if( colorNum >= COLOR_NONE && colorNum < COLOR_END )
        fStat = FormatDEInt( tstr, colorNum );
    else
        fStat = FormatDEInt( tstr, 0 );

    if( !fStat )
    {
        ERRMSG << "\n + [BUG] cannot write Color Number to Directory Entry\n";
        return false;
    }

    oln2 += tstr;

    // DE14: PARAMETER LINE COUNT NUMBER (+INT)
    if( !FormatDEInt( tstr, paramLineCount ) )
    {
        ERRMSG << "\n + [BUG] cannot write Parameter Line Count Number to Directory Entry\n";
        return false;
    }

    oln2 += tstr;

    // DE15: FORM NUMBER (+INT)
    if( !FormatDEInt( tstr, form ) )
    {
        ERRMSG << "\n + [BUG] cannot write Form Number to Directory Entry\n";
        return false;
    }

    oln2 += tstr;

    // DE16: NOT USED
    // DE17: NOT USED
    oln2.append( 16, ' ' );

    // DE18: ENTITY LABEL
    tmpInt = (int)label.length();
    tstr.clear();

    if( tmpInt < 8 )
        tstr.append( 8 - tmpInt, ' ' );

    tstr += label.substr(0, 8);
    oln2 += tstr;

    // DE19: ENTITY SUBSCRIPT NUMBER (INT)
    if( !FormatDEInt( tstr, entitySubscript ) )
    {
        ERRMSG << "\n + [BUG] cannot write Entity Subscript Number to Directory Entry\n";
        return false;
    }

    oln2 += tstr;

    // DE20: SECTION CODE AND SEQUENCE+1
    if( !FormatDEInt( tstr, sequenceNumber + 1 ) || tstr[0] != ' ' )
    {
        ERRMSG << "\n + [BUG] cannot write Section Code and Sequence Number + 1 to Directory Entry\n";
        return false;
    }

    tstr[0] = 'D';
    oln2 += tstr;

    if( oln2.length() != 80 )
    {
        ERRMSG << "\n + [BUG] Directory Entry (line 2) was incorrectly formatted (length = ";
        cerr << oln2.length() << ")\n";
        return false;
    }

    oln2 += "\n";

    aFile << oln1;
    aFile << oln2;

    if( aFile.fail() )
    {
        ERRMSG << "\n + [INFO] could not write data to file\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY::WritePD( std::ofstream& aFile )
{
    if( pdout.empty() || 0 != (pdout.length() % 81) )
    {
        ERRMSG << "\n + [INFO] improperly formatted PD output (length=";
        cerr << pdout.length() << ")\n";
        return false;
    }

    aFile << pdout;
    pdout.clear();

    if( aFile.fail() )
    {
        ERRMSG << "\n + [INFO] could not write data to file\n";
        return false;
    }

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


int IGES_ENTITY::GetDESequence( void )
{
    return sequenceNumber;
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
    // most entities do not support 'structure' so default is false
    ERRMSG << "\n + [BUG] attempting to set 'structure' parameter on Entity Type ";
    cerr << entityType << "\n";
    return false;
}


bool IGES_ENTITY::GetStructure(IGES_ENTITY** aStructure)
{
    // most entities do not support 'structure' so default is false
    *aStructure = NULL;

    ERRMSG << "\n + [BUG] attempting to read 'structure' parameter on Entity Type ";
    cerr << entityType << "\n";

    return false;
}


bool IGES_ENTITY::GetLineFontPattern(IGES_LINEFONT_PATTERN& aPattern)
{
    if( pLineFontPattern )
    {
        aPattern = LINEFONT_NONE;
        return false;
    }

    if( lineFontPattern < LINEFONT_NONE || lineFontPattern >= LINEFONT_END )
    {
        ERRMSG << "\n + [BUG] method invoked without valid LineFontPattern association\n";
        return false;
    }

    aPattern = (IGES_LINEFONT_PATTERN)lineFontPattern;
    return true;
}


bool IGES_ENTITY::GetLineFontPatternEntity(IGES_ENTITY** aPattern)
{
    *aPattern = NULL;

    if( !pLineFontPattern )
        return false;

    *aPattern = pLineFontPattern;
    return true;
}


bool IGES_ENTITY::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    if( pLineFontPattern )
    {
        pLineFontPattern->DelReference( this );
        pLineFontPattern = NULL;
    }

    if( aPattern < LINEFONT_NONE || aPattern >= LINEFONT_END )
    {
        ERRMSG << "\n + [BUG] method invoked with invalid linefont pattern (";
        cerr << aPattern << ") in entity type #" << entityType << "\n";
        return false;
    }

    lineFontPattern = aPattern;
    return true;
}


bool IGES_ENTITY::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    lineFontPattern = 0;

    if( pLineFontPattern )
    {
        pLineFontPattern->DelReference( this );
        pLineFontPattern = NULL;
    }

    if( !aPattern )
        return true;

    int tEnt = aPattern->GetEntityType();

    if( tEnt != ENT_LINE_FONT_DEFINITION )
    {
        ERRMSG << "\n + [BUG] invalid entity (#" << tEnt;
        cerr << ") assigned to LineFontPattern (expecting ";
        cerr << ENT_LINE_FONT_DEFINITION << ") ";
        cerr << "in entity type #" << entityType << "\n";
        return false;
    }

    if( !aPattern->AddReference( this ) )
    {
        ERRMSG << "\n + [BUG] could not add reference to child entity\n";
        return false;
    }

    pLineFontPattern = aPattern;

    return true;
}


bool IGES_ENTITY::GetLevel(int &aLevel)
{
    if( pLevel )
    {
        aLevel = 0;
        return false;
    }

    if( level < 0 )
    {
        ERRMSG << "\n + [BUG] method invoked without valid level assignment in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    aLevel = level;
    return true;
}


bool IGES_ENTITY::GetLevelEntity(IGES_ENTITY** aLevel)
{
    *aLevel = NULL;

    if( pLevel )
    {
        *aLevel = pLevel;
        return true;
    }

    return false;
}


bool IGES_ENTITY::SetLevel( int aLevel )
{
    if( pLevel )
    {
        pLevel->DelReference( this );
        pLevel = NULL;
    }

    if( aLevel < 0 )
    {
        ERRMSG << "\n + [BUG] method invoked with invalid level (< 0) in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    level = aLevel;
    return true;
}


bool IGES_ENTITY::SetLevel( IGES_ENTITY* aLevel )
{
    level = 0;

    if( pLevel )
    {
        pLevel->DelReference( this );
        pLevel = NULL;
    }

    if( !aLevel )
        return true;

    int tEnt = aLevel->GetEntityType();
    int tFrm = aLevel->GetEntityForm();

    if( tEnt != ENT_PROPERTY || tFrm != 1 )
    {
        ERRMSG << "\n + [BUG] invalid entity (" << tEnt << "-" << tFrm;
        cerr << ") assigned to level (expecting ";
        cerr << ENT_PROPERTY << "-1) in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    if( !aLevel->AddReference( this ) )
    {
        ERRMSG << "\n + [BUG] could not add reference to child entity\n";
        return false;
    }

    pLevel = aLevel;
    return true;
}


bool IGES_ENTITY::GetView(IGES_ENTITY** aView)
{
    *aView = NULL;

    if( pView )
    {
        *aView = pView;
        return true;
    }

    return false;
}


bool IGES_ENTITY::SetView( IGES_ENTITY* aView )
{
    view = 0;

    if( pView )
    {
        pView->DelReference( this );
        pView = NULL;
    }

    if( !aView )
        return true;

    int tEnt = aView->GetEntityType();
    int tFrm = aView->GetEntityForm();

    if( tEnt != ENT_VIEW || tEnt != ENT_ASSOCIATIVITY_INSTANCE
        || (tEnt == ENT_ASSOCIATIVITY_INSTANCE && tFrm != 3 && tFrm != 4 && tFrm != 19) )
    {
        ERRMSG << "\n + [BUG] invalid entity (" << tEnt << "-" << tFrm;
        cerr << ") assigned to level (expecting 410 or 402-3/4/19) in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    if( !aView->AddReference( this ) )
    {
        ERRMSG << "\n + [BUG] could not add reference to child entity\n";
        return false;
    }

    pView = aView;
    return true;
}


bool IGES_ENTITY::GetTransform(IGES_ENTITY** aTransform)
{
    *aTransform = NULL;

    if( pTransform )
    {
        *aTransform = pTransform;
        return true;
    }

    return false;
}


bool IGES_ENTITY::SetTransform( IGES_ENTITY* aTransform )
{
    transform = 0;

    if( pTransform )
    {
        pTransform->DelReference( this );
        pTransform = NULL;
    }

    if( !aTransform )
        return true;

    int tEnt = aTransform->GetEntityType();

    if( tEnt != ENT_TRANSFORMATION_MATRIX )
    {
        ERRMSG << "\n + [BUG] invalid entity (" << tEnt;
        cerr << ") assigned to transform (expecting ";
        cerr << ENT_TRANSFORMATION_MATRIX << ") in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    pTransform = dynamic_cast<IGES_ENTITY_124*>(aTransform);

    if( !pTransform )
    {
        ERRMSG << "\n + [BUG] could not cast IGES_ENTITY* to IGES_ENTITY_124*\n";
        return false;
    }

    if( !pTransform->AddReference( this ) )
    {
        pTransform = NULL;
        ERRMSG << "\n + [BUG] could not add reference to child entity\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY::GetLabelAssoc(IGES_ENTITY** aLabelAssoc)
{
    *aLabelAssoc = NULL;

    if( 0 == (labelAssoc & 0x80000000) )
        return true;

    if( !pLabelAssoc )
    {
        ERRMSG << "\n + [BAD DATA] invalid Label Association pointer in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    *aLabelAssoc = pLabelAssoc;
    return true;
}


bool IGES_ENTITY::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    labelAssoc = 0;

    if( pLabelAssoc )
    {
        pLabelAssoc->DelReference( this );
        pLabelAssoc = NULL;
    }

    if( !aLabelAssoc )
        return true;

    int tEnt = aLabelAssoc->GetEntityType();
    int tFrm = aLabelAssoc->GetEntityForm();

    if( tEnt != ENT_ASSOCIATIVITY_INSTANCE || tFrm != 5 )
    {
        ERRMSG << "\n + [BUG] invalid entity (" << tEnt << "-" << tFrm;
        cerr << ") assigned to Label Display Associativity (expecting ";
        cerr << ENT_ASSOCIATIVITY_INSTANCE << "-5) in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    if( !aLabelAssoc->AddReference( this ) )
    {
        ERRMSG << "\n + [BUG] could not add reference to child entity\n";
        return false;
    }

    pLabelAssoc = aLabelAssoc;
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
        ERRMSG << "\n + [BAD DATA] invalid color entity in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    *aColor = pColor;
    return true;
}


bool IGES_ENTITY::SetColor( IGES_COLOR aColor )
{
    if( pColor )
    {
        pColor->DelReference( this );
        pColor = NULL;
    }

    if( aColor < COLOR_NONE || aColor >= COLOR_END )
    {
        ERRMSG << "\n + [BUG] method invoked with invalid color (";
        cerr << aColor << ") in entity type #" << entityType << "\n";
        return false;
    }

    colorNum = aColor;
    return true;
}


bool IGES_ENTITY::SetColor( IGES_ENTITY* aColor )
{
    colorNum = 0;

    if( pColor )
    {
        pColor->DelReference( this );
        pColor = NULL;
    }

    if( !aColor )
        return true;

    int tEnt = aColor->GetEntityType();

    if( tEnt != ENT_COLOR_DEFINITION )
    {
        ERRMSG << "\n + [BUG] invalid entity (#" << tEnt;
        cerr << ") assigned to Color Definition (expecting ";
        cerr << ENT_COLOR_DEFINITION << ") in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    if( !aColor->AddReference( this ) )
    {
        ERRMSG << "\n + [BUG] could not add reference to child entity\n";
        return false;
    }

    pColor = aColor;
    return true;
}


bool IGES_ENTITY::GetLineWeightNum(int& aLineWeight)
{
    aLineWeight = lineWeightNum;
    return true;
}


bool IGES_ENTITY::SetLineWeightNum( int aLineWeight )
{
    if( aLineWeight < 0 )
    {
    }

    if( !parent )
    {
        ERRMSG << "\n + [BUG] method invoked without parent entity in entity type #";
        cerr << entityType << "\n";
        return false;
    }

    int tmpLW = parent->globalData.maxLinewidthGrad;

    if( aLineWeight > tmpLW )
    {
        ERRMSG << "\n + [WARNING] lineweight (" << aLineWeight;
        cerr << ") exceeds max gradation (" << tmpLW << ") in entity type #";
        cerr << entityType << "\n";
        lineWeightNum = tmpLW;
        return true;
    }

    lineWeightNum = aLineWeight;
    return true;
}


bool IGES_ENTITY::SetLabel(const std::string aLabel)
{
    label = aLabel.substr(0, 8);

    if( aLabel.length() > 8 )
    {
        ERRMSG << "\n + [WARNING] label exceeds 8 characters; truncating to '";
        cerr << label << "' in entity type #" << entityType << "\n";
    }

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

// read optional (extra) PD parameters
bool IGES_ENTITY::readExtraParams( int& index )
{
    int nAType;
    int nBType;
    int idef = 0;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;
    int ent;
    iExtras.clear();

    if( !ParseInt( pdout, index, nAType, eor, pd, rd, &idef ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of extra entities\n";
        return false;
    }

    if( nAType < 0 )
    {
        ERRMSG << "\n + [INFO] invalid number of entities: " << nAType << "\n";
        return false;
    }

    int i;

    for( i = 0; i < nAType; ++i )
    {
        if( !ParseInt( pdout, index, ent, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the entity DE index\n";
            return false;
        }

        if( ent < 1 || (ent & 1) == 0 || ent > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE index (" << ent << ")\n";
            return false;
        }

        iExtras.push_back( ent );
    }

    if( eor )
        return true;

    if( !ParseInt( pdout, index, nBType, eor, pd, rd, &idef ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of extra entities (Type 312)\n";
        return false;
    }

    if( nBType < 0 )
    {
        ERRMSG << "\n + [INFO] invalid number of entities: " << nBType << "\n";
        return false;
    }

    for( i = 0; i < nBType; ++i )
    {
        if( !ParseInt( pdout, index, ent, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the entity DE index\n";
            return false;
        }

        if( ent < 1 || (ent & 1) == 0 || ent > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE index (" << ent << ")\n";
            return false;
        }

        iExtras.push_back( ent );
    }

    if( !eor )
    {
        ERRMSG << "\n + [CORRUPT FILE] did not find End-of-Record marker in optional section\n";
        return false;
    }

    return true;
}


// read optional (extra) PD comments
bool IGES_ENTITY::readComments( int& index )
{
    if( index % 64 )
        index += 64 - (index % 64);

    int sz = (int)pdout.length();
    std::string comment;

    while( index + 64 <= sz )
    {
        comment = pdout.substr( index, 64 );
        comments.push_back( comment );
        index += 64;
    }

    if( index != (int)pdout.length() )
    {
        ERRMSG << "\n + [WARNING] comment block does not seem to be a multiple of 64 bytes\n";
    }

    return true;
}

// format optional (extra) PD parameters for output
bool IGES_ENTITY::formatExtraParams( std::string& fStr,int& pdSeq, char pd, char rd )
{
    if( extras.empty() )
    {
        ERRMSG << "\n + [BUG] invoked when there were no extra parameters\n";
        return false;
    }

    std::list<int> secA;    // section for Types 402 and 212
    std::list<int> secB;    // section for Types 4312

    std::list<IGES_ENTITY*>::iterator sExt = extras.begin();
    std::list<IGES_ENTITY*>::iterator eExt = extras.end();
    int eType;

    while( sExt != eExt )
    {
        eType = (*sExt)->GetEntityType();

        switch( eType )
        {
            case 402:
            case 212:
                secA.push_back( eType );
                break;

            case 312:
                secB.push_back( eType );
                break;

            default:
                ERRMSG << "\n + [BUG] invalid type in Optional Entities (extras) list: ";
                cerr << eType << "\n";
                return false;
                break;
        }

        ++sExt;
    }

    std::string tstr;   // individual item to add to optional entities

    ostringstream ostr;
    std::list<int>::iterator sSec;
    std::list<int>::iterator eSec;

    // if there is no data in Section A, write a parameter delimeter to
    if( secA.empty() )
    {
        tstr = pd;

        AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );
    }
    else
    {
        sSec = secA.begin();
        eSec = secA.end();

        if( secB.empty() )
            --eSec;

        ostr.str("");
        ostr << secA.size() << pd;
        tstr = ostr.str();
        AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );

        while( sSec != eSec )
        {
            ostr.str("");
            ostr << *sSec << pd;
            tstr = ostr.str();
            AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );
            ++sSec;
        }

        if( secB.empty() )
        {
            ostr.str("");
            ostr << *sSec << rd;
            tstr = ostr.str();
            AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );
            return true;
        }
    }

    sSec = secB.begin();
    eSec = secB.end();
    --eSec;

    ostr.str("");
    ostr << secB.size() << pd;
    tstr = ostr.str();
    AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );

    while( sSec != eSec )
    {
        ostr.str("");
        ostr << *sSec << pd;
        tstr = ostr.str();
        AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );
        ++sSec;
    }

    ostr.str("");
    ostr << *sSec << rd;
    tstr = ostr.str();
    AddPDItem( tstr, fStr, pdout, pdSeq, sequenceNumber, pd, rd );

    return true;
}


// format optional (extra) PD comments for output
bool IGES_ENTITY::formatComments( int& pdSeq )
{
    if( 0 != pdout.length() % 81 )
    {
        ERRMSG << "\n + [BUG] PD output is not a multiple of 81\n";
        return false;
    }

    if( comments.empty() )
        return true;

    std::list<std::string>::iterator sCom = comments.begin();
    std::list<std::string>::iterator eCom = comments.end();
    std::string tmp;
    std::string tmp1;

    size_t sz;
    size_t sidx;

    while( sCom != eCom )
    {
        sz = sCom->length();

        for( sidx = 0; sidx < sz; )
        {
            tmp = sCom->substr( sidx, 72 );
            sidx += tmp.length();

            if( tmp.length() < 72 )
                tmp.append( 72 - tmp.length(), ' ' );

                if( !FormatDEInt( tmp1, pdSeq++ ) )
                {
                    ERRMSG << "\n + [BUG] could not format optional parameter comment\n";
                    return false;
                }

                tmp1[0] = 'P';
                tmp += tmp1 + "\n";
                pdout += tmp;
        }
        ++sCom;
    }

    return true;
}



int IGES_ENTITY::GetNOptionalEntities( void )
{
    return extras.size();
}


std::list<IGES_ENTITY*>* IGES_ENTITY::GetOptionalEntities( void )
{
    return &extras;
}


bool IGES_ENTITY::AddOptionalEntity( IGES_ENTITY* aEntity )
{
    if( !aEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for entity\n";
        return false;
    }

    int eType = aEntity->GetEntityType();

    if( eType != 402 && eType != 212 && eType != 312 )
    {
        ERRMSG << "\n + [BUG] invalid entity (Type " << eType;
        cerr << "); only types 402, 212, and 312 are valid.\n";
        return false;
    }

    if( eType != 402 )
    {
        if( !aEntity->AddReference( this ) )
        {
            ERRMSG << "\n + [info] could not add reference to specified entity.\n";
            return false;
        }
    }

    extras.push_back( aEntity );
    return true;
}


bool IGES_ENTITY::DelOptionalEntity( IGES_ENTITY* aEntity )
{
    if( !aEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for entity\n";
        return false;
    }

    int eType = aEntity->GetEntityType();

    if( eType != 402 && eType != 212 && eType != 312 )
    {
        ERRMSG << "\n + [BUG] invalid entity (Type " << eType;
        cerr << "); only types 402, 212, and 312 are valid.\n";
        return false;
    }

    if( !DelReference( aEntity ) )
    {
        ERRMSG << "\n + [INFO] could not delete reference\n";
        return false;
    }

    return true;
}


int IGES_ENTITY::GetNComments( void )
{
    return comments.size();
}


std::list<std::string>* IGES_ENTITY::GetComments( void )
{
    return &comments;
}


bool IGES_ENTITY::AddComment( const std::string& aComment )
{
    if( aComment.empty() )
    {
        ERRMSG << "\n + [INFO] empty comment string\n";
    }

    comments.push_back( aComment );
    return true;
}


bool IGES_ENTITY::DelComment( int index )
{
    if( index < 0 || index >= (int)comments.size() )
    {
        ERRMSG << "\n + [INFO] invalid index (" << index << ") ";
        cerr << "valid indices are 0 .. " << (comments.size() -1) << "\n";
        return false;
    }

    list<string>::iterator bs = comments.begin();

    int i = 0;
    while( i < index )
    {
        ++i;
        ++bs;
    }

    comments.erase( bs );
    return true;
}


bool IGES_ENTITY::ClearComments( void )
{
    comments.clear();
    return true;
}


bool IGES_ENTITY::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    depends = aDependency;
    return true;
}


bool IGES_ENTITY::SetEntityUse( IGES_STAT_USE aUseCase )
{
    use = aUseCase;
    return true;
}


bool IGES_ENTITY::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    if( aHierarchy == STAT_HIER_USE_PROP
        || hierarchy == STAT_HIER_USE_PROP )
    {
        ERRMSG << "\n + [WARNING] SetHierarchy does not correctly support Option 2\n";
    }

    hierarchy = aHierarchy;
    return true;
}
