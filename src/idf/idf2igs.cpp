/*
 * file: idf2igs.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 100: Circle, Section 4.3, p.66+ (94+)
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
 *  This program takes an IDF base name, loads the board outline
 *  and component outine files, and creates an IGES assembly file.
 */


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <list>
#include <utility>
#include <clocale>
#include <vector>
#include <boost/filesystem.hpp>

#include <idf_helpers.h>
#include <idf_common.h>
#include <idf_parser.h>

#include <error_macros.h>
#include <iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <mcad_segment.h>
#include <iges_geom_pcb.h>


using namespace std;

static struct
{
    string basename;
    IGES_ENTITY_314** colors;
} globs;

// colors to be used in the output assembly model
#define NCOLORS 9
bool initColors( IGES& model, IGES_ENTITY_314** colors );

// convert IDF outline to IGS outline
bool convertOln( MCAD_OUTLINE* olnIGS, IDF_OUTLINE* olnIDF );
bool convertDrills( list< MCAD_SEGMENT* >& drills, const list<IDF_DRILL_DATA*>* dh );
// merge overlapping drills into cutouts; return true if any drills were merged;
// if invalid geometry was encountered the error flag will be set
bool mergeDrills( list< MCAD_SEGMENT* >& drills, list< MCAD_OUTLINE* >& cutouts, bool& error );
// take given drill list and punch a cutout using nearest holes in succession
bool bundleDrills( list< MCAD_SEGMENT* >* drills, list< MCAD_OUTLINE* >& cutouts );
// delete drill data
void killDrills( list< MCAD_SEGMENT* >& drills );
// delete cutout data
void killCutouts( list< MCAD_OUTLINE* >& cutouts );

bool MakeBoard( IDF3_BOARD& board, IGES& model );
bool MakeComponents( IDF3_BOARD& board, IGES& model );
bool MakeOtherOutlines( IDF3_BOARD& board, IGES& model );
// build a component part model from the given outline data
bool buildComponent( IGES& model, const IDF3_COMP_OUTLINE* idf, IGES_ENTITY_308** subfig );

// routines to make IGES model creation easier
bool newSubfigure( IGES& model, IGES_ENTITY_308** aNewSubfig );
IGES_ENTITY_124* calcTransform( IGES& model, double dX, double dY, double dZ, double dA, bool bottom );

// retrieve an index to the next color in the sequence
int GetComponentColor( void );


void PrintUsage( void )
{
    cout << "-\nUsage: idfigs input_file.emn\n";
    return;
}


int main( int argc, char **argv )
{
    if( argc != 2 )
    {
        PrintUsage();
        return -1;
    }

    // IDF implicitly requires the C locale
    setlocale( LC_ALL, "C" );

    // Essential inputs:
    // 1. IDF file

    std::string inputFilename = argv[1];

    if( inputFilename.empty() )
    {
        cerr << "* no IDF filename supplied\n";
        PrintUsage();
        return -1;
    }

    IDF3_BOARD pcb( IDF3::CAD_ELEC );

    cout << "** Reading file: " << inputFilename << "\n";

    if( !pcb.ReadFile( inputFilename, true ) )
    {
        cerr << "** Failed to read IDF data:\n";
        cerr << pcb.GetError() << "\n\n";

        return -1;
    }

    // restore the locale
    setlocale( LC_ALL, "" );

    // create an IGES model and set its parameters
    IGES model;
    model.globalData.productIDSS = "idf2igs test";
    model.globalData.nativeSystemID = "libIGES";
    model.globalData.modelScale = 1.0;
    model.globalData.unitsFlag = UNIT_MM;
    model.globalData.minResolution = 1e-8;

    // create the color schemes:
    IGES_ENTITY_314* colors[NCOLORS];

    if( !initColors( model, colors ) )
    {
        cerr << "** Failed to create IGES color entities\n";
        return -1;
    }

    globs.colors = colors;

    // Create the VRML file and write the header
    boost::filesystem::path bofname( inputFilename );
    bofname.replace_extension( "igs" );
    string fname = bofname.filename().string();
    globs.basename = boost::filesystem::basename( bofname );

    cout << "Output file: '" << fname << "'\n";

    // STEP 1: Render the PCB alone
    MakeBoard( pcb, model );

    // STEP 2: Render the components
    MakeComponents( pcb, model );

    // STEP 3: Render the OTHER outlines
    MakeOtherOutlines( pcb, model );

    model.Write( fname.c_str(), true );

    return 0;
}



bool MakeBoard( IDF3_BOARD& board, IGES& model )
{
    if( board.GetBoardOutlinesSize() < 1 )
    {
        ERROR_IDF << "\n";
        cerr << "* Cannot proceed; no board outline in IDF object\n";
        return false;
    }

    IGES_GEOM_PCB otln; // main board outline
    MCAD_OUTLINE* pIGS;
    list< MCAD_OUTLINE* > cutouts;
    list< MCAD_SEGMENT* > drills;

    list< IDF_OUTLINE* >::const_iterator scont = board.GetBoardOutline()->GetOutlines()->begin();
    list< IDF_OUTLINE* >::const_iterator econt = board.GetBoardOutline()->GetOutlines()->end();
    list< IDF_OUTLINE* >::const_iterator rcont = scont;

    // extract the board outline and cutouts
    while( scont != econt )
    {
        if( scont == rcont )
            pIGS = &otln;
        else
            pIGS = new MCAD_OUTLINE;

        if( !convertOln( pIGS, *scont ) )
        {
            if( scont != rcont )
                delete pIGS;

            return false;
        }

        if( scont != rcont )
            cutouts.push_back( pIGS );

        ++scont;
    }

    // extract the board drill holes
    convertDrills( drills, &board.GetBoardDrills() );

    // extract component drill holes
    map< string, IDF3_COMPONENT* >*const comp = board.GetComponents();
    map< string, IDF3_COMPONENT* >::const_iterator sc = comp->begin();
    map< string, IDF3_COMPONENT* >::const_iterator ec = comp->end();

    while( sc != ec )
    {
        convertDrills( drills, sc->second->GetDrills() );
        ++sc;
    }

    // note: the peculiar strategy used here for adding drill holes and
    // cutouts ensures that we can support overlapping drill holes
    // and drill holes on the PCB edges while minimizing the chances of
    // creating invalid geometry conditions

    // check drill list for overlapping drills and convert all overlapping drills to cutouts
    bool dud = false;
    mergeDrills( drills, cutouts, dud );

    if( dud )
    {
        ERROR_IDF << "\n + fatal error encountered while attempting to merge drill holes\n";
        killDrills( drills );
        killCutouts( cutouts );
        return false;
    }

    // subtract drill holes from PCB edge
    list< MCAD_SEGMENT* >::iterator sDH;
    list< MCAD_SEGMENT* >::iterator eDH;

    sDH = drills.begin();
    eDH = drills.end();

    while( sDH != eDH )
    {
        if( otln.SubOutline( *sDH , dud ) )
        {
            sDH = drills.erase( sDH );
            continue;
        }

        if( dud )
        {
            ERROR_IDF << "\n + fatal error encountered while attempting to add drill hole to main outline\n";
            killDrills( drills );
            killCutouts( cutouts );
            return false;
        }

        ++sDH;
    }

    // add drill holes to outline edges
    list< MCAD_OUTLINE* >::iterator sMO = cutouts.begin();
    list< MCAD_OUTLINE* >::iterator eMO = cutouts.end();

    while( sMO != eMO )
    {
        sDH = drills.begin();
        eDH = drills.end();

        while( sDH != eDH )
        {
            if( (*sMO)->AddOutline( *sDH , dud ) )
            {
                sDH = drills.erase( sDH );
                continue;
            }

            if( dud )
            {
                ERROR_IDF << "\n + fatal error encountered while attempting to add drill hole to cutout\n";
                killDrills( drills );
                killCutouts( cutouts );
                return false;
            }

            ++sDH;
        }

        ++sMO;
    }

    // add all cutouts and remaining drills to the main outline
    sMO = cutouts.begin();
    eMO = cutouts.end();

    while( sMO != eMO )
    {
        otln.AddCutout( *sMO, true, dud );
        ++sMO;
    }

    sDH = drills.begin();
    eDH = drills.end();

    while( sDH != eDH )
    {
        otln.AddCutout( *sDH, false, dud );
        ++sDH;
    }

    // put in part and solid instance, names, and color
    // create the PCB model
    vector<IGES_ENTITY_144*> surfs;
    double th = 0.5 * board.GetBoardThickness();
    otln.GetVerticalSurface( &model, dud, surfs, th, -th );
    otln.GetTrimmedPlane( &model, dud, surfs, th );
    otln.GetTrimmedPlane( &model, dud, surfs, -th );

    IGES_ENTITY_308* subfig;

    if( !newSubfigure( model, &subfig ) )
    {
        ERROR_IDF << "\n + could not create a subfigure entity\n";
        return false;
    }

    vector<IGES_ENTITY_144*>::iterator sSL = surfs.begin();
    vector<IGES_ENTITY_144*>::iterator eSL = surfs.end();

    while( sSL != eSL )
    {
        (*sSL)->SetColor( (IGES_ENTITY*) globs.colors[0] );
        subfig->AddDE( (IGES_ENTITY*)(*sSL) );
        ++sSL;
    }

    // add the name
    subfig->NAME = globs.basename;

    IGES_ENTITY* ep;
    IGES_ENTITY_408* p408;
    model.NewEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, &ep );
    p408 = (IGES_ENTITY_408*)ep;
    p408->SetDE( subfig );
    p408->SetLabel( globs.basename );

    return true;
}


// convert IDF outline to IGS outline
bool convertOln( MCAD_OUTLINE* olnIGS, IDF_OUTLINE* olnIDF )
{
    if( ((IDF_OUTLINE*)olnIDF)->size() < 1 )
    {
        ERROR_IDF << "invalid contour: no vertices\n";
        return false;
    }

    std::list<IDF_SEGMENT*>::iterator sseg;
    std::list<IDF_SEGMENT*>::iterator eseg;
    IDF_SEGMENT* ps;
    MCAD_SEGMENT* pm;
    MCAD_POINT   pts[3];
    sseg = olnIDF->begin();
    eseg = olnIDF->end();
    bool dud = false;

    while( sseg != eseg )
    {
        // convert IDF to IGES
        ps = *sseg;
        pm = new MCAD_SEGMENT;

        if( ps->IsCircle() )
        {
            bool SetParams( MCAD_POINT aCenter, MCAD_POINT aStart, MCAD_POINT aEnd, bool isCW );
            pts[0].x = ps->center.x;
            pts[0].y = ps->center.y;
            pts[0].z = 0;

            pts[1].x = pts[0].x + ps->radius;
            pts[1].y = pts[0].y;
            pts[1].z = 0;

            pm->SetParams( pts[0], pts[1], pts[1], false );
        }
        else if( ps->radius > 0.0 )
        {
            pts[0].x = ps->center.x;
            pts[0].y = ps->center.y;
            pts[0].z = 0;

            pts[1].x = ps->startPoint.x;
            pts[1].y = ps->startPoint.y;
            pts[1].z = 0;

            pts[2].x = ps->endPoint.x;
            pts[2].y = ps->endPoint.y;
            pts[2].z = 0;

            if( ps->angle > 0.0 )
                pm->SetParams( pts[0], pts[1], pts[2], false );
            else
                pm->SetParams( pts[0], pts[1], pts[2], true );

        }
        else
        {
            pts[0].x = ps->startPoint.x;
            pts[0].y = ps->startPoint.y;
            pts[0].z = 0;

            pts[1].x = ps->endPoint.x;
            pts[1].y = ps->endPoint.y;
            pts[1].z = 0;

            pm->SetParams( pts[0], pts[1] );
        }

        if( !olnIGS->AddSegment( pm, dud ) || dud )
        {
            delete pm;
            ERROR_IDF << "could not add segment to outline\n";
            return false;
        }

        ++sseg;
    }

    return true;
}


bool convertDrills( list< MCAD_SEGMENT* >& drills, const list<IDF_DRILL_DATA*>* dh )
{
    // XXX - for brevity, error checking has been left out

    list<IDF_DRILL_DATA*>::const_iterator sD = dh->begin();
    list<IDF_DRILL_DATA*>::const_iterator eD = dh->end();
    MCAD_SEGMENT* sp;
    MCAD_POINT p[2];

    while( sD != eD )
    {
        sp = new MCAD_SEGMENT;

        p[0].x = (*sD)->GetDrillXPos();
        p[0].y = (*sD)->GetDrillYPos();
        p[0].z = 0.0;

        p[1].x = p[0].x + 0.5 * (*sD)->GetDrillDia();
        p[1].y = p[0].y;
        p[1].z = 0.0;

        sp->SetParams( p[0], p[1], p[1], false );
        drills.push_back( sp );

        ++sD;
    }

    return true;
}

// merge overlapping drills into cutouts; return true if any drills were merged;
// if invalid geometry was encountered the error flag will be set
bool mergeDrills( list< MCAD_SEGMENT* >& drills, list< MCAD_OUTLINE* >& cutouts, bool& error )
{
    error = false;

    if( drills.empty() )
        return false;

    list< MCAD_SEGMENT* >::iterator sD = drills.begin();
    list< MCAD_SEGMENT* >::iterator eD = drills.end();
    list< MCAD_SEGMENT* >::iterator iD;
    list<MCAD_POINT> ilist;
    MCAD_INTERSECT_FLAG flag = MCAD_IFLAG_NONE;

    list< list< MCAD_SEGMENT* >* >bundles;
    list< MCAD_SEGMENT* >* sp;

    while( sD != eD )
    {
        iD = sD;
        ++iD;

        while( iD != eD )
        {
            if( (*sD)->GetIntersections( **iD, ilist, flag ) )
            {
                sp = new list< MCAD_SEGMENT* >;
                bundles.push_back( sp );
                sp->push_back( *sD );
                sp->push_back( *iD );

                drills.erase( iD );
                sD = drills.erase( sD );

                // search for all overlapping holes
                list< MCAD_SEGMENT* >::iterator sS = sp->begin();
                list< MCAD_SEGMENT* >::iterator eS = sp->end();
                list< MCAD_SEGMENT* >::iterator sD1;
                list< MCAD_SEGMENT* >::iterator eD1;

                // find every drill which overlaps with each drill in
                // the list; this may result in an incredible number of
                // permutations but is necessary to ensure that overlapping
                // drill holes do not generate invalid geometry.
                while( sS != eS )
                {
                    sD1 = drills.begin();
                    eD1 = drills.end();

                    while( sD1 != eD1 )
                    {
                        if( (*sS)->GetIntersections( **sD1, ilist, flag ) )
                        {
                            sp->push_back( *sD1 );

                            if( sD == sD1 )
                            {
                                sD1 = drills.erase( sD1 );
                                sD = sD1;
                            }
                            else
                            {
                                sD1 = drills.erase( sD1 );
                            }

                        }

                        if( flag )
                        {
                            ERROR_IDF << "\n + [INFO] geometry error (flag = " << flag << ")\n";
                            return false;
                        }

                        ++sD1;
                    }


                    ++sS;
                }

                iD = sD;

                if( iD != eD )
                    ++iD;

                continue;
            }

            if( flag )
            {
                ERROR_IDF << "\n + [INFO] geometry error (flag = " << flag << ")\n";
                return false;
            }

            ++iD;
        }

      ++sD;
    }

    if( bundles.empty() )
        return false;

    // create outlines from each 'bundle'
    list< list< MCAD_SEGMENT* >* >::iterator sB = bundles.begin();
    list< list< MCAD_SEGMENT* >* >::iterator eB = bundles.end();

    while( sB != eB )
    {
        if( !bundleDrills( *sB, cutouts ) )
        {
            ERROR_IDF << "\n + [INFO] problems encountered while merging drill holes\n";
            return false;
        }

        ++sB;
    }

    return true;
}


// take given drill list and punch a cutout using nearest holes in succession
bool bundleDrills( list< MCAD_SEGMENT* >* drills, list< MCAD_OUTLINE* >& cutouts )
{
    vector< pair<double, MCAD_SEGMENT*> > dist; // distance of each from first drill
    list< MCAD_SEGMENT* >::iterator sD = drills->begin();
    list< MCAD_SEGMENT* >::iterator eD = drills->end();
    ++sD;

    MCAD_POINT p0 = drills->front()->GetCenter();
    MCAD_POINT p1;
    double dx;
    double dy;

    // calculate distance^2 between each drill hole
    while( sD != eD )
    {
        p1 = (*sD)->GetCenter();
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        dx = dx*dx + dy*dy;
        dist.push_back( pair<double, MCAD_SEGMENT*>( dx, *sD ) );
        ++sD;
    }

    // sort according to distances
    size_t nd = dist.size();
    pair<double, MCAD_SEGMENT*> tdrill;

    for( size_t i = 0; i < nd -1; ++i )
    {
        for( size_t j = i + 1; j < nd; ++j )
        {
            if( dist[j].first < dist[i].first )
            {
                tdrill = dist[i];
                dist[i] = dist[j];
                dist[j] = tdrill;
            }
        }
    }

    MCAD_OUTLINE* op = new MCAD_OUTLINE;
    bool dud = false;
    op->AddSegment( drills->front(), dud );
    cutouts.push_back( op );

    for( size_t i = 0; i < nd; ++i )
    {
        if( !op->AddOutline( dist[i].second, dud ) )
        {
            ERROR_IDF << "\n + [INFO] could not merge drill holes\n";
            return false;
        }
    }

    drills->clear();
    return true;
}


// delete drill data
void killDrills( list< MCAD_SEGMENT* >& drills )
{
    if( drills.empty() )
        return;

    list< MCAD_SEGMENT* >::iterator sD = drills.begin();
    list< MCAD_SEGMENT* >::iterator eD = drills.end();

    while( sD != eD )
    {
        delete *sD;
        ++sD;
    }

    drills.clear();
    return;
}

// delete cutout data
void killCutouts( list< MCAD_OUTLINE* >& cutouts )
{
    if( cutouts.empty() )
        return;

    list< MCAD_OUTLINE* >::iterator sO = cutouts.begin();
    list< MCAD_OUTLINE* >::iterator eO = cutouts.end();

    while( sO != eO )
    {
        delete *sO;
        ++sO;
    }

    cutouts.clear();
    return;
}


bool initColors( IGES& model, IGES_ENTITY_314** colors )
{
    IGES_ENTITY* ep;

    unsigned char cdef[NCOLORS][3] = {
        { 0x2F, 0xD0, 0x37 },   // green for PC board
        { 0x2F, 0xAA, 0xD0 },   // cyan
        { 0xD0, 0x55, 0x2F },   // peach
        { 0xF0, 0x76, 0x0F },   // orange
        { 0x77, 0x0B, 0xF4 },   // purple
        { 0xF4, 0xC8, 0x0B },   // yellow
        { 0xF4, 0x0B, 0x84 },   // pink
        { 0x1B, 0x20, 0xE4 },   // blue
        { 0x8A, 0xAA, 0x55 },   // olive
    };

    string cname[NCOLORS] = {
      "pcb green",
      "pcb cyan",
      "pcb peach",
      "pcb orange",
      "pcb purple",
      "pcb yellow",
      "pcb pink",
      "pcb blue",
      "pcb olive"
    };

    for( int i = 0; i < NCOLORS; ++i )
    {
        if( !model.NewEntity( ENT_COLOR_DEFINITION, &ep ) )
            return false;

        colors[i] = dynamic_cast<IGES_ENTITY_314*>( ep );

        if( !colors[i] )
        {
            cerr << "*** could not cast pointer to color entity\n";
            model.DelEntity( ep );

            for( int j = 0; j < i; ++j )
            {
                model.DelEntity( (IGES_ENTITY*)colors[j] );
                colors[j] = NULL;
            }

            return false;
        }

        colors[i]->red = cdef[i][0] * 100.0 / 255.0 ;
        colors[i]->green = cdef[i][1] * 100.0 / 255.0 ;
        colors[i]->blue = cdef[i][2] * 100.0 / 255.0 ;

        colors[i]->cname = cname[i];
    }

    return true;
}


bool MakeComponents( IDF3_BOARD& board, IGES& model )
{
    map< string, IGES_ENTITY_308*> componentList; // the IGES component models
    double th = 0.5 * board.GetBoardThickness();

    const map< string, IDF3_COMP_OUTLINE*>* cop = &board.GetComponentOutlines();
    map< string, IDF3_COMP_OUTLINE*>::const_iterator sOP = cop->begin();
    map< string, IDF3_COMP_OUTLINE*>::const_iterator eOP = cop->end();

    IGES_ENTITY_308* subfig;

    while( sOP != eOP )
    {
        if( !buildComponent( model, sOP->second, &subfig ) )
        {
            ERRMSG << "+ [INFO] could not build a component model\n";
            return false;
        }

        if( !subfig )
        {
            // there was no outline to render but we do not
            // have an error condition
            ++sOP;
            continue;
        }

        componentList.insert( pair<string, IGES_ENTITY_308*>( sOP->first, subfig ) );
        ++sOP;
    }

    // instantiate every component

    const map< string, IDF3_COMPONENT* >*const comp = board.GetComponents();

    map< string, IDF3_COMPONENT* >::const_iterator sc = comp->begin();
    map< string, IDF3_COMPONENT* >::const_iterator ec = comp->end();

    list< IDF3_COMP_OUTLINE_DATA* >::const_iterator so;
    list< IDF3_COMP_OUTLINE_DATA* >::const_iterator eo;

    double vX, vY, vA;
    double tX, tY, tZ, tA;
    bool   bottom;
    IDF3::IDF_LAYER lyr;
    string uid;

    while( sc != ec )
    {
        so = sc->second->GetOutlinesData()->begin();
        eo = sc->second->GetOutlinesData()->end();
        // position and orientation of the component on the board
        sc->second->GetPosition( vX, vY, vA, lyr );

        if( lyr == IDF3::LYR_BOTTOM )
            bottom = true;
        else
            bottom = false;

        while( so != eo )
        {
            uid = ((IDF3_COMP_OUTLINE*)((*so)->GetOutline()))->GetUID().c_str();
            map<string, IGES_ENTITY_308*>::iterator outline =
                componentList.find( uid );

            if( outline == componentList.end() )
            {
                // there is no model for this component
                ++so;
                continue;
            }

            // retrieve the additional component and orientation offsets
            (*so)->GetOffsets( tX, tY, tZ, tA );
            tX += vX;
            tY += vY;
            tZ += th;
            tA = (tA + vA) * M_PI / 180.0;

            IGES_ENTITY_124* tx = calcTransform( model, tX, tY, tZ, tA, bottom );

            if( !tx )
            {
                ERRMSG << "\n + [INFO]: could not instantiate a transform for the entity\n";
                return false;
            }

            IGES_ENTITY* ep;
            IGES_ENTITY_408* p408;
            model.NewEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, &ep );
            p408 = (IGES_ENTITY_408*)ep;
            p408->SetTransform( tx );
            p408->SetDE( outline->second );
            p408->SetLabel( sc->second->GetRefDes() );

            ++so;
        }

        ++sc;
    }

    return true;
}


bool MakeOtherOutlines( IDF3_BOARD& board, IGES& model )
{
    double bt = 0.5 * board.GetBoardThickness();

    // Add the component outlines
    const std::map< std::string, OTHER_OUTLINE* >*const comp = board.GetOtherOutlines();
    std::map< std::string, OTHER_OUTLINE* >::const_iterator sc = comp->begin();
    std::map< std::string, OTHER_OUTLINE* >::const_iterator ec = comp->end();

    while( sc != ec )
    {
        if( sc->second->OutlinesSize() < 1 )
        {
            ++sc;
            continue;
        }

        IGES_GEOM_PCB otln; // main outline for this section
        MCAD_OUTLINE* pIGS;
        list< MCAD_OUTLINE* > cutouts;

        list< IDF_OUTLINE* >::const_iterator scont = sc->second->GetOutlines()->begin();
        list< IDF_OUTLINE* >::const_iterator econt = sc->second->GetOutlines()->end();
        list< IDF_OUTLINE* >::const_iterator rcont = scont;

        // extract the outline and cutouts
        while( scont != econt )
        {
            if( scont == rcont )
                pIGS = &otln;
            else
                pIGS = new MCAD_OUTLINE;

            if( !convertOln( pIGS, *scont ) )
            {
                if( scont != rcont )
                    delete pIGS;

                return false;
            }

            if( scont != rcont )
                cutouts.push_back( pIGS );

            ++scont;
        }

        // add all cutouts to the main outline
        bool dud = false;
        list< MCAD_OUTLINE* >::iterator sMO = cutouts.begin();
        list< MCAD_OUTLINE* >::iterator eMO = cutouts.end();

        while( sMO != eMO )
        {
            otln.AddCutout( *sMO, true, dud );
            ++sMO;
        }

        // put in part and solid instance, names, and color
        bool bottom = false;

        if( sc->second->GetSide() == IDF3::LYR_BOTTOM )
            bottom = true;

        vector<IGES_ENTITY_144*> surfs;
        double th = sc->second->GetThickness();

        if( bottom )
        {
            otln.GetVerticalSurface( &model, dud, surfs, -(th + bt), -bt );
            otln.GetTrimmedPlane( &model, dud, surfs, -(th + bt) );
            otln.GetTrimmedPlane( &model, dud, surfs, -bt );
        }
        else
        {
            otln.GetVerticalSurface( &model, dud, surfs, th + bt, bt );
            otln.GetTrimmedPlane( &model, dud, surfs, th + bt );
            otln.GetTrimmedPlane( &model, dud, surfs, bt );
        }

        IGES_ENTITY_308* subfig;

        if( !newSubfigure( model, &subfig ) )
        {
            ERROR_IDF << "\n + could not create a subfigure entity\n";
            return false;
        }

        vector<IGES_ENTITY_144*>::iterator sSL = surfs.begin();
        vector<IGES_ENTITY_144*>::iterator eSL = surfs.end();
        int cidx = GetComponentColor();

        while( sSL != eSL )
        {
            (*sSL)->SetColor( (IGES_ENTITY*) globs.colors[cidx] );
            subfig->AddDE( (IGES_ENTITY*)(*sSL) );
            ++sSL;
        }

        // add the name
        subfig->NAME = sc->first;

        IGES_ENTITY* ep;
        IGES_ENTITY_408* p408;
        model.NewEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, &ep );
        p408 = (IGES_ENTITY_408*)ep;
        p408->SetDE( subfig );
        p408->SetLabel( sc->first );
        ++sc;
    }

    return true;
}


bool newSubfigure( IGES& model, IGES_ENTITY_308** aNewSubfig )
{
    *aNewSubfig = NULL;
    IGES_ENTITY* ep;
    IGES_ENTITY_308* p308;

    if( !model.NewEntity( ENT_SUBFIGURE_DEFINITION, &ep ) )
    {
        ERRMSG << "\n + [INFO] could not create Subfigure Definition Entity\n";
        return false;
    }

    p308 = dynamic_cast<IGES_ENTITY_308*>(ep);

    if( !p308 )
    {
        model.DelEntity( ep );
        ERRMSG << "\n + [INFO] could not cast pointer to Subfigure Definition Entity\n";
        return false;
    }

    *aNewSubfig = p308;
    return true;
}


// build a component part model from the given outline data
bool buildComponent( IGES& model, const IDF3_COMP_OUTLINE* idf, IGES_ENTITY_308** subfig )
{
    *subfig = NULL;

    // note we defeat the 'const' attribute here
    IDF_OUTLINE* op = ((IDF3_COMP_OUTLINE*)idf)->GetOutline( 0 );

    if( op->empty() )
        return true;

    double th = idf->GetThickness();

    if( th < 1e-3 )
    {
        ERRMSG << "\n + [INFO] bad thickness (" << th << ") in component outline\n";
        return true;
    }

    IGES_GEOM_PCB otln;     // component outline

    if( !convertOln( &otln, op ) )
    {
        ERRMSG << "\n + [INFO] could not convert component outline\n";
        return true;
    }

    // put in part definition, names, color and create the component model
    bool dud = false;
    vector<IGES_ENTITY_144*> surfs;
    otln.GetVerticalSurface( &model, dud, surfs, th, 0.0 );
    otln.GetTrimmedPlane( &model, dud, surfs, th );
    otln.GetTrimmedPlane( &model, dud, surfs, 0.0 );

    if( !newSubfigure( model, subfig ) )
    {
        ERROR_IDF << "\n + could not create a subfigure entity\n";
        return false;
    }

    vector<IGES_ENTITY_144*>::iterator sSL = surfs.begin();
    vector<IGES_ENTITY_144*>::iterator eSL = surfs.end();

    int cidx = GetComponentColor();

    while( sSL != eSL )
    {
        (*sSL)->SetColor( (IGES_ENTITY*) globs.colors[cidx] );
        (*subfig)->AddDE( (IGES_ENTITY*)(*sSL) );
        ++sSL;
    }

    // add the name; note this dirty trick to work around retrieval of the UID
    (*subfig)->NAME = ((IDF3_COMP_OUTLINE*)idf)->GetUID().c_str();

    return true;
}


int GetComponentColor( void )
{
    static int cidx = 1;    // index starts at 1 since 0 is the PCB color
    int tc = cidx;

    if( ++cidx == NCOLORS )
        cidx = 1;

    return tc;
}


// create a rotation matrix around Y
void rotateY( MCAD_MATRIX& mat, double angle )
{
    double cosN = cos( angle );
    double sinN = sin( angle );
    mat.v[0][0] = cosN;
    mat.v[0][2] = sinN;
    mat.v[2][0] = -sinN;
    mat.v[2][2] = cosN;

    return;
}

// create a rotation matrix around Z
void rotateZ( MCAD_MATRIX& mat, double angle )
{
    double cosN = cos( angle );
    double sinN = sin( angle );
    mat.v[0][0] = cosN;
    mat.v[0][1] = -sinN;
    mat.v[1][0] = sinN;
    mat.v[1][1] = cosN;
}


IGES_ENTITY_124* calcTransform( IGES& model, double dX, double dY, double dZ, double dA, bool bottom )
{
    MCAD_MATRIX m0;
    MCAD_MATRIX m1;

    if( dA != 0.0 )
    {
        rotateZ( m0, dA );
    }

    if( bottom )
    {
        dZ = -dZ;
        rotateY( m1, M_PI );
    }

    m1 *= m0;
    IGES_ENTITY_124* p124 = NULL;
    IGES_ENTITY* ep;

    if( !model.NewEntity( ENT_TRANSFORMATION_MATRIX, &ep ) )
        return NULL;

    p124 = (IGES_ENTITY_124*)ep;
    p124->T.R = m1;
    p124->T.T.x = dX;
    p124->T.T.y = dY;
    p124->T.T.z = dZ;

    return p124;
}
