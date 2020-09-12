/*
 * file: entity108.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 108: Plane Entity, Section 4.12, p.92+ (120+)
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

#include <sstream>
#include <error_macros.h>
#include <core/iges.h>
#include <core/iges_io.h>
#include <core/entity108.h>

/*
 * TODO:
 * + use a function to retrieve parameters; this would allow any applied TRANSFORM to be
 *   imposed.
 * + check that the associated curve is planar.
 */

using namespace std;

IGES_ENTITY_108::IGES_ENTITY_108( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 108;
    form = 0;           /* default unbounded plane */

    /* default plane (XY, Z = 0) */
    A = 0.0;
    B = 0.0;
    C = 1.0;
    D = 0.0;

    /* default no bounding curve */
    iPtr = 0;

    /* default symbol location and size */
    X = 0.0;
    Y = 0.0;
    Z = 0.0;
    return;
}


IGES_ENTITY_108::~IGES_ENTITY_108()
{
    if( PTR &&  !PTR->delReference(this) )
    {
        ERRMSG << "\n + [BUG] could not delete reference from a child entity\n";
    }

    return;
}


bool IGES_ENTITY_108::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    structure = 0;
    lineFontPattern = 0;
    level = 0;
    view = 0;
    transform = 0;
    labelAssoc = 0;
    lineWeightNum = 0;

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->delReference(this);
        pStructure = NULL;
    }

    if( pLineFontPattern )
    {
        ERRMSG << "\n + [VIOLATION] Line Font Pattern entity is set\n";
        pLineFontPattern->delReference(this);
        pLineFontPattern = NULL;
    }

    if( 1 < iPtr )
    {
        int iEnt = (iPtr >> 1);
        int sEnt = (int) entities->size();

        if( iEnt >= sEnt )
        {
            ERRMSG << "\n + [INFO] invalid pointer (DE: ";
            cerr << iPtr << " ) does not exist\n";
            return false;
        }

        PTR = dynamic_cast<IGES_CURVE*>((*entities)[iEnt]);

        if( NULL == PTR )
        {
            ERRMSG << "\n + [INFO] could not establish reference to child entity\n";
            cerr << " + Child Entity Type " << ((*entities)[iEnt])->GetEntityType() << "\n";
            return false;
        }

        // ensure that the curve is a closed curve
        // **CAVEAT** TODO: planarity of curve is not checked here
        if( !PTR->IsClosed())
        {
            ERRMSG << "\n + [VIOLATION] Child entity (Type: " << ((*entities)[iEnt])->GetEntityType();
            cerr << ", DE: " << iPtr << ") is not a closed curve\n";
            return false;
        }

        bool dup = false;

        if( !(*entities)[iEnt]->addReference( this, dup ))
        {
            ERRMSG << "\n + [INFO] could not establish reference to child entity\n";
            return false;
        }

        if( dup )
        {
            ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
            return false;
        }
    }

    return true;
}


bool IGES_ENTITY_108::format( int &index )
{
    if ((0 == form) && (NULL != PTR))
    {
        ERRMSG << "\n + [INFO]: Entity 108 Form 0 has an associated curve\n";
        return false;
    }

    if ((0 != form) && (NULL == PTR))
    {
        ERRMSG << "\n + [INFO]: Entity 108 Form " << form << " has no associated curve\n";
        return false;
    }

    pdout.clear();
    iExtras.clear();

    // ensure compliance of STATUS NUM with specification
    depends = STAT_INDEPENDENT;     // fixed value
    use = STAT_USE_DEFINITION;      // fixed value

    /* Validate parameters */
    if ((0.0 == A) && (0.0 == B) && (0.0 == C))
    {
        ERRMSG << "\n + [INFO] invalid plane definition (A,B,C = 0)\n";
        return false;
    }

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
    double uir = parent->globalData.minResolution;

    ostringstream ostr;
    ostr << entityType << pd;
    string fStr = ostr.str();
    string tStr;

    if( !FormatPDREal( tStr, A, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format planar coeff A\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, B, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format planar coeff B\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, C, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format planar coeff C\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, D, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format planar coeff D\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if (NULL != PTR)
    {
        ostr.str( "" );
        ostr << PTR->getDESequence() << pd;
        tStr = ostr.str();
    }
    else
    {
        FormatDEInt( tStr, 0 );
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, X, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format display X\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, Y, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format display Y\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, Z, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format display Z\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, Size, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format display Size\n";
        return false;
    }

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


bool IGES_ENTITY_108::rescale( double sf )
{
    // there is nothing to scale
    return true;
}


bool IGES_ENTITY_108::unlink(IGES_ENTITY *aChildEntity)
{
    // check if there are any extra entities to unlink
    if(IGES_ENTITY::unlink(aChildEntity) )
        return true;

    if( aChildEntity == PTR )
    {
        PTR = NULL;
        return true;
    }

    return false;
}


bool IGES_ENTITY_108::isOrphaned( void )
{
    if( (refs.empty() && (depends != STAT_INDEPENDENT) ) || ((0 != form) && (NULL != PTR)) )
    {
        return true;
    }

    return false;
}


bool IGES_ENTITY_108::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    if( !aParentEntity )
    {
        ERRMSG << "\n + [BUG] parent entity is NULL\n";
        return false;
    }

    if( aParentEntity == PTR )
    {
        ERRMSG << "\n + [BUG] child entity is it's own grandpa\n";
        return false;
    }

    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_108::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_108::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    use = STAT_USE_GEOMETRY;        // fixed
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if ((form != 0) && (form != 1) && (form != -1))
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number in Entity 108\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_108::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Entity 108\n";
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

    if( !ParseReal( pdout, idx, A, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for A\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, B, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for B\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, C, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for C\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, D, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for D\n";
        pdout.clear();
        return false;
    }

    /* check if this is a valid plane */
    if ((0.0 == A) && (0.0 == B) && (0.0 == C))
    {
        ERRMSG << "\n + [BAD FILE] invalid plane definition (A,B,C = 0)\n";
        return false;
    }

    if( !ParseInt(pdout, idx, iPtr, eor, pd, rd) )
    {
        ERRMSG << "\n + [BAD FILE] Entity 108 missing PTR\n";
        return false;
    }

    if( (0 == form) && (0 != iPtr) )
    {
        ERRMSG << "\n + [BAD FILE] Entity 108 Form 0 has non-zero PTR\n";
        return false;
    }

    if( (0 != form) && (0 == iPtr) )
    {
        ERRMSG << "\n + [BAD FILE] Entity 108 Form " << form << " has zero PTR\n";
        return false;
    }

    if( !ParseReal( pdout, idx, X, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for X\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, Y, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for Y\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, Z, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for Z\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, Size, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for Size\n";
        pdout.clear();
        return false;
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
    return true;
}


bool IGES_ENTITY_108::SetEntityForm( int aForm )
{
    if ((-1 > aForm) || (1 < aForm))
    {
        ERRMSG << "\n + [WARNING] [BUG] Entity 108 only supports Forms -1,0,1 (requested form: ";
        cerr << aForm << ")\n";
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_108::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( aDependency != 0 )
    {
        ERRMSG << "\n + [WARNING] [BUG] Entity 108 only supports STAT_INDEPENDENT\n";
    }

    return true;
}


bool IGES_ENTITY_108::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase != 2 )
    {
        ERRMSG << "\n + [WARNING] [BUG] Entity 108 only supports STAT_USE_DEFINITION\n";
    }

    return true;
}


bool IGES_ENTITY_108::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // value ignored
    ERRMSG << "\n + [WARNING] [BUG] Entity 108 does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_108::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [WARNING] [BUG] Entity 108 does not support Line Font Pattern\n";
    return true;
}


bool IGES_ENTITY_108::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [WARNING] [BUG] Entity 108 does not support Line Font Pattern\n";
    return true;
}


bool IGES_ENTITY_108::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Line Weight\n";
    return true;
}
