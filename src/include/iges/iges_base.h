/*
 * file: iges_base.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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
 * Description: common basic structures required for the implementation
 * of the Initial Graphics Exchange Specification (IGES) v6.
 */

#ifndef IGES_BASE_H
#define IGES_BASE_H

#include <cstddef>

// precision used to represent angles
#define IGES_ANGLE_RES (1.0e-15)


enum IGES_COLOR
{
    COLOR_START = 0,
    COLOR_NONE = 0,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_END
};


// UNIT types in IGES
enum IGES_UNIT
{
    UNIT_START = 1,
    UNIT_INCH = UNIT_START,
    UNIT_IN   = UNIT_INCH,
    UNIT_MILLIMETER,
    UNIT_MM = UNIT_MILLIMETER,
    UNIT_EXTERN,        // unit specified in Global Section Part 15;
                        // must conform to IEEE-260-1978 or MIL-STD-12D(1981)
    UNIT_FOOT,
    UNIT_MILE,
    UNIT_METER,
    UNIT_KILOMETER,
    UNIT_MIL,
    UNIT_MICRON,
    UNIT_CENTIMETER,
    UNIT_MICROINCH,
    UNIT_END
};


// possible types of basic IGES data
enum IGES_TYPE
{
    IGES_TYPE_START = 0,
    IGES_TYPE_INT = 0,
    IGES_TYPE_FLOAT,
    IGES_TYPE_DOUBLE,
    IGES_TYPE_LSTRING,
    IGES_TYPE_HSTRING,
    IGES_TYPE_POINTER,
    IGES_TYPE_LOGICAL,
    IGES_TYPE_END
};


// recognized drafting standards
enum IGES_DRAFTING_STANDARD
{
    DRAFT_START = 0,
    DRAFT_NONE = 0,     // default, no standard specified
    DRAFT_ISO,          // International Organization for Standardization
    DRAFT_AFNOR,        // French Association for Standardization
    DRAFT_ANSI,         // American National Standards Institute
    DRAFT_BSI,          // British Standards Institute
    DRAFT_CSA,          // Canadian Standards Association
    DRAFT_DIN,          // German Institute for Standardization
    DRAFT_JIS,          // Japanese Institute for Standardization
    DRAFT_END
};


// line font patterns
enum IGES_LINEFONT_PATTERN
{
    LINEFONT_START = 0,
    LINEFONT_NONE = 0,
    LINEFONT_SOLID,
    LINEFONT_DASHED,
    LINEFONT_PHANTOM,
    LINEFONT_CENTERLINE,
    LINEFONT_DOTTED,
    LINEFONT_END
};


// Types of IGES entities (94 entities, some of which have several forms)
enum IGES_ENTITY_TYPE
{
    // NOTE: Check for UNTESTED entities or UNTESTED FORMS and keep in
    // mind that they may work as specified but this is not guaranteed
    // due to lack of testing by the industry association. Note that all
    // BREP association classes (loop, face, edge, vertex, shell) are
    // not thoroughly tested so it is safest to define objects as CSG
    // rather than BREP.

    // curve and surface entities
    ENT_CIRCULAR_ARC = 100,
    ENT_COMPOSITE_CURVE = 102,
    ENT_CONIC_ARC = 104,
    ENT_COPIOUS_DATA = 106,
    ENT_PLANE = 108,
    ENT_LINE = 110,
    ENT_PARAM_SPLINE_CURVE = 112,
    ENT_PARAM_SPLINE_SURFACE = 114,
    ENT_POINT = 116,
    ENT_RULED_SURFACE = 118,
    ENT_SURFACE_OF_REVOLUTION = 120,
    ENT_TABULATED_CYLINDER = 122,
    ENT_TRANSFORMATION_MATRIX = 124,
    ENT_FLASH = 125,
    ENT_NURBS_CURVE = 126,
    ENT_NURBS_SURFACE = 128,
    ENT_OFFSET_CURVE = 130,
    ENT_OFFSET_SURFACE = 140,
    ENT_BOUNDARY = 141,                             // ** untested
    ENT_CURVE_ON_PARAMETRIC_SURFACE = 142,
    ENT_BOUNDED_SURFACE = 143,                      // ** untested
    ENT_TRIMMED_PARAMETRIC_SURFACE = 144,
    ENT_PLANE_SURFACE = 190,                        // ** untested
    ENT_RIGHT_CIRCULAR_CYLINDRICAL_SURFACE = 192,   // ** untested
    ENT_RIGHT_CIRCULAR_CONICAL_SURFACE = 194,       // ** untested
    ENT_SPHERICAL_SURFACE = 196,                    // ** untested
    ENT_TOROIDAL_SURFACE = 198,                     // ** untested

    // constructive solid geometry (CSG) entities
    ENT_BLOCK = 150,
    ENT_RIGHT_ANGULAR_WEDGE = 152,
    ENT_RIGHT_CIRCULAR_CYLINDER = 154,
    ENT_RIGHT_CIRCULAR_CONE_FRUSTUM = 156,
    ENT_SPHERE = 158,
    ENT_TORUS = 160,
    ENT_SOLID_OF_REVOLUTION = 162,
    ENT_SOLID_OF_LINEAR_EXTRUSION = 164,
    ENT_ELLIPSOID = 168,

    // CSG operators
    ENT_BOOLEAN_TREE = 180,
    ENT_SELECTED_COMPONENT = 182,               // ** untested
    ENT_SOLID_ASSEMBLY = 184,
    ENT_SOLID_INSTANCE = 430,

    // Boundary Representation (BREP) entities
    ENT_MANIFOLD_SOLID_BREP = 186,              // ** untested
    ENT_VERTEX = 502,                           // ** untested
    ENT_EDGE = 504,                             // ** untested
    ENT_LOOP = 508,                             // ** untested
    ENT_FACE = 510,                             // ** untested
    ENT_SHELL = 514,                            // ** untested

    // additional BREP analytical entities
    ENT_DIRECTION = 123,                        // ** untested

    // annotation entities
    ENT_ANGULAR_DIMENSION = 202,
    ENT_CURVE_DIMENSION = 204,                  // ** untested
    ENT_DIAMETER_DIMENSION = 206,
    ENT_FLAG_NOTE = 208,
    ENT_GENERAL_LABEL = 210,
    ENT_GENERAL_NOTE = 212,                     // ** untested
    ENT_NEW_GENERAL_NOTE = 213,                 // ** untested
    ENT_LEADER_ARROW = 214,
    ENT_LINEAR_DIMENSION = 216,                 // ** untested
    ENT_ORDINATE_DIMENSION = 218,               // ** untested: form 1
    ENT_POINT_DIMENSION = 220,
    ENT_RADIUS_DIMENSION = 222,                 // ** untested: form 1
    ENT_GENERAL_SYMBOL = 228,                   // ** untested: form 1-3
    ENT_SECTIONED_AREA = 230,                   // ** untested: form 0-1

    // structure entities
    ENT_NULL = 0,
    ENT_CONNECT_POINT = 132,
    ENT_NODE = 134,
    ENT_FINITE_ELEMENT = 136,                   // ** untested
    ENT_NODAL_DISPLACEMENT_AND_ROTATION = 138,
    ENT_NODAL_RESULTS = 146,                    // ** untested: form 0-34
    ENT_ELEMENT_RESULTS = 148,                  // ** untested: form 0-34
    ENT_ASSOCIATIVITY_DEFINITION = 302,
    ENT_LINE_FONT_DEFINITION = 304,
    ENT_MACRO_DEFINITION = 306,                 // ** untested
    ENT_SUBFIGURE_DEFINITION = 308,
    ENT_TEXT_FONT_DEFINITION = 310,
    ENT_TEXT_DISPLAY_TEMPLATE = 312,
    ENT_COLOR_DEFINITION = 314,
    ENT_UNITS_DATA = 316,                       // ** untested
    ENT_NETWORK_SUBFIGURE_DEFINITION = 320,
    ENT_ATTRIBUTE_TABLE_DEFINITION = 322,
    ENT_ASSOCIATIVITY_INSTANCE = 402,           // ** untested: form 19-36
    ENT_DRAWING = 404,
    ENT_PROPERTY = 406,
    ENT_SINGULAR_SUBFIGURE_INSTANCE = 408,
    ENT_VIEW = 410,                             // ** untested: form 1
    ENT_RECTANGULAR_ARRAY_SUBFIGURE_INSTANCE = 412,
    ENT_CIRCULAR_ARRAY_SUBFIGURE_INSTANCE = 414,
    ENT_EXTERNAL_REFERENCE = 416,               // ** untested: form 3
    ENT_NODAL_LOAD_OR_CONSTRAINT = 418,
    ENT_NETWORK_SUBFIGURE_INSTANCE = 420,
    ENT_ATTRIBUTE_TABLE_INSTANCE = 422,
    ENT_START_IMPLEMENTOR_MACRO_INSTANCE_1 = 600,
    ENT_END_IMPLEMENTOR_MACRO_INSTANCE_1 = 699,
    ENT_START_IMPLEMENTOR_MACRO_INSTANCE_2 = 10000,
    ENT_END_IMPLEMENTOR_MACRO_INSTANCE_2 = 99999
};


enum IGES_STAT_DEPENDS
{
    STAT_INDEPENDENT = 0,
    STAT_DEP_PHY = 1,
    STAT_DEP_LOG = 2,
    STAT_DEP_PHYLOG = 3,
    STAT_DEP_END
};


enum IGES_STAT_USE
{
    STAT_USE_GEOMETRY = 0,
    STAT_USE_ANNOTATION = 1,
    STAT_USE_DEFINITION = 2,
    STAT_USE_OTHER = 3,
    STAT_USE_LOGICAL = 4,
    STAT_USE_2D_PARAMETRIC = 5,
    STAT_USE_CONSTRUCTION_GEOM = 6,
    STAT_USE_END
};


enum IGES_STAT_HIER
{
    STAT_HIER_ALL_SUB = 0,
    STAT_HIER_NO_SUB = 1,
    STAT_HIER_USE_PROP = 2,
    STAT_HIER_END
};


/**
 * Enum BTREE_OPERATOR
 * represents the types of operators allowed in a Boolean Tree
 */
enum BTREE_OPERATOR
{
    OP_START = 1,
    OP_UNION = 1,
    OP_INTERSECT,
    OP_DIFFERENCE,
    OP_END
};


/**
 * Struct BTREE_NODE
 * represents an IGES Boolean Tree parameter or operation.
 */
class IGES_ENTITY;
struct MCAD_API BTREE_NODE
{
    bool op;    // true if this is an operation; false if an entity pointer
    int  val;   // Operation code (1,2,3) or DE Sequence of entity
    IGES_ENTITY* pEnt;  // pointer to entity (Primitive, Solid Instance, BTree, or manifold solid BREP

    BTREE_NODE()
    {
        op = false;
        val = 0;
        pEnt = NULL;
    }
};


/**
 * Struct EDGE_DATA
 * stores information on instantiated entities representing
 * part of an Edge entity
 */
class IGES_ENTITY_502;
struct MCAD_API EDGE_DATA
{
    IGES_ENTITY* curv;
    IGES_ENTITY_502* svp;
    IGES_ENTITY_502* tvp;
    int sv;
    int tv;

    EDGE_DATA()
    {
        curv = NULL;
        svp = NULL;
        tvp = NULL;
        sv = 0;
        tv = 0;
    }
};

#endif  // IGES_BASE_H
