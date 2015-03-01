/*
 * file: entity102.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 102: Composite Curve, Section 4.4, p.69+ (97+)
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

/*
 * Implementation notes:
 * + Hierarchy is not ignored in this case
 * + entity100 and entity110 require a GetStartPoint() and GetEndPoint()
 */

#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity102.h>

using namespace std;


IGES_ENTITY_102::IGES_ENTITY_102( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 102;
    form = 0;
    return;
}


IGES_ENTITY_102::~IGES_ENTITY_102()
{
    if( !curves.empty() )
    {
        std::list<IGES_ENTITY*>::iterator rbeg = curves.begin();
        std::list<IGES_ENTITY*>::iterator rend = curves.end();

        while( rbeg != rend )
        {
            if( !(*rbeg)->DelReference( this ) )
            {
                ERRMSG << "\n + [BUG] could not delete reference from a child entity\n";
            }

            ++rbeg;
        }

        curves.clear();
    }

    return;
}


bool IGES_ENTITY_102::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    // Associate pointers
    std::list<int>::iterator bcur = iCurves.begin();
    std::list<int>::iterator ecur = iCurves.end();
    int sEnt = (int)entities->size();
    bool ok = true;

    while( bcur != ecur )
    {
        if( *bcur >= 0 && *bcur < sEnt )
        {
            if( !(*entities)[*bcur]->AddReference( this ) )
            {
                ERRMSG << "\n + [INFO] failed to add reference to child\n";
                ok = false;
            }

            extras.push_back( (*entities)[*bcur] );
        }
        else
        {
            ERRMSG << "\n + [CORRUPT FILE] referenced curve entity (";
            cerr << *bcur << ") does not exist\n";
            ok = false;
        }

        ++bcur;
    }

    // XXX - go through the list and check:
    // (a) entities have a Physical Dependency
    // (c) entities have start/end points which coincide as required
    // (d) these rules of the specification are followed:
    //     + May contain:
    //          Point
    //          Connect Point
    //          parameterized curve entities EXCEPT Composite Curve itself
    //
    //     + Must not have 2 consecutive Point or Connect Point entities
    //       unless they are the *only* 2 entities in the composite curve,
    //       in which case the Use Case flag must be set to 04 (logical/positional)
    //
    //     + May not consist of a single Point or Connect Point entity
    //

    std::list<IGES_ENTITY*>::iterator sp = curves.begin();
    std::list<IGES_ENTITY*>::iterator ep = curves.end();
    int acc = 0;
    int iEnt;

    /*
     * Allowable entities
     * 100 ENT_CIRCULAR_ARC
     * 104 ENT_CONIC_ARC
     * 110 ENT_LINE
     * 112 ENT_PARAM_SPLINE_CURVE
     * 116 *ENT_POINT
     * 126 ENT_NURBS_CURVE
     * 132 *ENT_CONNECT_POINT
     * 106 ENT_COPIOUS_DATA FORMS: (Due to complexity, postpone any implementation of this)
     *        1, 2, 3
     *        11, 12, 13
     *        63
     */

    while( sp != ep )
    {
        iEnt = (*sp)->GetEntityType();

        if( iEnt != 100 && iEnt != 104 && iEnt !=110 && iEnt != 112
            && iEnt != 116 && iEnt != 126 && iEnt != 132 && iEnt != 106 )
        {
            ERRMSG << "\n + [INFO] Unsupported entity (";
            cerr << iEnt << ") in Composite Curve\n";
            return false;
        }

        if( acc > 0 )
        {
            if( (iEnt == 116 || iEnt == 132) && acc > 0 )
            {
                // XXX - check rule about consecutive 116/132

            }

            // XXX - check that StartPoint[N] == EndPoint[N-1]
        }

        ++acc;
        ++sp;
    }

    // XXX - if only 1 entity, ensure it is neither 116 nor 132

#warning + [WARNING] TO BE IMPLEMENTED
    return ok;
}


bool IGES_ENTITY_102::format( int &index )
{
    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_102::rescale( double sf )
{
    // there is nothing to scale
    return true;
}


bool IGES_ENTITY_102::Unlink( IGES_ENTITY* aChildEntity )
{
    if( IGES_ENTITY::Unlink( aChildEntity ) )
        return true;

    // XXX - TO BE IMPLEMENTED
    // check the list of curves
#warning + [WARNING] TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_102::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_102::IGES_ENTITY_102::AddReference( IGES_ENTITY* aParentEntity )
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed to method\n";
        return false;
    }

    if( aParentEntity->GetEntityType() == 102 )
    {
        ERRMSG << "\n + [INFO] violation of specification: may not reference Entity 102\n";
        return false;
    }

    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_102::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_102::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Composite Curve\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_102::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{

    // XXX - TO BE IMPLEMENTED
    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_102::SetEntityForm( int aForm )
{
    if( aForm == 0 )
        return true;

    ERRMSG << "\n + [BUG] invalid form (" << aForm << ") in Composite Curve entity\n";
    return false;
}


bool IGES_ENTITY_102::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    hierarchy = aHierarchy;
    return true;
}


int IGES_ENTITY_102::GetNSegments( void )
{
    return (int)curves.size();
}


IGES_ENTITY* IGES_ENTITY_102::GetSegment( int index )
{
    if( index < 0 || index >= (int)curves.size() )
    {
        ERRMSG << "\n + [INFO] invalid index (" << index << ")\n";
        return NULL;
    }

    std::list<IGES_ENTITY*>::iterator sl = curves.begin();

    for( int i = 0; i < index; ++i )
        ++sl;

    return *sl;
}
