/*
 * file: entity406.cpp
 *
 * Copyright 2017, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 406: Property Entity, Section 4.98, p.476 (504)
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

#include <string>
#include <sstream>
#include <error_macros.h>
#include <core/iges.h>
#include <core/iges_io.h>
#include <core/entity124.h>
#include <core/entity406.h>

using namespace std;

IGES_ENTITY_406::IGES_ENTITY_406( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = ENT_PROPERTY;
    // Notes:
    // 1. default to Form 15 (name) since this is currently the only supported form.
    // 2. if other forms are implemented then we must default to Form 0 (invalid)
    //    and initialize data as NULL. Data shall be assigned when a Form is set.
    form = 15;  // default to Form 15: Name
    data = new std::string;

    // TODO: The parent must not have a name in the DE, otherwise that
    // name should become the default name of this object and the parent's
    // name shoud be changed to an empty string. Having both a name in
    // the DE as well as a Type 406 Form 15 name may be a violation of the
    // specification.

    return;
}   // IGES_ENTITY_406( IGES* aParent )


IGES_ENTITY_406::~IGES_ENTITY_406()
{
    // delete internal data
    switch( form )
    {
        case 15:
            if (data)
            {
                std::string *sp = (std::string *)data;
                delete sp;
                data = NULL;
            }
            break;

        default:
            ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
            break;
    }

    return;
}   // ~IGES_ENTITY_406()


const void * IGES_ENTITY_406::GetData()
{
    if( 15 == form )
    {
        return data;
    }

    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return NULL;
}


bool IGES_ENTITY_406::SetData(const void *Data)
{
    if (!Data)
        return false;

    if( 15 == form )
    {
        const char *cp = (const char *)Data;
        std::string* sp = (std::string*)data;
        *sp = cp;
        return true;
    }

    ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
    return false;
}


bool IGES_ENTITY_406::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    // unsupported entity traits which have associated pointers
    structure = 0;
    lineFontPattern = 0;
    view = 0;
    transform = 0;
    labelAssoc = 0;
    colorNum = 0;

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


bool IGES_ENTITY_406::unlink(IGES_ENTITY *aChildEntity)
{
    if( !aChildEntity )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed to method\n";
        return false;
    }

    // note: no special handling should be required on unlink
    return IGES_ENTITY::unlink(aChildEntity);
}


bool IGES_ENTITY_406::format( int &index )
{
    if (15 != form)
    {
        ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
        return false;
    }

    // Note: as other forms are implemented then the format methods
    // should be moved to Form-specific static methods in order to
    // minimize cluttering of IGES_ENTITY_406::format()

    pdout.clear();
    iExtras.clear();

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
    ostr << entityType << pd << 1 << pd;
    string fStr = ostr.str();
    string tStr;

    // note: 2 sets of OPTIONAL parameters may exist at the end of
    // any PD; see p.32/60+ for details; if optional parameters
    // need to be written then we should use 'pd' rather than 'rd'
    // in this call to FormatPDREal()
    char idelim;

    if( extras.empty() )
        idelim = rd;
    else
        idelim = pd;

    if (data)
    {
        tStr = *((std::string *)data);
    }
    else
    {
        tStr = idelim;
    }

    if( !AddPDHStr( tStr, fStr, pdout, index, sequenceNumber, pd, rd, idelim ) )
    {
        ERRMSG << "\n + [INFO] could not add Form 15 data (name)\n";
        return false;
    }

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
    std::cout << "PDOUT: '" << pdout << "'\n";

    paramLineCount = index - parameterData;

    return true;
}


bool IGES_ENTITY_406::rescale( double sf )
{
    // there is nothing to scale so the return value is always true
    return true;
}


bool IGES_ENTITY_406::isOrphaned( void )
{
    if((0 == form) || ( refs.empty() && depends != STAT_INDEPENDENT ))
        return true;

    return false;
}


bool IGES_ENTITY_406::addReference( IGES_ENTITY* aParentEntity,
                        bool& isDuplicate )
{
    // TODO: having a name in the DE as well as a Type 406 Form 15
    // may be a violation of the spec (see similar comment in initializer).
    // Ideally a parent's DE name field should be checked and altered
    // consistently if necessary.
    return IGES_ENTITY::addReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_406::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_406::readDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    lineFontPattern = 0;            // N.A.
    view = 0;                       // N.A.
    transform = 0;                  // N.A.
    labelAssoc = 0;                 // N.A.
    visible = true;                 // N.A., but true => Blank Status = 0
    use = STAT_USE_GEOMETRY;        // field ignored
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 15 )
    {
        ERRMSG << "\n + [INFO] invalid or unimplemented Form Number in Entity Type 406\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_406::readPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Entity Type 406\n";
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
    int nparams = 0;

    if( !ParseInt( pdout, idx, nparams, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read number of properties\n";
        pdout.clear();
        return false;
    }

    // Note: as more forms are supported the parsing should be performed
    // by more specialized routines in order to keep this function simple.

    // Parse Form 15
    if( nparams != 1 )
    {
        ERRMSG << "\n + [BAD FILE] unexpected number of parameters for Form 15\n";
        pdout.clear();
        return false;
    }

    std::string tstr;
    if( !ParseHString( pdout, idx, tstr, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read name property\n";
        pdout.clear();
        return false;
    }

    std::string *sp = (std::string *)data;
    *sp = tstr;

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


void IGES_ENTITY_406::Compact( void )
{
    IGES_ENTITY::Compact();
    // Note: For Form 15 there is nothing to do; if no
    // other form requires Compact() then this should
    // simply be deleted from the class,
    return;
}


bool IGES_ENTITY_406::SetEntityForm( int aForm )
{
    if( 15 != aForm)
    {
        ERRMSG << "\n + [WARNING] TO BE IMPLEMENTED\n";
        return false;
    }

    if (0 != form && aForm != form)
    {
        ERRMSG << "\n + [WARNING] [BUG] Form is already assigned and cannot be changed.\n";
        return false;
    }

    form = aForm;

    // Note: when forms other than Form 15 are supported, we must
    // instantiate the data structure here.

    return true;
}


bool IGES_ENTITY_406::SetLineFontPattern( IGES_LINEFONT_PATTERN aPattern )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}


bool IGES_ENTITY_406::SetLineFontPattern( IGES_ENTITY* aPattern )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}


bool IGES_ENTITY_406::SetView( IGES_ENTITY* aView )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}

bool IGES_ENTITY_406::SetTransform( IGES_ENTITY* aTransform )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}


bool IGES_ENTITY_406::SetLabelAssoc( IGES_ENTITY* aLabelAssoc )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}


bool IGES_ENTITY_406::SetLineWeightNum( int aLineWeight )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}


bool IGES_ENTITY_406::SetColor( IGES_COLOR aColor )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}


bool IGES_ENTITY_406::SetColor( IGES_ENTITY* aColor )
{
    ERRMSG << "\n + [WARNING] [BUG] method not supported by Entity Type 406\n";
    return true;
}
