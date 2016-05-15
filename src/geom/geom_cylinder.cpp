/*
 * file: geom_cylinder.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: object to aid in creating a vertical cylindrical
 * surface within IGES.
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

#include <cmath>
#include <algorithm>
#include <error_macros.h>
#include <core/iges.h>
#include <geom/mcad_helpers.h>
#include <geom/geom_cylinder.h>
#include <core/entity100.h>
#include <core/entity102.h>
#include <core/entity110.h>
#include <core/entity120.h>
#include <core/entity124.h>
#include <core/entity126.h>
#include <core/entity142.h>
#include <core/entity144.h>
#include <sisl.h>

using namespace std;

// make a 3D linear NURB from 2 points
static bool makeNurb( double* p0, double* p1, SISLCurve** pCurve )
{
    double epar = 1e-8;
    int stat = 0;

    s1602( p0, p1, 2, 3, 0.0, &epar, pCurve, &stat );

    switch( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified problems creating NURBS curve\n";
            break;

        default:
            ERRMSG << "\n + [ERROR] could not create NURBS curve\n";
            return false;
            break;
    }

    return true;
}


IGES_GEOM_CYLINDER::IGES_GEOM_CYLINDER()
{
    init();
    return;
}


IGES_GEOM_CYLINDER::~IGES_GEOM_CYLINDER()
{
    clear();
    return;
}


void IGES_GEOM_CYLINDER::init( void )
{
    narcs = 0;
    radius = 0.0;

    for( int i = 0; i < 6; ++i )
        angles[i] = 0.0;

    return;
}


void IGES_GEOM_CYLINDER::clear( void )
{
    narcs = 0;
    radius = 0.0;

    for( int i = 0; i < 6; ++i )
        angles[i] = 0.0;

    for( int i = 0; i < 5; ++i )
    {
        arcs[i].x = 0.0;
        arcs[i].y = 0.0;
        arcs[i].z = 0.0;
    }

    return;
}


bool IGES_GEOM_CYLINDER::SetParams( MCAD_POINT center, MCAD_POINT start, MCAD_POINT end )
{
    clear();

    if( center.z < 0.0 || center.z > 0.0
        || start.z < 0.0 || start.z > 0.0
        || end.z < 0.0 || end.z > 0.0 )
    {
        ERRMSG << "\n + [ERROR] non-zero Z values\n";
        return false;
    }

    if( PointMatches( center, start, 1e-8 )
        || PointMatches( center, end, 1e-8 ) )
    {
        ERRMSG << "\n + [ERROR] zero radius\n";
        return false;
    }

    MCAD_POINT p0 = center - start;
    double rad1 = sqrt( p0.x*p0.x + p0.y*p0.y );
    p0 = center - end;
    double rad2 = sqrt( p0.x*p0.x + p0.y*p0.y );

    if( abs(rad1 - rad2) > 1e-8 )
    {
        ERRMSG << "\n + [ERROR] radii differ by more than 1e-8\n";
        return false;
    }

    radius = rad1;
    arcs[0] = center;

    if( PointMatches( start, end, 1e-8 ) )
    {
        arcs[1] = center;
        arcs[1].x += radius;
        arcs[2] = center;
        arcs[2].x -= radius;
        arcs[3] = arcs[1];

        narcs = 2;

        angles[0] = 0;
        angles[1] = M_PI;
        angles[2] = M_PI;
        angles[3] = 2.0 * M_PI;
        return true;
    }

    double ang1 = atan2( start.y - center.y, start.x - center.x );
    double ang2 = atan2( end.y - center.y, end.x - center.x );

    // ensure that the final angle is always > start angle
    if( ang2 < ang1 )
        ang2 += 2.0 * M_PI;

    if( ang1 < 0.0 )
    {
        // range of angles will be > M_PI .. < 4*M_PI
        ang1 += 2.0 * M_PI;
        ang2 += 2.0 * M_PI;

        angles[0] = ang1;

        if( ang2 <= 2.0 * M_PI )
        {
            angles[1] = ang2;
            narcs = 1;
        }
        else
        {
            angles[1] = 2.0 * M_PI;
            angles[2] = 0.0;

            if( ang2 <= 3.0 * M_PI )
            {
                angles[3] = ang2 - 2.0 * M_PI;
                narcs = 2;
            }
            else
            {
                angles[3] = M_PI;
                angles[4] = M_PI;
                angles[5] = ang2 - 2.0 * M_PI;
                narcs = 3;
            }
        }
    }
    else
    {
        // range of angles will be >= 0 .. < 3*M_PI
        angles[0] = ang1;

        if( ang2 <= M_PI || ( ang1 >= M_PI && ang2 <= 2.0 * M_PI ) )
        {
            angles[1] = ang2;
            narcs = 1;
        }
        else
        {
            if( ang1 < M_PI )
            {
                angles[1] = M_PI;
                angles[2] = M_PI;

                if( ang2 <= 2.0 * M_PI )
                {
                    angles[3] = ang2;
                    narcs = 2;
                }
                else
                {
                    angles[3] = 2.0 * M_PI;
                    angles[4] = 0.0;
                    angles[5] = ang2 - 2.0 * M_PI;
                    narcs = 3;
                }
            }
            else
            {
                if( ang2 <= 2.0 * M_PI )
                {
                    angles[1] = ang2;
                    narcs = 1;
                }
                else
                {
                    angles[1] = 2.0 * M_PI;
                    angles[2] = 0.0;
                    angles[3] = ang2 - 2.0 * M_PI;
                    narcs = 2;
                }
            }
        }
    }

    // note: we now know how many arcs and what the angles are;
    // calculate the parameters for the IGES representation
    // of the simple arc
    arcs[1] = start;
    arcs[narcs +1] = end;

    double dx;
    double dy;

    if( narcs > 1 )
    {
        dx = radius * cos( angles[2] );
        dy = radius * sin( angles[2] );

        arcs[2].x = center.x + dx;
        arcs[2].y = center.y + dy;
        arcs[2].z = 0.0;
    }

    if( narcs > 2 )
    {
        dx = radius * cos( angles[4] );
        dy = radius * sin( angles[4] );

        arcs[3].x = center.x + dx;
        arcs[3].y = center.y + dy;
        arcs[3].z = 0.0;
    }

    return true;
}


bool IGES_GEOM_CYLINDER::Instantiate( IGES* model, double top, double bot,
    IGES_ENTITY_144**& result, int& nParts, bool aReverse )
{
    // note: we never clear 'result' as the user may be storing a list of
    // trimmed entity pointers
    nParts = 0;
    result = NULL;

    if( !narcs )
    {
        ERRMSG << "\n + [ERROR] no model data to Instantiate\n";
        return false;
    }

    if( !model )
    {
        ERRMSG << "\n + [BUG] NULL pointer passed for model\n";
        return false;;
    }

    if( abs( top - bot) < 0.001 )
    {
        ERRMSG << "\n + [BUG] top == bottom\n";
        return false;;
    }

    // swap top and bot so that the non-reversed case
    // results in a surface seen from the inside
    if( top < bot )
        swap( top, bot );

    std::vector<IGES_ENTITY_144*> parts;

    // Requirements:
    // + [4*narcs]xE110: iline, Line (axis of revolution, generatrix, and geometric bound)
    // + E120: isurf, Surface of Revolution
    // + [2x(narcs) + 2]xE126: icurve, curve segments for E102 NURBS bound
    // + [2x(narcs)]xE100: iarc, arc segments for geometric bound
    // + [2xnarcs]xE102: icc, compound curve (1 for NURBS bound, 1 for geometric bound)
    // + (narcs)xE142: ibound, Curve on surface (bounds of E120)
    // + (narcs)xE144: itps, trimmed surface
    // + (narcs)xE124: transforms required for bottom part of simple bounding curve

    #define N_ILINE 12
    #define N_ISURF 3
    #define N_ICURVE 12
    #define N_IARC 6
    #define N_ICC 6
    #define N_IBOUNDS 3
    #define N_ITPS 3
    #define N_ITRANS 3
    #define N_INURBS 12

    IGES_ENTITY_110* iline[N_ILINE];
    IGES_ENTITY_120* isurf[N_ISURF];
    IGES_ENTITY_126* icurve[N_ICURVE];
    IGES_ENTITY_100* iarc[N_IARC];
    IGES_ENTITY_102* icc[N_ICC];
    IGES_ENTITY_142* ibound[N_IBOUNDS];
    IGES_ENTITY_144* itps[N_ITPS];
    IGES_ENTITY_124* itrans[N_ITRANS];
    SISLCurve* inurbs[N_INURBS];

    for( int i = 0; i < N_ILINE; ++i )
        iline[i] = NULL;

    for( int i = 0; i < N_ISURF; ++i )
        isurf[i] = NULL;

    for( int i = 0; i < N_ICURVE; ++i )
        icurve[i] = NULL;

    for( int i = 0; i < N_IARC; ++i )
        iarc[i] = NULL;

    for( int i = 0; i < N_ICC; ++i )
        icc[i] = NULL;

    for( int i = 0; i < N_IBOUNDS; ++i )
        ibound[i] = NULL;

    for( int i = 0; i < N_ITPS; ++i )
        itps[i] = NULL;

    for( int i = 0; i < N_ITRANS; ++i )
        itrans[i] = NULL;

    for( int i = 0; i < N_INURBS; ++i )
        inurbs[i] = NULL;

#define CLEANUP do { \
    for( int i = 0; i < N_ILINE; ++i ) \
    { \
        if( iline[i] ) \
            model->DelEntity((IGES_ENTITY*)iline[i]); \
        iline[i] = NULL; \
    } \
    for( int i = 0; i < N_ISURF; ++i ) \
    { \
        if( isurf[i] ) \
            model->DelEntity((IGES_ENTITY*)isurf[i]); \
        isurf[i] = NULL; \
    } \
    for( int i = 0; i < N_ICURVE; ++i ) \
    { \
        if( icurve[i] ) \
            model->DelEntity((IGES_ENTITY*)icurve[i]); \
        icurve[i] = NULL; \
    } \
    for( int i = 0; i < N_IARC; ++i ) \
    { \
        if( iarc[i] ) \
            model->DelEntity((IGES_ENTITY*)iarc[i]); \
        iarc[i] = NULL; \
    } \
    for( int i = 0; i < N_ICC; ++i ) \
    { \
        if( icc[i] ) \
            model->DelEntity((IGES_ENTITY*)icc[i]); \
        icc[i] = NULL; \
    }\
    for( int i = 0; i < N_IBOUNDS; ++i ) \
    { \
        if( ibound[i] ) \
            model->DelEntity((IGES_ENTITY*)ibound[i]); \
            ibound[i] = NULL; \
    }\
    for( int i = 0; i < N_ITPS; ++i ) \
    { \
        if( itps[i] ) \
            model->DelEntity((IGES_ENTITY*)itps[i]); \
            itps[i] = NULL; \
    }\
    for( int i = 0; i < N_ITRANS; ++i ) \
    { \
        if( itrans[i] ) \
            model->DelEntity((IGES_ENTITY*)itrans[i]); \
            itrans[i] = NULL; \
    }\
    for( int i = 0; i < N_INURBS; ++i ) \
    { \
        if( inurbs[i] ) \
            freeCurve( inurbs[i] ); \
        inurbs[i] = NULL; \
    } } while( 0 );

    MCAD_POINT p0;
    MCAD_POINT p1;

    // create the entities
    IGES_ENTITY* ep;

    // line entities
    int nsegs = narcs * 4;

    for( int i = 0; i < nsegs; ++i )
    {
        if( !model->NewEntity( ENT_LINE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES lines\n";
            CLEANUP;
            return false;
        }

        iline[i] = dynamic_cast<IGES_ENTITY_110*>(ep);

        if( !iline[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES lines\n";
            CLEANUP;
            return false;
        }

        iline[i]->SetDependency( STAT_DEP_PHY );
    }

    // surface entity
    for( int i = 0; i < narcs; ++i )
    {
        if( !model->NewEntity( ENT_SURFACE_OF_REVOLUTION, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES surface of revolution\n";
            CLEANUP;
            return false;
        }

        isurf[i] = dynamic_cast<IGES_ENTITY_120*>(ep);

        if( !isurf[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES surface of revolution\n";
            CLEANUP;
            return false;
        }

        isurf[i]->SetDependency( STAT_DEP_PHY );
    }

    // transform entity
    for( int i = 0; i < narcs; ++i )
    {
        if( !model->NewEntity( ENT_TRANSFORMATION_MATRIX, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES transform matrix\n";
            CLEANUP;
            return false;
        }

        itrans[i] = dynamic_cast<IGES_ENTITY_124*>(ep);

        if( !itrans[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES transform matrix\n";
            CLEANUP;
            return false;
        }

        itrans[i]->T.T.x = arcs[0].x;
        itrans[i]->T.T.z = 2.0 * bot;
        itrans[i]->T.R.v[0][0] = -1.0;
        itrans[i]->T.R.v[2][2] = -1.0;
        itrans[i]->SetEntityForm( 1 );
    }

    // piecewise nurbs segments
    nsegs = narcs * 4;

    for( int i = 0; i < nsegs; ++i )
    {
        if( !model->NewEntity( ENT_NURBS_CURVE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES NURBS arc\n";
            CLEANUP;
            return false;
        }

        icurve[i] = dynamic_cast<IGES_ENTITY_126*>(ep);

        if( !icurve[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES lines\n";
            CLEANUP;
            return false;
        }

        icurve[i]->SetDependency( STAT_DEP_PHY );
    }

    // piecewise circular arc segments
    nsegs = narcs * 2;

    for( int i = 0; i < nsegs; ++i )
    {
        if( !model->NewEntity( ENT_CIRCULAR_ARC, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES circular arc\n";
            CLEANUP;
            return false;
        }

        iarc[i] = dynamic_cast<IGES_ENTITY_100*>(ep);

        if( !iarc[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES circular arcs\n";
            CLEANUP;
            return false;
        }

        iarc[i]->SetDependency( STAT_DEP_PHY );
    }

    // composite curves
    for( int i = 0; i < nsegs; ++i )
    {
        if( !model->NewEntity( ENT_COMPOSITE_CURVE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES composite curve\n";
            CLEANUP;
            return false;
        }

        icc[i] = dynamic_cast<IGES_ENTITY_102*>(ep);

        if( !icc[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES composite curve\n";
            CLEANUP;
            return false;
        }

        icc[i]->SetDependency( STAT_DEP_PHY );
    }

    // boundary (curve on surface)
    for( int i = 0; i < narcs; ++i )
    {
        if( !model->NewEntity( ENT_CURVE_ON_PARAMETRIC_SURFACE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES curve on surface\n";
            CLEANUP;
            return false;
        }

        ibound[i] = dynamic_cast<IGES_ENTITY_142*>(ep);

        if( !ibound[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES curve on surface\n";
            CLEANUP;
            return false;
        }

        ibound[i]->SetDependency( STAT_DEP_PHY );
    }

    // trimmed parametric surface
    for( int i = 0; i < narcs; ++i )
    {
        if( !model->NewEntity( ENT_TRIMMED_PARAMETRIC_SURFACE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate IGES trimmed surface\n";
            CLEANUP;
            return false;
        }

        itps[i] = dynamic_cast<IGES_ENTITY_144*>(ep);

        if( !itps[i] )
        {
            model->DelEntity( ep );
            ERRMSG << "\n + [BUG] could not typecast IGES trimmed surface\n";
            CLEANUP;
            return false;
        }
    }

    // create the axis of revolution and generatrix
    for( int i = 0; i < narcs; ++i )
    {
        int idx0 = i * 2;
        int idx1 = idx0 + 1;

        iline[idx0]->X1 = arcs[0].x;
        iline[idx0]->Y1 = arcs[0].y;
        iline[idx0]->X2 = arcs[0].x;
        iline[idx0]->Y2 = arcs[0].y;

        iline[idx1]->X1 = arcs[0].x + radius;
        iline[idx1]->Y1 = arcs[0].y;
        iline[idx1]->X2 = arcs[0].x + radius;
        iline[idx1]->Y2 = arcs[0].y;

        if( aReverse )
        {
            iline[idx0]->Z1 = top;
            iline[idx0]->Z2 = bot;
        }
        else
        {
            iline[idx0]->Z1 = bot;
            iline[idx0]->Z2 = top;
        }

        iline[idx1]->Z1 = top;
        iline[idx1]->Z2 = bot;

        if( !isurf[i]->SetAxis( (IGES_CURVE*)iline[idx0] )
            || !isurf[i]->SetGeneratrix( (IGES_CURVE*)iline[idx1] ) )
        {
            ERRMSG << "\n + [BUG] could not create surface of revolution\n";
            CLEANUP;
            return false;
        }

        isurf[i]->startAngle = 0.0;
        isurf[i]->endAngle = 2.0 * M_PI;
    }

    // create lines for geometric bounds
    // [bounds = ccw top arc + line->bot + cw bot  arc + line->top]
    int ibase = narcs * 2;
    iline[ibase]->X1 = arcs[2].x;
    iline[ibase]->Y1 = arcs[2].y;
    iline[ibase]->Z1 = top;
    iline[ibase]->X2 = arcs[2].x;
    iline[ibase]->Y2 = arcs[2].y;
    iline[ibase]->Z2 = bot;

    iline[ibase + 1]->X1 = arcs[1].x;
    iline[ibase + 1]->Y1 = arcs[1].y;
    iline[ibase + 1]->Z1 = bot;
    iline[ibase + 1]->X2 = arcs[1].x;
    iline[ibase + 1]->Y2 = arcs[1].y;
    iline[ibase + 1]->Z2 = top;

    if( narcs > 1 )
    {
        iline[ibase + 2]->X1 = arcs[3].x;
        iline[ibase + 2]->Y1 = arcs[3].y;
        iline[ibase + 2]->Z1 = top;
        iline[ibase + 2]->X2 = arcs[3].x;
        iline[ibase + 2]->Y2 = arcs[3].y;
        iline[ibase + 2]->Z2 = bot;

        iline[ibase + 3]->X1 = arcs[2].x;
        iline[ibase + 3]->Y1 = arcs[2].y;
        iline[ibase + 3]->Z1 = bot;
        iline[ibase + 3]->X2 = arcs[2].x;
        iline[ibase + 3]->Y2 = arcs[2].y;
        iline[ibase + 3]->Z2 = top;
    }

    if( narcs > 2 )
    {
        iline[ibase + 4]->X1 = arcs[4].x;
        iline[ibase + 4]->Y1 = arcs[4].y;
        iline[ibase + 4]->Z1 = top;
        iline[ibase + 4]->X2 = arcs[4].x;
        iline[ibase + 4]->Y2 = arcs[4].y;
        iline[ibase + 4]->Z2 = bot;

        iline[ibase + 5]->X1 = arcs[3].x;
        iline[ibase + 5]->Y1 = arcs[3].y;
        iline[ibase + 5]->Z1 = bot;
        iline[ibase + 5]->X2 = arcs[3].x;
        iline[ibase + 5]->Y2 = arcs[3].y;
        iline[ibase + 5]->Z2 = top;
    }

    // arcs for geometric bound
    iarc[0]->zOffset = top;
    iarc[0]->xCenter = arcs[0].x;
    iarc[0]->yCenter = arcs[0].y;
    iarc[0]->xStart = arcs[1].x;
    iarc[0]->yStart = arcs[1].y;
    iarc[0]->xEnd = arcs[2].x;
    iarc[0]->yEnd = arcs[2].y;

    iarc[1]->zOffset = bot;
    iarc[1]->xCenter = 0.0;
    iarc[1]->yCenter = arcs[0].y;
    iarc[1]->xStart = arcs[0].x - arcs[2].x;
    iarc[1]->yStart = arcs[2].y;
    iarc[1]->xEnd = arcs[0].x - arcs[1].x;
    iarc[1]->yEnd = arcs[1].y;
    iarc[1]->SetTransform( itrans[0] );

    if( narcs > 1 )
    {
        iarc[2]->zOffset = top;
        iarc[2]->xCenter = arcs[0].x;
        iarc[2]->yCenter = arcs[0].y;
        iarc[2]->xStart = arcs[2].x;
        iarc[2]->yStart = arcs[2].y;
        iarc[2]->xEnd = arcs[3].x;
        iarc[2]->yEnd = arcs[3].y;

        iarc[3]->zOffset = bot;
        iarc[3]->xCenter = 0.0;
        iarc[3]->yCenter = arcs[0].y;
        iarc[3]->xStart = arcs[0].x - arcs[3].x;
        iarc[3]->yStart = arcs[3].y;
        iarc[3]->xEnd = arcs[0].x - arcs[2].x;
        iarc[3]->yEnd = arcs[2].y;
        iarc[3]->SetTransform( itrans[1] );
    }

    if( narcs > 2 )
    {
        iarc[4]->zOffset = top;
        iarc[4]->xCenter = arcs[0].x;
        iarc[4]->yCenter = arcs[0].y;
        iarc[4]->xStart = arcs[3].x;
        iarc[4]->yStart = arcs[3].y;
        iarc[4]->xEnd = arcs[4].x;
        iarc[4]->yEnd = arcs[4].y;

        iarc[5]->zOffset = bot;
        iarc[5]->xCenter = 0.0;
        iarc[5]->yCenter = arcs[0].y;
        iarc[5]->xStart = arcs[0].x - arcs[4].x;
        iarc[5]->yStart = arcs[4].y;
        iarc[5]->xEnd = arcs[0].x - arcs[3].x;
        iarc[5]->yEnd = arcs[3].y;
        iarc[5]->SetTransform( itrans[2] );
    }

    // compound curve for geometric bound
    if( !icc[narcs]->AddSegment( iarc[0] )
        || !icc[narcs]->AddSegment( iline[ibase] )
        || !icc[narcs]->AddSegment( iarc[1] )
        || !icc[narcs]->AddSegment( iline[ibase + 1] ) )
    {
        ERRMSG << "\n + [BUG] could not create geometric bound #1\n";
        CLEANUP;
        return false;
    }

    if( narcs > 1 )
    {
        if( !icc[narcs + 1]->AddSegment( iarc[2] )
            || !icc[narcs + 1]->AddSegment( iline[ibase + 2] )
            || !icc[narcs + 1]->AddSegment( iarc[3] )
            || !icc[narcs + 1]->AddSegment( iline[ibase + 3] ) )
        {
            ERRMSG << "\n + [BUG] could not create geometric bound #2\n";
            CLEANUP;
            return false;
        }
    }

    if( narcs > 2 )
    {
        if( !icc[narcs + 2]->AddSegment( iarc[4] )
            || !icc[narcs + 2]->AddSegment( iline[ibase + 4] )
            || !icc[narcs + 2]->AddSegment( iarc[5] )
            || !icc[narcs + 2]->AddSegment( iline[ibase + 5] ) )
        {
            ERRMSG << "\n + [BUG] could not create geometric bound #3\n";
            CLEANUP;
            return false;
        }
    }

    // at this stage we have the geometric bounds; now we must
    // calculate the NURBS bounds; these are all linear bounds:
    // (0, startAng, 0) .. (0, endAng, 0)
    // (0, endAng, 0) .. (1, endAng, 0)
    // (1, endAng, 0) .. (1, startAng, 0)
    // (1, startAng, 0) .. (0, startAng, 0)
    for( int i = 0; i < narcs; ++i )
    {
        int idx = i * 4;
        int idx2 = i * 2;
        double data[6]; // 2 control points for inurbs

        // (0, startAng, 0) .. (0, endAng, 0)
        data[0] = 0.0;
        data[1] = angles[idx2];
        data[2] = 0.0;
        data[3] = 0.0;
        data[4] = angles[idx2 + 1];
        data[5] = 0.0;

        if( aReverse )
        {
            data[1] = 2.0 * M_PI - data[1];
            data[4] = 2.0 * M_PI - data[4];
        }

        if( !makeNurb( data, &data[3], &inurbs[idx] ) )
        {
            ERRMSG << "\n + [BUG] could not create NURBS bound #" << i << ".1\n";
            CLEANUP;
            return false;
        }

        // (0, endAng, 0) .. (1, endAng, 0)
        data[0] = 0.0;
        data[1] = data[4];
        data[2] = 0.0;
        data[3] = 1.0;
        data[5] = 0.0;

        if( !makeNurb( data, &data[3], &inurbs[idx +1] ) )
        {
            ERRMSG << "\n + [BUG] could not create NURBS bound #" << i << ".2\n";
            CLEANUP;
            return false;
        }

        // (1, endAng, 0) .. (1, startAng, 0)
        data[0] = 1.0;
        data[2] = 0.0;
        data[3] = 1.0;
        data[4] = angles[idx2];
        data[5] = 0.0;

        if( aReverse )
            data[4] = 2.0 * M_PI - data[4];

        if( !makeNurb( data, &data[3], &inurbs[idx +2] ) )
        {
            ERRMSG << "\n + [BUG] could not create NURBS bound #" << i << ".3\n";
            CLEANUP;
            return false;
        }

        // (1, startAng, 0) .. (0, startAng, 0)
        data[0] = 1.0;
        data[1] = data[4];
        data[2] = 0.0;
        data[3] = 0.0;
        data[5] = 0.0;

        if( !makeNurb( data, &data[3], &inurbs[idx +3] ) )
        {
            ERRMSG << "\n + [BUG] could not create NURBS bound #" << i << ".4\n";
            CLEANUP;
            return false;
        }
    }

    nsegs = narcs * 4;

    for( int i = 0; i < nsegs; ++i )
    {
        int iKE = inurbs[i]->in + inurbs[i]->ik - 1;

        if( !icurve[i]->SetNURBSData( inurbs[i]->in, inurbs[i]->ik, inurbs[i]->et,
            inurbs[i]->ecoef, false, inurbs[i]->et[0], inurbs[i]->et[iKE] ) )
        {
            ERRMSG << "\n + [BUG] could not transfer bounds data to NURBS #" << i << "\n";
            CLEANUP;
            return false;
        }
    }

    // compound curves for NURBS bound
    for( int i = 0; i < narcs; ++i )
    {
        int idx = i * 4;
        if( !icc[i]->AddSegment( (IGES_CURVE*)icurve[idx++] )
            || !icc[i]->AddSegment( (IGES_CURVE*)icurve[idx++] )
            || !icc[i]->AddSegment( (IGES_CURVE*)icurve[idx++] )
            || !icc[i]->AddSegment( (IGES_CURVE*)icurve[idx] ) )
        {
            ERRMSG << "\n + [BUG] could not create geometric bound #" << i << "\n";
            CLEANUP;
            return false;
        }
    }

    for( int i = 0; i < narcs; ++i )
    {
        ibound[i]->CRTN = 1;
        ibound[i]->PREF = 1;

        if( !ibound[i]->SetSPTR( (IGES_ENTITY*)isurf[i] )
            || !ibound[i]->SetBPTR( (IGES_ENTITY*)icc[i] )
            || !ibound[i]->SetCPTR( (IGES_ENTITY*)icc[i + narcs] ) )
        {
            ERRMSG << "\n + [BUG] could not create curve on surface #" << i << "\n";
            CLEANUP;
            return false;
        }
    }

    for( int i = 0; i < narcs; ++i )
    {
        itps[i]->N1 = 1;
        itps[i]->N2 = 0;

        if( !itps[i]->SetPTS( (IGES_ENTITY*)isurf[i] )
            || !itps[i]->SetPTO( ibound[i] ) )
        {
            ERRMSG << "\n + [BUG] could not create trimmed surface #" << i << "\n";
            CLEANUP;
            return false;
        }
    }

    for( int i = 0; i < narcs; ++i )
        parts.push_back( itps[i] );

    // clean up on success
    do
    {
        for( int i = 0; i < N_ILINE; ++i )
            iline[i] = NULL;

        for( int i = 0; i < N_ISURF; ++i )
            isurf[i] = NULL;

        for( int i = 0; i < N_ICURVE; ++i )
            icurve[i] = NULL;

        for( int i = 0; i < N_IARC; ++i )
            iarc[i] = NULL;

        for( int i = 0; i < N_ICC; ++i )
            icc[i] = NULL;

        for( int i = 0; i < N_IBOUNDS; ++i )
            ibound[i] = NULL;

        for( int i = 0; i < N_ITPS; ++i )
            itps[i] = NULL;

        for( int i = 0; i < N_ITRANS; ++i )
            itrans[i] = NULL;

        for( int i = 0; i < N_INURBS; ++i )
        {
            if( inurbs[i] )
                freeCurve( inurbs[i] );

            inurbs[i] = NULL;
        }

    } while( 0 );

    nParts = (int)parts.size();
    result = new IGES_ENTITY_144*[nParts];

    for( int i = 0; i < nParts; ++i )
        result[i] = parts[i];

    return true;
}
