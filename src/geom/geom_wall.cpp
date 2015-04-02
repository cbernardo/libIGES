/*
 * file: geom_wall.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: object to aid in the creation of a rectangular
 * surface within IGES
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
#include <geom_wall.h>


IGES_GEOM_WALL::IGES_GEOM_WALL()
{
    init();
    return;
}


IGES_GEOM_WALL::~IGES_GEOM_WALL()
{
    clear();
    return;
}


void IGES_GEOM_WALL::init( void )
{
    plane = NULL;
    side[0] = NULL;
    side[1] = NULL;
    side[2] = NULL;
    side[3] = NULL;

    return;
}


void IGES_GEOM_WALL::clear( void )
{
    if( plane )
    {
        freeSurf( plane );
        plane = NULL;
    }

    if( side[0] )
    {
        freeCurve( side[0] );
        side[0] = NULL;
    }

    if( side[1] )
    {
        freeCurve( side[1] );
        side[1] = NULL;
    }

    if( side[2] )
    {
        freeCurve( side[2] );
        side[2] = NULL;
    }

    if( side[3] )
    {
        freeCurve( side[3] );
        side[3] = NULL;
    }

    return;
}


bool IGES_GEOM_WALL::SetParams( IGES_POINT p0, IGES_POINT p1, IGES_POINT p2, IGES_POINT p3 )
{
    clear();
    double data[12];

    data[0] = p0.x;
    data[1] = p0.y;
    data[2] = p0.z;

    data[3] = p1.x;
    data[4] = p1.y;
    data[5] = p1.z;

    data[6] = p3.x;
    data[7] = p3.y;
    data[8] = p3.z;

    data[9]  = p2.x;
    data[10] = p2.y;
    data[11] = p2.z;

    vertex[0] = p0;
    vertex[1] = p1;
    vertex[2] = p2;
    vertex[3] = p3;

    int stat = 0;

    // create the NURBS representation of the surface
    s1536( data, 2, 2, 3, 2, 0, 0, 0, 0, 2, 2, 1, 1, &plane, &stat );

    switch( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified problems creating NURBS plane\n";
            stat = 0;
            break;

        default:
            ERRMSG << "\n + [ERROR] could not create NURBS plane\n";
            clear();
            return false;
            break;
    }

    // create the NURBS representation of the sides as projected on the surface
    for( int i = 0; i < 4; ++i )
    {
        double startp[3];
        double endp[3];
        SISLCurve** pCurve;

        switch( i )
        {
            case 0:
                startp[0] = 0.0;
                startp[1] = 0.0;
                startp[2] = 0.0;
                endp[0]   = 1.0;
                endp[1]   = 0.0;
                endp[2]   = 0.0;
                break;

            case 1:
                startp[0] = 1.0;
                startp[1] = 0.0;
                startp[2] = 0.0;
                endp[0]   = 1.0;
                endp[1]   = 1.0;
                endp[2]   = 0.0;
                break;

            case 2:
                startp[0] = 1.0;
                startp[1] = 1.0;
                startp[2] = 0.0;
                endp[0]   = 0.0;
                endp[1]   = 1.0;
                endp[2]   = 0.0;
                break;

            default:
                startp[0] = 0.0;
                startp[1] = 1.0;
                startp[2] = 0.0;
                endp[0]   = 0.0;
                endp[1]   = 0.0;
                endp[2]   = 0.0;
                break;
        }

        pCurve = &side[i];
        double epar;
        s1602( startp, endp, 2, 3, 0.0, &epar, pCurve, &stat );

        switch( stat )
        {
            case 0:
                break;

            case 1:
                ERRMSG << "\n + [WARNING] unspecified problems creating NURBS plane\n";
                stat = 0;
                break;

            default:
                ERRMSG << "\n + [ERROR] could not create NURBS plane\n";
                clear();
                return false;
                break;
        }

    }   // loop to create curve on surface (sides[])

    return true;
}


IGES_ENTITY_144* IGES_GEOM_WALL::Instantiate( IGES* model )
{
    if( !model )
    {
        ERRMSG << "\n + [ERROR] NULL passed for IGES object\n";
        return NULL;
    }

    if( !plane || !side[0] || !side[1]
        || !side[2] || !side[3] )
    {
        ERRMSG << "\n + [ERROR] no surface data to instantiate\n";
        return NULL;
    }

    // Requirements:
    // Entity128: isurf, NURBS surface
    // Entity126(x4): ibound[], NURBS parametric lines (sides)
    // Entity110(x4): iline[], simple line representing sides
    // Entity102(x2): icc[], compound curves
    // Entity142: icurve, Curve on surface
    // Entity144: itps, Trimmed parametric surface

    IGES_ENTITY* ep;
    IGES_ENTITY_128* isurf;

    if( !model->NewEntity( ENT_NURBS_SURFACE, &ep ) )
    {
        ERRMSG << "\n + [INFO] could not instantiate new entity\n";
        return NULL;
    }

    isurf = dynamic_cast<IGES_ENTITY_128*>(ep);

    if( NULL == isurf )
    {
        ERRMSG << "\n + [BUG] cast failed on surface entity\n";
        model->DelEntity( ep );
        return NULL;
    }

    IGES_ENTITY_126* ibound[4];

    for( int i = 0; i < 4; ++i )
    {
        if( !model->NewEntity( ENT_NURBS_CURVE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate new entity\n";

            for( int j = 0; j < i; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            return NULL;
        }

        ibound[i] = dynamic_cast<IGES_ENTITY_126*>(ep);

        if( NULL == ibound[i] )
        {
            ERRMSG << "\n + [BUG] cast failed on NURBS curve entity\n";
            model->DelEntity( ep );

            for( int j = 0; j < i; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            return NULL;
        }
    }

    IGES_ENTITY_110* iline[4];

    for( int i = 0; i < 4; ++i )
    {
        if( !model->NewEntity( ENT_LINE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate new entity\n";

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < i; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            return NULL;
        }

        iline[i] = dynamic_cast<IGES_ENTITY_110*>(ep);

        if( NULL == iline[i] )
        {
            ERRMSG << "\n + [BUG] cast failed on line entity\n";
            model->DelEntity( ep );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < i; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            return NULL;
        }
    }

    IGES_ENTITY_102* icc[2];

    for( int i = 0; i < 2; ++i )
    {
        if( !model->NewEntity( ENT_COMPOSITE_CURVE, &ep ) )
        {
            ERRMSG << "\n + [INFO] could not instantiate new entity\n";

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            for( int j = 0; j < i; ++j )
                model->DelEntity( (IGES_ENTITY*)icc[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            return NULL;
        }

        icc[i] = dynamic_cast<IGES_ENTITY_102*>(ep);

        if( NULL == icc[i] )
        {
            ERRMSG << "\n + [BUG] cast failed on line entity\n";
            model->DelEntity( ep );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            for( int j = 0; j < i; ++j )
                model->DelEntity( (IGES_ENTITY*)icc[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            return NULL;
        }
    }

    IGES_ENTITY_142* icurve;

    if( !model->NewEntity( ENT_CURVE_ON_PARAMETRIC_SURFACE, &ep ) )
    {
        ERRMSG << "\n + [INFO] could not instantiate new entity\n";

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        return NULL;
    }

    icurve = dynamic_cast<IGES_ENTITY_142*>(ep);

    if( NULL == icurve )
    {
        ERRMSG << "\n + [BUG] cast failed on curve on surface entity\n";
        model->DelEntity( ep );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        return NULL;
    }

    IGES_ENTITY_144* itps;

    if( !model->NewEntity( ENT_TRIMMED_PARAMETRIC_SURFACE, &ep ) )
    {
        ERRMSG << "\n + [INFO] could not instantiate new entity\n";

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        model->DelEntity( (IGES_ENTITY*)icurve );
        return NULL;
    }

    itps = dynamic_cast<IGES_ENTITY_144*>(ep);

    if( NULL == itps )
    {
        ERRMSG << "\n + [BUG] cast failed on curve on trimmed surface entity\n";
        model->DelEntity( ep );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        model->DelEntity( (IGES_ENTITY*)icurve );
        return NULL;
    }

    // copy the NURBS surface data to isurf
    if( !isurf->SetNURBSData( plane->in1, plane->in2, plane->ik1, plane->ik2,
        plane->et1, plane->et2, plane->ecoef, false, false, false ) )
    {
        ERRMSG << "\n + [BUG] failed to transfer data to surface entity\n";

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        model->DelEntity( (IGES_ENTITY*)icurve );
        model->DelEntity( (IGES_ENTITY*)itps );
        return NULL;
    }

    // copy side[n] to ibound[n]
    for( int i = 0; i < 4; ++i )
    {
        if( !ibound[i]->SetNURBSData( side[i]->in, side[i]->ik, side[i]->et,
            side[i]->ecoef, false ) )
        {
            ERRMSG << "\n + [BUG] failed to transfer data to surface entity\n";

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            for( int j = 0; j < 2; ++j )
                model->DelEntity( (IGES_ENTITY*)icc[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            model->DelEntity( (IGES_ENTITY*)icurve );
            model->DelEntity( (IGES_ENTITY*)itps );
            return NULL;
        }
    }

    // set line data in iline[n]
    do
    {
        iline[0]->X1 = vertex[0].x;
        iline[0]->Y1 = vertex[0].y;
        iline[0]->Z1 = vertex[0].z;
        iline[0]->X2 = vertex[1].x;
        iline[0]->Y2 = vertex[1].y;
        iline[0]->Z2 = vertex[1].z;

        iline[1]->X1 = vertex[1].x;
        iline[1]->Y1 = vertex[1].y;
        iline[1]->Z1 = vertex[1].z;
        iline[1]->X2 = vertex[2].x;
        iline[1]->Y2 = vertex[2].y;
        iline[1]->Z2 = vertex[2].z;

        iline[2]->X1 = vertex[2].x;
        iline[2]->Y1 = vertex[2].y;
        iline[2]->Z1 = vertex[2].z;
        iline[2]->X2 = vertex[3].x;
        iline[2]->Y2 = vertex[3].y;
        iline[2]->Z2 = vertex[3].z;

        iline[3]->X1 = vertex[3].x;
        iline[3]->Y1 = vertex[3].y;
        iline[3]->Z1 = vertex[3].z;
        iline[3]->X2 = vertex[0].x;
        iline[3]->Y2 = vertex[0].y;
        iline[3]->Z2 = vertex[0].z;
    } while( 0 );

    // put ibound[n] into icc[0]
    for( int i = 0; i < 4; ++i )
    {
        if( !icc[0]->AddSegment( dynamic_cast<IGES_CURVE*>(ibound[i]) ) )
        {
            ERRMSG << "\n + [BUG] failed to transfer data to complex curve[0]\n";

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            for( int j = 0; j < 2; ++j )
                model->DelEntity( (IGES_ENTITY*)icc[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            model->DelEntity( (IGES_ENTITY*)icurve );
            model->DelEntity( (IGES_ENTITY*)itps );
            return NULL;
        }
    }

    // put iline[n] into icc[1]
    for( int i = 0; i < 4; ++i )
    {
        if( !icc[1]->AddSegment( dynamic_cast<IGES_CURVE*>(iline[i]) ) )
        {
            ERRMSG << "\n + [BUG] failed to transfer data to complex curve[1]\n";

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)ibound[j] );

            for( int j = 0; j < 4; ++j )
                model->DelEntity( (IGES_ENTITY*)iline[j] );

            for( int j = 0; j < 2; ++j )
                model->DelEntity( (IGES_ENTITY*)icc[j] );

            model->DelEntity( (IGES_ENTITY*)isurf );
            model->DelEntity( (IGES_ENTITY*)icurve );
            model->DelEntity( (IGES_ENTITY*)itps );
            return NULL;
        }
    }

    // put isurf, icc into icurve
    icurve->CRTN = 1;
    icurve->PREF = 1;

    if( !icurve->SetSPTR( (IGES_ENTITY*)isurf )
        || !icurve->SetBPTR( (IGES_ENTITY*)icc[0] )
        || !icurve->SetCPTR( (IGES_ENTITY*)icc[1] ) )
    {
        ERRMSG << "\n + [BUG] failed to transfer data to parametric curve on surface\n";

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        model->DelEntity( (IGES_ENTITY*)icurve );
        model->DelEntity( (IGES_ENTITY*)itps );
        return NULL;
    }

    // put isurf, icurve into itps
    itps->N1 = 0;   // bound is the same as the limits of the surface
    itps->N2 = 0;

    if( !itps->SetPTS( (IGES_ENTITY*)isurf )
        || !itps->SetPTO( icurve ) )
    {
        ERRMSG << "\n + [BUG] failed to transfer data to parametric curve on surface\n";

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)ibound[j] );

        for( int j = 0; j < 4; ++j )
            model->DelEntity( (IGES_ENTITY*)iline[j] );

        for( int j = 0; j < 2; ++j )
            model->DelEntity( (IGES_ENTITY*)icc[j] );

        model->DelEntity( (IGES_ENTITY*)isurf );
        model->DelEntity( (IGES_ENTITY*)icurve );
        model->DelEntity( (IGES_ENTITY*)itps );
        return NULL;
    }

    // XXX - TO BE IMPLEMENTED
    // +* copy surface to isurf
    // +* copy side[n] to ibound[n]
    // +* set line data in iline[n]
    // +* put ibound[n] into icc[0]
    // +* put iline[n] into icc[1]
    // +* put isurf, icc into icurve
    // +* put isurf, icurve into itps

    return itps;
}
