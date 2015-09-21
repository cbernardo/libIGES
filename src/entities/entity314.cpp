/*
 * file: entity314.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 314: Color, Section 4.77, p.386 (414)
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

#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity314.h>
#include <entity124.h>

using namespace std;

IGES_ENTITY_314::IGES_ENTITY_314( IGES* aParent ) : IGES_ENTITY( aParent ),
CNAME( cname )
{
    entityType = 314;
    form = 0;
    red = 85.0;
    green = 20.0;
    blue = 20.0;
    colorNum = COLOR_RED;
}


IGES_ENTITY_314::~IGES_ENTITY_314()
{
    return;
}


bool IGES_ENTITY_314::associate(std::vector<IGES_ENTITY *> *entities)
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

    if( pLevel )
    {
        ERRMSG << "\n + [VIOLATION] Level entity is set\n";
        pLevel->delReference(this);
        pLevel = NULL;
    }

    if( pView )
    {
        ERRMSG << "\n + [VIOLATION] View entity is set\n";
        pView->delReference(this);
        pView = NULL;
    }

    if( pTransform )
    {
        ERRMSG << "\n + [VIOLATION] Transform entity is set\n";
        pTransform->delReference(this);
        pTransform = NULL;
    }

    if( pLabelAssoc )
    {
        ERRMSG << "\n + [VIOLATION] Label Associativity is set\n";
        pLabelAssoc->delReference(this);
        pLabelAssoc = NULL;
    }

    if( pColor )
    {
        ERRMSG << "\n + [VIOLATION] Color entity is set\n";
        pColor->delReference(this);
        pColor = NULL;
    }

    return true;
}


bool IGES_ENTITY_314::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    // ensure compliance of STATUS NUM with specification
    depends = STAT_INDEPENDENT;     // fixed value
    use = STAT_USE_DEFINITION;      // fixed value

    if( red < 0.0 || red > 100.0 )
    {
        ERRMSG << "\n + [INFO] invalid value for RED (" << red << ")\n";
        red = 85.0;
    }

    if( green < 0.0 || green > 100.0 )
    {
        ERRMSG << "\n + [INFO] invalid value for GREEN (" << green << ")\n";
        green = 20.0;
    }

    if( blue < 0.0 || blue > 100.0 )
    {
        ERRMSG << "\n + [INFO] invalid value for BLUE (" << blue << ")\n";
        blue = 20.0;
    }

    if( cname.empty() )
        cname = "none";

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
    string fStr = ostr.str();
    string tStr;

    if( !FormatPDREal( tStr, red, pd, 0.1 ) )
    {
        ERRMSG << "\n + [INFO] could not format RED\n";
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tStr, green, pd, 0.1 ) )
    {
        ERRMSG << "\n + [INFO] could not format GREEN\n";
        pdout.clear();
        return false;
    }

    AddPDItem( tStr, fStr, pdout, index, sequenceNumber, pd, rd );

    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    if( !FormatPDREal( tStr, blue, idelim, 0.1 ) )
    {
        ERRMSG << "\n + [INFO] could not format yEnd\n";
        pdout.clear();
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


bool IGES_ENTITY_314::rescale( double sf )
{
    // there is nothing to scale
    return true;
}


bool IGES_ENTITY_314::unlink(IGES_ENTITY *aChild)
{
    // check if there are any extra entities to unlink
    return IGES_ENTITY::unlink(aChild);
}


bool IGES_ENTITY_314::isOrphaned( void )
{
    if( refs.empty() )
        return true;

    return false;
}


bool IGES_ENTITY_314::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_314::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_314::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    depends = STAT_INDEPENDENT;     // fixed value
    use = STAT_USE_DEFINITION;      // fixed value

    structure = 0;                  // N.A.
    lineFontPattern = 0;            // N.A.
    level = 0;                      // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    labelAssoc = 0;                 // N.A.
    lineWeightNum = 0;              // N.A.

    if( form != 0 )
    {
        ERRMSG << "\n + [CORRUPT FILE] non-zero Form Number in Color Def.\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_314::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Color Definition\n";
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

    if( !ParseReal( pdout, idx, red, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for RED\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, green, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for RED\n";
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, blue, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no value for RED\n";
        pdout.clear();
        return false;
    }

    if( !eor && !ParseHString( pdout, idx, cname, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] problems encountered while parsing color name\n";
        pdout.clear();
        return false;
    }

    if( red < 0.0 || red > 100.0 )
    {
        ERRMSG << "\n + [INFO] invalid value for RED (" << red << ")\n";
        red = 85.0;
    }

    if( green < 0.0 || green > 100.0 )
    {
        ERRMSG << "\n + [INFO] invalid value for GREEN (" << green << ")\n";
        green = 20.0;
    }

    if( blue < 0.0 || blue > 100.0 )
    {
        ERRMSG << "\n + [INFO] invalid value for BLUE (" << blue << ")\n";
        blue = 20.0;
    }

    if( cname.empty() )
        cname = "none";

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


bool IGES_ENTITY_314::SetEntityForm( int aForm )
{
    if( aForm != 0 )
    {
        ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity only supports Form 0 (requested form: ";
        cerr << aForm << ")\n";
    }

    return true;
}


bool IGES_ENTITY_314::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    if( aDependency != 0 )
    {
        ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity only supports STAT_INDEPENDENT\n";
    }

    return true;
}


bool IGES_ENTITY_314::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase != 2 )
    {
        ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity only supports STAT_USE_DEFINITION\n";
    }

    return true;
}


bool IGES_ENTITY_314::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // value ignored
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support hierarchy\n";
    return true;
}


bool IGES_ENTITY_314::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Line Font Pattern\n";
    return true;
}


bool IGES_ENTITY_314::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Line Font Pattern\n";
    return true;
}


bool IGES_ENTITY_314::SetLevel( int aLevel )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Level\n";
    return true;
}


bool IGES_ENTITY_314::SetLevel( IGES_ENTITY* aLevel )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Level\n";
    return true;
}


bool IGES_ENTITY_314::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support View\n";
    return true;
}


bool IGES_ENTITY_314::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Transform\n";
    return true;
}


bool IGES_ENTITY_314::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Label Associativity\n";
    return true;
}


bool IGES_ENTITY_314::SetColor( IGES_COLOR aColor )
{
    if( aColor > COLOR_NONE && aColor < COLOR_END )
    {
        colorNum = aColor;
        return true;
    }

    ERRMSG << "\n + [WARNING] [BUG] Color Number must be one of 1 .. 8\n";
    return false;
}


bool IGES_ENTITY_314::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support child Color Definition Entity\n";
    return true;
}


bool IGES_ENTITY_314::SetVisibility(bool isVisible)
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Blank Status (visibility)\n";
    return true;
}


bool IGES_ENTITY_314::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [WARNING] [BUG] Color Definition Entity does not support Line Weight\n";
    return true;
}


const char* IGES_ENTITY_314::GetCName( void )
{
    return cname.c_str();
}


void IGES_ENTITY_314::SetCName( const char* aName )
{
    if( NULL == aName )
        cname.clear();
    else
    cname = aName;

    return;
}
