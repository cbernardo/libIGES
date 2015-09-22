/*
 * file: entityNULL.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 0: NULL, Section 4.2, p.65 (93)
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
#include <iges.h>
#include <iges_io.h>
#include <entityNULL.h>

using namespace std;


IGES_ENTITY_NULL::IGES_ENTITY_NULL( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = ENT_NULL;
    trueEntity = ENT_NULL;

    return;
}


IGES_ENTITY_NULL::~IGES_ENTITY_NULL()
{
    return;
}


void IGES_ENTITY_NULL::Compact( void )
{
    return;
}


void IGES_ENTITY_NULL::setEntityType( int aEntityID )
{
    switch( aEntityID )
    {
        case ENT_CIRCULAR_ARC:
        case ENT_COMPOSITE_CURVE:
        case ENT_CONIC_ARC :
        case ENT_COPIOUS_DATA :
        case ENT_PLANE :
        case ENT_LINE:
        case ENT_PARAM_SPLINE_CURVE:
        case ENT_PARAM_SPLINE_SURFACE:
        case ENT_POINT:
        case ENT_RULED_SURFACE:
        case ENT_SURFACE_OF_REVOLUTION:
        case ENT_TABULATED_CYLINDER:
        case ENT_TRANSFORMATION_MATRIX:
        case ENT_FLASH:
        case ENT_NURBS_CURVE:
        case ENT_NURBS_SURFACE:
        case ENT_OFFSET_CURVE:
        case ENT_OFFSET_SURFACE:
        case ENT_BOUNDARY:
        case ENT_CURVE_ON_PARAMETRIC_SURFACE:
        case ENT_BOUNDED_SURFACE:
        case ENT_TRIMMED_PARAMETRIC_SURFACE:
        case ENT_PLANE_SURFACE:
        case ENT_RIGHT_CIRCULAR_CYLINDRICAL_SURFACE:
        case ENT_RIGHT_CIRCULAR_CONICAL_SURFACE:
        case ENT_SPHERICAL_SURFACE:
        case ENT_TOROIDAL_SURFACE:
        case ENT_BLOCK:
        case ENT_RIGHT_ANGULAR_WEDGE:
        case ENT_RIGHT_CIRCULAR_CYLINDER:
        case ENT_RIGHT_CIRCULAR_CONE_FRUSTUM:
        case ENT_SPHERE:
        case ENT_TORUS:
        case ENT_SOLID_OF_REVOLUTION:
        case ENT_SOLID_OF_LINEAR_EXTRUSION:
        case ENT_ELLIPSOID:
        case ENT_BOOLEAN_TREE:
        case ENT_SELECTED_COMPONENT:
        case ENT_SOLID_ASSEMBLY:
        case ENT_SOLID_INSTANCE:
        case ENT_MANIFOLD_SOLID_BREP:
        case ENT_VERTEX:
        case ENT_EDGE:
        case ENT_LOOP:
        case ENT_FACE:
        case ENT_SHELL:
        case ENT_DIRECTION:
        case ENT_ANGULAR_DIMENSION:
        case ENT_CURVE_DIMENSION:
        case ENT_DIAMETER_DIMENSION:
        case ENT_FLAG_NOTE:
        case ENT_GENERAL_LABEL:
        case ENT_GENERAL_NOTE:
        case ENT_NEW_GENERAL_NOTE:
        case ENT_LEADER_ARROW:
        case ENT_LINEAR_DIMENSION:
        case ENT_ORDINATE_DIMENSION:
        case ENT_POINT_DIMENSION:
        case ENT_RADIUS_DIMENSION:
        case ENT_GENERAL_SYMBOL:
        case ENT_SECTIONED_AREA:
        case ENT_NULL:
        case ENT_CONNECT_POINT:
        case ENT_NODE:
        case ENT_FINITE_ELEMENT:
        case ENT_NODAL_DISPLACEMENT_AND_ROTATION:
        case ENT_NODAL_RESULTS:
        case ENT_ELEMENT_RESULTS:
        case ENT_ASSOCIATIVITY_DEFINITION:
        case ENT_LINE_FONT_DEFINITION:
        case ENT_MACRO_DEFINITION:
        case ENT_SUBFIGURE_DEFINITION:
        case ENT_TEXT_FONT_DEFINITION:
        case ENT_TEXT_DISPLAY_TEMPLATE:
        case ENT_COLOR_DEFINITION:
        case ENT_UNITS_DATA:
        case ENT_NETWORK_SUBFIGURE_DEFINITION:
        case ENT_ATTRIBUTE_TABLE_DEFINITION:
        case ENT_ASSOCIATIVITY_INSTANCE:
        case ENT_DRAWING:
        case ENT_PROPERTY:
        case ENT_SINGULAR_SUBFIGURE_INSTANCE:
        case ENT_VIEW:
        case ENT_RECTANGULAR_ARRAY_SUBFIGURE_INSTANCE:
        case ENT_CIRCULAR_ARRAY_SUBFIGURE_INSTANCE:
        case ENT_EXTERNAL_REFERENCE:
        case ENT_NODAL_LOAD_OR_CONSTRAINT:
        case ENT_NETWORK_SUBFIGURE_INSTANCE:
        case ENT_ATTRIBUTE_TABLE_INSTANCE:
            break;

        default:
            if( (aEntityID >= ENT_START_IMPLEMENTOR_MACRO_INSTANCE_1
                && aEntityID <= ENT_END_IMPLEMENTOR_MACRO_INSTANCE_1)
                ||(aEntityID >= ENT_START_IMPLEMENTOR_MACRO_INSTANCE_2
                && aEntityID <= ENT_END_IMPLEMENTOR_MACRO_INSTANCE_2) )
            {
                ERRMSG << "\n + Unsupported entity (#" << aEntityID << ")\n";
            }
            else
            {
                ERRMSG << "\n + [CORRUPT FILE] Invalid entity (#" << aEntityID << ")\n";
            }

            break;
    }

    trueEntity = aEntityID;

    return;
}


bool IGES_ENTITY_NULL::associate(std::vector<IGES_ENTITY *> *entities)
{
    // Since a NULL entity knows nothing of the Parameter Data,
    // the only associations which can be formed are the ones
    // provided by the base class implementation of associate().

    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] association failed; see messages above\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_NULL::unlink(IGES_ENTITY *aChild)
{
    // there are no properly established links so in
    // effect this routine always succeeds
    return true;
}


bool IGES_ENTITY_NULL::isOrphaned( void )
{
    // this entity can be deleted at any time
    return true;
}


bool IGES_ENTITY_NULL::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    // note: Add Reference will be called by any parent entities;
    // returning false notifies those entities that this item
    // is not currently supported.
    return false;
}


bool IGES_ENTITY_NULL::delReference(IGES_ENTITY *aParentEntity)
{
    ERRMSG << "\n + [BUG] invoking delReference on NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    entityType = trueEntity;

    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    entityType = ENT_NULL;

    return true;
}


bool IGES_ENTITY_NULL::readPD(std::ifstream &aFile, int &aSequenceVar)
{
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

    IGES_RECORD rec;

#ifdef DEBUG
    cout << "[INFO] Entity(NULL/" << trueEntity;
    cout << ") Parameter Data Record for entity at DE " << sequenceNumber << "\n";
#endif

    for(int i = 0; i < paramLineCount; ++i)
    {
        if( !ReadIGESRecord( &rec, aFile ) )
        {
            ERRMSG << "\n + could not read Parameter Data\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            cerr << " + [INFO] Parameter Line # (" << (parameterData + i) << ")\n";
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
            return false;
        }

        if( rec.index != (parameterData + i) )
        {
            ERRMSG << "\n + [BAD FILE] incorrect Parameter Line # (" << rec.index << ")\n";
            cerr << " + [INFO] Parameter Data Index (" << parameterData << ")\n";
            cerr << " + [INFO] Parameter Line Count (" << paramLineCount << ")\n";
            cerr << " + [INFO] Expected Parameter Line # (" << (parameterData + i) << ")\n";
            return false;
        }
    }

#ifdef DEBUG
    cout << "-----\n";
#endif

    aSequenceVar += paramLineCount;

    return true;
}


bool IGES_ENTITY_NULL::format( int &index )
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::writeDE(std::ofstream &aFile)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::writePD(std::ofstream &aFile)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return true;    // do not interfere with other write operations
}


bool IGES_ENTITY_NULL::SetEntityForm(int aForm)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetLineFontPattern(IGES_LINEFONT_PATTERN aPattern)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetLineFontPattern(IGES_ENTITY* aPattern)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetLevel(int aLevel)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetLevel(IGES_ENTITY* aLevel)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetView(IGES_ENTITY* aView)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetTransform(IGES_ENTITY* aTransform)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetLabelAssoc(IGES_ENTITY* aLabelAssoc)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetColor(IGES_COLOR aColor)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetColor(IGES_ENTITY* aColor)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetLineWeightNum(int aLineWeight)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetDependency(IGES_STAT_DEPENDS aDependency)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetEntityUse(IGES_STAT_USE aUseCase)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::SetHierarchy(IGES_STAT_HIER aHierarchy)
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}


bool IGES_ENTITY_NULL::rescale( double sf )
{
    ERRMSG << "\n + [BUG] invoking function in NULL Entity\n";
    return false;
}
