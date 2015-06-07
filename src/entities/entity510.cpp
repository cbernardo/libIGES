/*
 * file: entity514.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 514: Shell, Section 4.151, p.595+ (623+)
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


#include <error_macros.h>
#include <sstream>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity502.h>
#include <entity504.h>
#include <entity508.h>
#include <entity510.h>

using namespace std;


IGES_ENTITY_510::IGES_ENTITY_510( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 510;
    form = 1;

    visible = true;
    depends = STAT_DEP_PHY;         // required by specification
    msurface = NULL;
    mOuterLoopFlag = false;
    mDEsurf = 0;

    return;
}


IGES_ENTITY_510::~IGES_ENTITY_510()
{
    if( msurface )
        msurface->DelReference( this );

    list<IGES_ENTITY_508*>::iterator sL = mloops.begin();
    list<IGES_ENTITY_508*>::iterator eL = mloops.end();

    while( sL != eL )
    {
        (*sL)->DelReference( this );
        ++sL;
    }

    return;
}


bool IGES_ENTITY_510::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        iloops.clear();
        return false;
    }

    if( 0 >= mDEsurf )
    {
        ERRMSG << "\n + [INFO] invalid surface entity DE\n";
        iloops.clear();
        return false;
    }

    int iEnt = mDEsurf >> 1;

    if( iEnt >= (int)entities->size() )
    {
        ERRMSG << "\n + [INFO] invalid DE (" << mDEsurf;
        cerr << "), list size is " << entities->size() << "\n";
        iloops.clear();
        return false;
    }

    if( !checkSurfType( (*entities)[iEnt] ) )
    {
        ERRMSG << "\n + [INFO] invalid surface entity\n";
        iloops.clear();
        return false;
    }

    msurface = (*entities)[iEnt];
    bool dup = false;

    if( !msurface->AddReference( this, dup ) )
    {
        ERRMSG << "\n + [INFO] could not add reference to surface entity\n";
        msurface = NULL;
        iloops.clear();
        return false;
    }

    list<int>::iterator sL = iloops.begin();
    list<int>::iterator eL = iloops.end();

    while( sL != eL )
    {
        iEnt = (*sL) >> 1;

        if( iEnt >= (int)entities->size() )
        {
            ERRMSG << "\n + [INFO] invalid loop DE (" << (*sL);
            cerr << "), list size is " << entities->size() << "\n";
            iloops.clear();
            return false;
        }

        if( ENT_LOOP != (*entities)[iEnt]->GetEntityType() )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid loop entity at DE (" << (*sL) << ")\n";
            iloops.clear();
            return false;
        }

        if( !(*entities)[iEnt]->AddReference( this, dup ) )
        {
            ERRMSG << "\n + [INFO] could not add reference to loop\n";
            iloops.clear();
            return false;
        }

        mloops.push_back( (IGES_ENTITY_508*)((*entities)[iEnt]) );
        ++sL;
    }

    iloops.clear();
    return true;
}


bool IGES_ENTITY_510::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    if( index < 1 || index > 9999997 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    if( !msurface || mloops.empty() )
    {
        ERRMSG << "\n + [INFO] no valid surface or else empty loop list\n";
        return false;
    }

    parameterData = index;

    if( !parent )
    {
        ERRMSG << "\n + [INFO] method invoked with no parent IGES object\n";
        return false;
    }

    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << msurface->GetDESequence() << pd;
    ostr << mloops.size() << pd;

    if( mOuterLoopFlag )
        ostr << "1" << pd;
    else
        ostr << "0" << pd;

    string fStr = ostr.str();
    string tStr;

    list<IGES_ENTITY_508*>::iterator sL = mloops.begin();
    list<IGES_ENTITY_508*>::iterator eL = mloops.end();
    list<IGES_ENTITY_508*>::iterator iL = --(mloops.end());

    while( sL != iL )
    {
        ostr.str("");
        ostr << (*sL)->GetDESequence() << pd;
        tStr = ostr.str();
        AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );
        ++sL;
    }

    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    ostr.str("");
    ostr << (*sL)->GetDESequence() << idelim;
    tStr = ostr.str();
    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !extras.empty() && !formatExtraParams( fStr, index, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] could not format optional parameters\n";
        pdout.clear();
        iExtras.clear();
        return false;
    }

    if( !formatComments( index ) )
    {
        ERRMSG << "\n + [INFO] could not format comments\n";
        pdout.clear();
        return false;
    }

    paramLineCount = index - parameterData;

    return true;
}


bool IGES_ENTITY_510::rescale( double sf )
{
    // there is nothing to scale so we always succeed
    return true;
}


bool IGES_ENTITY_510::Unlink( IGES_ENTITY* aChildEntity )
{
    if( IGES_ENTITY::Unlink( aChildEntity ) )
        return true;

    if( aChildEntity == msurface )
    {
        msurface = NULL;
        return true;
    }

    list<IGES_ENTITY_508*>::iterator sL = mloops.begin();
    list<IGES_ENTITY_508*>::iterator eL = mloops.end();

    while( sL != eL )
    {
        if( aChildEntity == (*sL) )
        {
            sL = mloops.erase( sL );
            return true;
        }

        ++sL;
    }

    return false;
}


bool IGES_ENTITY_510::IsOrphaned( void )
{
    if( refs.empty() || NULL == msurface || mloops.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_510::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed to method\n";
        return false;
    }

    // NOTE: TO BE IMPLEMENTED:
    // Checks for circular references have been omitted

    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_510::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_510::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    depends = STAT_DEP_PHY;         // required
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    lineWeightNum = 0;              // N.A.
    colorNum = 0;                   // N.A.

    if( form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Face\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_510::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Edge Entity\n";
        pdout.clear();
        return false;
    }

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

	idx = (int)pdout.find(pd);

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    // DE to the surface entity
    if( !ParseInt( pdout, idx, mDEsurf, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the surface entity DE\n";
        pdout.clear();
        return false;
    }

    if( mDEsurf < 1 || mDEsurf > 9999997 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid DE to surface entity (" << mDEsurf << ")\n";
        pdout.clear();
        return false;
    }

    int nL; // number of loops in the face

    if( !ParseInt( pdout, idx, nL, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of loops\n";
        pdout.clear();
        return false;
    }

    if( nL < 1 )
    {
        ERRMSG << "\n + [INFO] invalid number of loops: " << nL << "\n";
        pdout.clear();
        return false;
    }

    int tmpI;

    if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the Outer Loop Flag\n";
        pdout.clear();
        return false;
    }

    if( 0 > tmpI || 1 < tmpI )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Outer Loop Flag (" << tmpI << ")\n";
        pdout.clear();
        return false;
    }

    if( 0 == tmpI )
        mOuterLoopFlag = false;
    else
        mOuterLoopFlag = true;

    // read in DEs for loops
    for( int i = 0; i < nL; ++i )
    {
        if( !ParseInt( pdout, idx, tmpI, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read loop DE\n";
            iloops.clear();
            pdout.clear();
            return false;
        }

        if( tmpI < 1 || tmpI > 9999997 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid DE to loop (" << tmpI << ")\n";
            iloops.clear();
            pdout.clear();
            return false;
        }

        iloops.push_back( tmpI );
    }

    if( !eor && !readExtraParams( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read optional pointers\n";
        pdout.clear();
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
        pdout.clear();
        return false;
    }

    pdout.clear();

    // note: this entity never performs scaling

    return true;
}


bool IGES_ENTITY_510::SetEntityForm( int aForm )
{
    if( aForm == 1 )
        return true;

    ERRMSG << "\n + [BUG] Face Entity only supports Form 1 (requested form: ";
    cerr << aForm << ")\n";
    return false;
}


bool IGES_ENTITY_510::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [BUG] Face Entity does not support Transform entities\n";
    return false;
}


bool IGES_ENTITY_510::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( STAT_DEP_PHY != aDependency )
    {
        ERRMSG << "\n + [BUG] Face Entity only supports STAT_DEP_PHY\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_510::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


bool IGES_ENTITY_510::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [BUG]: parameter not supported by this entity\n";
    return false;
}


const std::list<IGES_ENTITY_508*>* IGES_ENTITY_510::GetBounds( void )
{
    return &mloops;
}


bool IGES_ENTITY_510::AddBound( IGES_ENTITY_508* aLoop )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


bool IGES_ENTITY_510::SetSurface( IGES_ENTITY* aSurface )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}


IGES_ENTITY* IGES_ENTITY_510::GetSurface( void )
{
    return msurface;
}


void IGES_ENTITY_510::SetOuterLoopFlag( bool aFlag )
{
    mOuterLoopFlag = aFlag;
    return;
}


bool IGES_ENTITY_510::GetOuterLoopFlag( void )
{
    return mOuterLoopFlag;
}


bool IGES_ENTITY_510::checkSurfType( IGES_ENTITY* aEnt )
{
    // permitted surface entities:
    // 114   (parametric Spline Surface)
    // 118-1 (Ruled Surface)
    // 120   (Surface of Revolution)
    // 122   (Tabulated Cylinder)
    // 128   (NURBS)
    // 140   (Offset Surface)
    // 190   (Plane Surface [untested])
    // 192   (Right Circular Cylindrical Surface [untested])
    // 194   (Right Circular Conical Surface [untested])
    // 196   (Spherical Surface [untested])
    // 198   (Toroidal Surface [untested])

    int eType = aEnt->GetEntityType();

    switch( eType )
    {
        case 114:
        case 120:
        case 122:
        case 128:
        case 140:
        case 190:
        case 192:
        case 194:
        case 196:
        case 198:
            break;

        case 118:
            do
            {
                int ef = aEnt->GetEntityForm();

                if( 1 != ef )
                {
                    ERRMSG << "\n +[INFO] invalid form for surface entity type 118 (" << ef << ")\n";
                    return false;
                }

            } while( 0 );

            break;

        default:
            ERRMSG << "\n +[INFO] invalid surface entity type (" << eType << ")\n";
            return false;
    }

    return true;
}
