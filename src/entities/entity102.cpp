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

#include <sstream>

#include <libigesconf.h>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <mcad_helpers.h>
#include <all_entities.h>

using namespace std;


IGES_ENTITY_102::IGES_ENTITY_102( IGES* aParent ) : IGES_CURVE( aParent )
{
    entityType = 102;
    form = 0;
    return;
}


IGES_ENTITY_102::~IGES_ENTITY_102()
{
    if( !curves.empty() )
    {
        std::list<IGES_CURVE*>::iterator rbeg = curves.begin();
        std::list<IGES_CURVE*>::iterator rend = curves.end();

        while( rbeg != rend )
        {
            if( !((IGES_ENTITY*)(*rbeg))->DelReference( this ) )
            {
                ERRMSG << "\n + [BUG] could not delete reference from a child entity (type ";
                cerr << ((IGES_ENTITY*)(*rbeg))->GetEntityType() << ")\n";
            }

            ++rbeg;
        }

        curves.clear();
    }

    return;
}


bool IGES_ENTITY_102::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->DelReference( this );
        pStructure = NULL;
    }

    // Associate pointers
    std::list<int>::iterator bcur = iCurves.begin();
    std::list<int>::iterator ecur = iCurves.end();
    int sEnt = (int)entities->size();
    int iEnt = 0;
    int tEnt;
    bool ok = true;

    IGES_CURVE* cp;

    while( bcur != ecur )
    {
        iEnt = *bcur >> 1;

        if( iEnt < 0 || iEnt >= sEnt )
        {
            ERRMSG << "\n + [CORRUPT FILE] referenced curve entity (";
            cerr << *bcur << ") does not exist\n";
            ok = false;
            ++bcur;
            continue;
        }

        tEnt = ((*entities)[iEnt])->GetEntityType();
        cp = dynamic_cast<IGES_CURVE*>((*entities)[iEnt]);

        if( !cp )
        {
            ERRMSG << "\n + [ERROR] referenced entity is not a curve (Type: ";
            cerr << tEnt << ", DE: " << *bcur << ")\n";
            ok = false;
            ++bcur;
            continue;
        }

        if( tEnt == 102 )
        {
            ERRMSG << "\n + [VIOLATION] referenced entity is Type 102\n";
            ok = false;
            ++bcur;
            continue;
        }

        bool dup = false;

        if( !(*entities)[iEnt]->AddReference( this, dup ) )
        {
            ERRMSG << "\n + [INFO] failed to add reference to child\n";
            cerr << "+ Entity type: ";

            if ( (*entities)[iEnt]->GetEntityType() == 0 )
            {
                cerr << "NULL/";
                cerr << (((IGES_ENTITY_NULL*)(*entities)[iEnt])->GetTrueEntityType());
                cerr << "\n";
            }
            else
            {
                cerr << ((*entities)[iEnt]->GetEntityType()) << "\n";
            }

            ok = false;
        }
        else if( !dup )
        {
            curves.push_back( cp );
            ((*entities)[iEnt])->Associate( entities );
        }
        else
        {
            ERRMSG << "\n + [BUG] duplicate curve entry added to composite curve\n";
        }

        ++bcur;
    }

    // go through the list and check:
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

    std::list<IGES_CURVE*>::iterator sp = curves.begin();
    std::list<IGES_CURVE*>::iterator ep = curves.end();
    std::list<IGES_CURVE*>::iterator pp;   // iterator to previous item
    int acc = 0;
    int jEnt = 0;

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
     * 130 ENT_OFFSET_CURVE (Due to complexity, postpone this one)
     */

    while( sp != ep )
    {
        iEnt = (*sp)->GetEntityType();

        if( iEnt != 100 && iEnt != 104 && iEnt !=110 && iEnt != 112
            && iEnt != 116 && iEnt != 126 && iEnt != 132 && iEnt != 106 )
        {
            ERRMSG << "\n + [INFO] Unsupported entity (";
            cerr << iEnt << ") in Composite Curve\n";
            ok = false;
        }

        // note: the specification is not very clear on this issue;
        // the specification prohibits 2 consecutive Entity 116 and
        // also 2 consecutive Entity 132, but there is no prohibition
        // of the interleaved series 116,132,116,132... or similar.
        // In this interpretation of the standard, the only prohibitions
        // are 2 consecutive of 116, and 2 consecutive of 132 with the
        // exception (per spec) if these are the only entities.
        if( acc > 0 )
        {
            if( (iEnt == 116 || iEnt == 132) && acc > 0 )
            {
                // check rule about consecutive 116/132
                if( acc > 0 && jEnt == iEnt && iCurves.size() != 2 )
                {
                    ERRMSG << "\n + [VIOLATION] Consecutive Type 116/132 in Composite Curve\n";
                    ok = false;
                }
                else
                {
                    // the Use Flag must be Type 04 (logical/positional)
                    use = STAT_USE_LOGICAL;
                }
            }

            // check that StartPoint[N] == EndPoint[N-1]
            // we must execute the transform since 2D curves may be tested
            // against 3D curves
            MCAD_POINT p1;
            MCAD_POINT p2;
            double dN;

            if( !parent )
                dN = 1e-9;
            else
                dN = parent->globalData.minResolution;

            if( !(*sp)->GetStartPoint( p1, true )
                || !(*pp)->GetEndPoint( p2, true ) )
            {
                ERRMSG << "\n + [INFO] one of Start Point or End Point could not be determined\n";
                ok = false;
            }

            // note: print a warning rather than halting on a mismatch; the case of
            // parameter curves make strict matching difficult, for example a parameter
            // curve on a surface of revolution will have start/end points
            // (0, 0)/(PI, 0)
            if( !PointMatches(p1, p2, dN) )
            {
                ERRMSG << "\n + [INFO] sequencing condition not met for Curve Entity\n";
                cerr << " + EndPoint[N-1]: (" << p2.x << ", " << p2.y << ", " << p2.z << ")\n";
                cerr << " + StartPoint[N]: (" << p1.x << ", " << p1.y << ", " << p1.z << ")\n";
                cerr << " + DE of Curve Entity: " << sequenceNumber << "\n";
                cerr << " + N: " << acc << "\n";
                IGES_ENTITY* tt;

                if( (*sp)->GetTransform( &tt ))
                {
                    if( tt != NULL )
                    {
                        cerr << " + [INFO] DE: " << tt->GetDESequence() << "\n";
                        print_transform( &((IGES_ENTITY_124*)tt)->T );
                    }
                }

            }

        }

        jEnt = iEnt;
        pp = sp;
        ++acc;
        ++sp;
    }

    if( curves.size() == 1 && ( iEnt == 116 || iEnt == 132 ) )
    {
        ERRMSG << "\n + [VIOLATION] single entity of type 116/132 in Composite Curve\n";
        ok = false;
    }

    return ok;
}


bool IGES_ENTITY_102::format( int &index )
{
    pdout.clear();
    iCurves.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
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
    string lstr = ostr.str();
    string tstr;

    std::list<IGES_CURVE*>::iterator scur = curves.begin();
    std::list<IGES_CURVE*>::iterator ecur = curves.end();

    while( scur != ecur )
    {
        iCurves.push_back( (*scur)->GetDESequence() );
        ++scur;
    }

    ostr.str("");
    ostr << iCurves.size();

    if( iCurves.empty() && extras.empty() )
        ostr << rd;
    else
        ostr << pd;

    lstr += ostr.str();

    std::list<int>::iterator sSec = iCurves.begin();
    std::list<int>::iterator eSec = iCurves.end();

    while( sSec != eSec )
    {
        ostr.str("");
        ostr << *sSec;
        ++sSec;

        if( sSec == eSec && extras.empty() )
            ostr << rd;
        else
            ostr << pd;

        tstr = ostr.str();

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    if( !extras.empty() && !formatExtraParams( lstr, index, pd, rd ) )
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


bool IGES_ENTITY_102::rescale( double sf )
{
    // there is nothing to scale
    return true;
}


bool IGES_ENTITY_102::Unlink( IGES_ENTITY* aChildEntity )
{
    if( IGES_ENTITY::Unlink( aChildEntity ) )
        return true;

    // check the list of curves; if one is unlinked and it
    // is not a terminal entity then we must relinquish
    // links to all entities
    std::list<IGES_CURVE*>::iterator sp = curves.begin();
    std::list<IGES_CURVE*>::iterator ep = curves.end();

    bool clear_all = false;

    while( sp != ep )
    {
        if( aChildEntity == *sp )
        {
            if( sp != curves.begin() && sp != (--(curves.end())) )
                clear_all = true;

            sp = curves.erase( sp );
            break;
        }

        ++sp;
    }

    if( clear_all )
    {
        sp = curves.begin();
        ep = curves.end();

        while( sp != ep )
        {
            (*sp)->DelReference( this );
            ++sp;
        }

        curves.clear();
    }

    return true;
}


bool IGES_ENTITY_102::IsOrphaned( void )
{
    // if this entity has no segments then it has no
    // purpose for existence
    if( (refs.empty() && depends != STAT_INDEPENDENT)
        || curves.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_102::IGES_ENTITY_102::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed to method\n";
        return false;
    }

    if( aParentEntity->GetEntityType() == 102 )
    {
        ERRMSG << "\n + [VIOLATION] may not reference Entity 102\n";
        cerr << " + [INFO] parent entity sequence number (may not be valid): ";
        cerr << aParentEntity->GetDESequence() << "\n";
        cerr << " + [INFO] this object's entity sequence number (may not be valid): ";
        cerr << sequenceNumber << "\n";
        return false;
    }

    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
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
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Composite Curve Entity\n";
        pdout.clear();
        return false;
    }

    if( !curves.empty() )
    {
        ERRMSG << "\n + [INFO] the Composite Curve Entity currently contains data\n";
        pdout.clear();
        return false;
    }

    iCurves.clear();

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    idx = pdout.find( pd );

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    int nSeg;

    if( !ParseInt( pdout, idx, nSeg, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of segments in the Composite Curve\n";
        pdout.clear();
        return false;
    }

    if( nSeg < 1 )
    {
        ERRMSG << "\n + [INFO] invalid number of entities: " << nSeg << "\n";
        pdout.clear();
        return false;
    }

    int i;
    int ent;

    for( i = 0; i < nSeg; ++i )
    {
        if( !ParseInt( pdout, idx, ent, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the entity DE index\n";
            pdout.clear();
            return false;
        }

        if( ent < 1 || (ent & 1) == 0 || ent > 9999997 )
        {
            ERRMSG << "\n + [INFO] invalid DE index (" << ent << ")\n";
            pdout.clear();
            return false;
        }

        iCurves.push_back( ent );
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

    // note: normally a scale would be performed here (re. globalData.convert)
    // but this entity does not own scalable data.

    pdout.clear();
    return true;
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


int IGES_ENTITY_102::GetNCurves( void )
{
    return (int)curves.size();
}


IGES_CURVE* IGES_ENTITY_102::GetCurve( int index )
{
    if( index < 0 || index >= (int)curves.size() )
    {
        ERRMSG << "\n + [INFO] invalid index (" << index << ")\n";
        return NULL;
    }

    std::list<IGES_CURVE*>::iterator sl = curves.begin();

    for( int i = 0; i < index; ++i )
        ++sl;

    return *sl;
}


bool IGES_ENTITY_102::GetStartPoint( MCAD_POINT& pt, bool xform )
{
    if( curves.empty() )
        return false;

    std::list<IGES_CURVE*>::iterator sc = curves.begin();

    if( !(*sc)->GetStartPoint( pt, xform ) )
        return false;

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


bool IGES_ENTITY_102::GetEndPoint( MCAD_POINT& pt, bool xform )
{
    if( curves.empty() )
        return false;

    std::list<IGES_CURVE*>::reverse_iterator sc = curves.rbegin();

    if( !(*sc)->GetEndPoint( pt, xform ) )
        return false;

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


int IGES_ENTITY_102::GetNSegments( void )
{
    return GetNCurves();
}


bool IGES_ENTITY_102::IsClosed( void )
{
    if( curves.empty() )
        return false;

    std::list<IGES_CURVE*>::iterator sc = curves.begin();
    std::list<IGES_CURVE*>::iterator ec = curves.end();
    std::list<IGES_CURVE*>::reverse_iterator lc = curves.rbegin();

    // we require at least 1 item which reports segments > 0
    if( curves.size() == 1 && (*sc)->GetEntityType() != ENT_CIRCULAR_ARC )
        return false;

    MCAD_POINT p1;
    MCAD_POINT p2;

    if( !(*sc)->GetStartPoint( p1, true )
        || !(*lc)->GetEndPoint( p2, true ) )
    {
        ERRMSG << "\n + [info] one of Start Point or End Point could not be determined\n";
        return false;
    }

    bool has_segments = false;

    while( sc != ec )
    {
        if( (*sc)->GetNSegments() > 0 )
        {
            has_segments = true;
            break;
        }

        ++sc;
    }

    if( !has_segments )
        return false;

    double rm = 0.001;

    if( parent )
        rm = parent->globalData.minResolution;

    return PointMatches( p1, p2, rm );
}


bool IGES_ENTITY_102::Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform )
{
    ERRMSG << "\n + [WARNING] method invoked on composite curve\n";
    return false;
}


bool IGES_ENTITY_102::AddSegment( IGES_CURVE* aSegment )
{
    if( !aSegment )
    {
        ERRMSG << "\n + [ERROR] null pointer passed as aSegment\n";
        return false;
    }

    if( aSegment->GetEntityType() == ENT_COMPOSITE_CURVE )
    {
        ERRMSG << "\n + [VIOLATION] segment pointer is a composite curve\n";
        return false;
    }

    if( !curves.empty() && IsClosed() )
    {
        ERRMSG << "\n + [ERROR] curve is aready closed\n";
        return false;
    }

    bool dup = false;

    if( !aSegment->AddReference( this, dup ) )
    {
        ERRMSG << "\n + [ERROR] could not add reference\n";
        return false;
    }

    if( !dup )
    {
        if( !aSegment->SetDependency( STAT_DEP_PHY ) )
        {
            ERRMSG << "\n + [WARNING] could not set physical dependency on Entity #";
            cerr << aSegment->GetEntityType() << "\n";
        }

        curves.push_back( aSegment );
    }

    return true;
}
