/*
 * file: idf2igs.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
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

#include <idf_helpers.h>
#include <idf_common.h>
#include <idf_parser.h>

#include <error_macros.h>
#include <dll_iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <dll_mcad_segment.h>
#include <dll_iges_geom_pcb.h>
#include <all_api_entities.h>
#include <mcad_utils.h>

class IGES_ENTITY_124;
class IGES_ENTITY_144;
class IGES_ENTITY_308;
class IGES_ENTITY_314;
class IGES_ENTITY_408;

using namespace std;

// colors to be used in the output assembly model
#define NCOLORS 9

static struct
{
    string basename;
    IGES_ENTITY_314* colors[NCOLORS];
} globs;

bool initColors( DLL_IGES& model, IGES_ENTITY_314** colors );

// convert IDF outline to IGS outline
bool convertOln( MCAD_OUTLINE* olnIGS, IDF_OUTLINE* olnIDF );
bool convertDrills( list< MCAD_SEGMENT* >& drills, const list< IDF_DRILL_DATA* >* dh );
// merge overlapping drills into cutouts; return true if any drills were merged;
// if invalid geometry was encountered the error flag will be set
bool mergeDrills( list< MCAD_SEGMENT* >& drills, list< MCAD_OUTLINE* >& cutouts, bool& error );
// take given drill list and punch a cutout using nearest holes in succession
bool bundleDrills( list< MCAD_SEGMENT* >* drills, list< MCAD_OUTLINE* >& cutouts );
// delete drill data
void killDrills( list< MCAD_SEGMENT* >& drills );
// delete cutout data
void killCutouts( list< MCAD_OUTLINE* >& cutouts );

bool MakeBoard( IDF3_BOARD& board, DLL_IGES& model );
bool MakeComponents( IDF3_BOARD& board, DLL_IGES& model );
bool MakeOtherOutlines( IDF3_BOARD& board, DLL_IGES& model );
// build a component part model from the given outline data
bool buildComponent( DLL_IGES& model, const IDF3_COMP_OUTLINE* idf, IGES_ENTITY_308** subfig );

// routines to make IGES model creation easier
bool newSubfigure( DLL_IGES& model, IGES_ENTITY_308** aNewSubfig );
IGES_ENTITY_124* calcTransform( DLL_IGES& model, double dX, double dY, double dZ, double dA, bool bottom );

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
    DLL_IGES model;
    // WARNING: TO BE FIXED: iges.h/cpp needs accessro functions implemented
    // model.globalData.productIDSS = "idf2igs test";
    // model.globalData.nativeSystemID = "libIGES";
    model.SetUnitsFlag( UNIT_MM );
    model.SetMinResolution( 1e-8 );

    if( !initColors( model, globs.colors ) )
    {
        cerr << "** Failed to create IGES color entities\n";
        return -1;
    }

    // Create the VRML file and write the header
    MCAD_FILEPATH mofname;
    mofname.SetPath( inputFilename.c_str() );
    mofname.SetExtension( "igs" );
    string fname = mofname.GetFullPath();
    globs.basename = mofname.GetBaseName();

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



bool MakeBoard( IDF3_BOARD& board, DLL_IGES& model )
{
    if( board.GetBoardOutlinesSize() < 1 )
    {
        ERROR_IDF << "\n";
        cerr << "* Cannot proceed; no board outline in IDF object\n";
        return false;
    }

    DLL_IGES_GEOM_PCB otln( true );     // main board outline
    DLL_MCAD_OUTLINE  loop( false );    // outline for a cutout
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
        {
            pIGS = otln.GetRawPtr();
        }
        else
        {
            loop.Detach();
            loop.NewOutline();
            pIGS = loop.GetRawPtr();
        }

        if( !convertOln( pIGS, *scont ) )
        {
            if( scont != rcont )
                loop.DelOutline();

            return false;
        }

        if( scont != rcont )
            cutouts.push_back( pIGS );

        ++scont;
    }

    loop.Detach();

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
    DLL_MCAD_OUTLINE mO( false );

    while( sMO != eMO )
    {
        sDH = drills.begin();
        eDH = drills.end();
        mO.Attach( *sMO );

        while( sDH != eDH )
        {
            if( mO.AddOutline( *sDH , dud ) )
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

        mO.Detach();
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
    IGES_ENTITY_144** surfs = NULL;
    int nSurfs = 0;
    double th = 0.5 * board.GetBoardThickness();
    otln.GetVerticalSurface( model.GetRawPtr(), dud, surfs, nSurfs, th, -th );
    otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, th );
    otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, -th );
    otln.Detach();

    IGES_ENTITY_308* subfig;

    if( !newSubfigure( model, &subfig ) )
    {
        ERROR_IDF << "\n + could not create a subfigure entity\n";
        delete [] surfs;
        return false;
    }

    DLL_IGES_ENTITY_144 e144( model, false );
    DLL_IGES_ENTITY_308 e308( model, false );
    e308.Attach( (IGES_ENTITY*)subfig );

    for( int i = 0; i < nSurfs; ++i )
    {
        e144.Attach( (IGES_ENTITY*)surfs[i] );
        e144.SetColor( (IGES_ENTITY*) globs.colors[0] );
        e144.Detach();
        e308.AddDE( (IGES_ENTITY *)surfs[i] );
    }

    // add the name
    e308.SetName( (char const*)globs.basename.c_str() );
    e308.Detach();

    DLL_IGES_ENTITY_408 e408( model, true );
    e408.SetDE( subfig );
    e408.SetLabel( globs.basename.c_str() );
    e408.Detach();

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

    DLL_MCAD_OUTLINE oIGS( false );
    oIGS.Attach( olnIGS );

    std::list<IDF_SEGMENT*>::iterator sseg;
    std::list<IDF_SEGMENT*>::iterator eseg;
    IDF_SEGMENT* ps;
    DLL_MCAD_SEGMENT pm( false );
    MCAD_POINT   pts[3];
    sseg = olnIDF->begin();
    eseg = olnIDF->end();
    bool dud = false;

    while( sseg != eseg )
    {
        // convert IDF to IGES
        ps = *sseg;
        pm.NewSegment();

        if( ps->IsCircle() )
        {
            pts[0].x = ps->center.x;
            pts[0].y = ps->center.y;
            pts[0].z = 0;

            pts[1].x = pts[0].x + ps->radius;
            pts[1].y = pts[0].y;
            pts[1].z = 0;

            pm.SetParams( pts[0], pts[1], pts[1], false );
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
                pm.SetParams( pts[0], pts[1], pts[2], false );
            else
                pm.SetParams( pts[0], pts[1], pts[2], true );

        }
        else
        {
            pts[0].x = ps->startPoint.x;
            pts[0].y = ps->startPoint.y;
            pts[0].z = 0;

            pts[1].x = ps->endPoint.x;
            pts[1].y = ps->endPoint.y;
            pts[1].z = 0;

            pm.SetParams( pts[0], pts[1] );
        }

        if( !oIGS.AddSegment( pm, dud ) || dud )
        {
            oIGS.Detach();
            pm.DelSegment();
            ERROR_IDF << "could not add segment to outline\n";
            return false;
        }

        pm.Detach();
        ++sseg;
    }

    oIGS.Detach();
    return true;
}


bool convertDrills( list< MCAD_SEGMENT* >& drills, const list<IDF_DRILL_DATA*>* dh )
{
    // XXX - for brevity, error checking has been left out

    list< IDF_DRILL_DATA* >::const_iterator sD = dh->begin();
    list< IDF_DRILL_DATA* >::const_iterator eD = dh->end();
    DLL_MCAD_SEGMENT sp( false );
    MCAD_POINT p[2];

    while( sD != eD )
    {
        sp.NewSegment();

        p[0].x = (*sD)->GetDrillXPos();
        p[0].y = (*sD)->GetDrillYPos();
        p[0].z = 0.0;

        p[1].x = p[0].x + 0.5 * (*sD)->GetDrillDia();
        p[1].y = p[0].y;
        p[1].z = 0.0;

        sp.SetParams( p[0], p[1], p[1], false );
        drills.push_back( sp.GetRawPtr() );
        sp.Detach();

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
    MCAD_POINT* ilist = NULL;
    int nPoints = 0;
    MCAD_INTERSECT_FLAG flag = MCAD_IFLAG_NONE;

#define CLEAR_ILIST() do {\
    if( NULL != ilist ) delete [] ilist; \
    ilist = NULL; \
    nPoints = 0; } while( 0 )

    list< list< MCAD_SEGMENT* >* >bundles;
    list< MCAD_SEGMENT* >* sp;

    DLL_MCAD_SEGMENT seg0( false );
    DLL_MCAD_SEGMENT seg1( false );
    DLL_MCAD_SEGMENT seg2( false );

    while( sD != eD )
    {
        seg2.Attach( *sD );
        iD = sD;
        ++iD;

        bool skip = false;

        while( iD != eD )
        {
            if( seg2.GetIntersections( *iD, ilist, nPoints, flag ) )
            {
                skip = true;
                CLEAR_ILIST();
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
                    seg0.Attach( *sS );

                    while( sD1 != eD1 )
                    {
                        seg1.Attach( *sD1 );

                        if( seg0.GetIntersections( seg1, ilist, nPoints, flag ) )
                        {
                            CLEAR_ILIST();

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

                            seg1.Detach();
                            continue;
                        }

                        seg1.Detach();

                        if( flag )
                        {
                            ERROR_IDF << "\n + [INFO] geometry error (flag = " << flag << ")\n";
                            seg0.Detach();
                            seg2.Detach();
                            return false;
                        }

                        ++sD1;
                    }

                    seg0.Detach();
                    ++sS;
                }

                iD = sD;
                continue;
            }

            if( flag )
            {
                seg2.Detach();
                ERROR_IDF << "\n + [INFO] geometry error (flag = " << flag << ")\n";
                return false;
            }

            ++iD;
        }

        seg2.Detach();

        if( skip )
            continue;

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

    MCAD_POINT p0;
    MCAD_POINT p1;
    double dx;
    double dy;
    double r0;
    double r1;
    double r2;

    DLL_MCAD_SEGMENT seg0( false );
    seg0.Attach( drills->front() );
    seg0.GetCenter( p0 );
    seg0.GetRadius( r0 );
    seg0.Detach();

    // calculate [distance - (R0 + R1)] between each drill hole
    while( sD != eD )
    {
        seg0.Attach( *sD );
        seg0.GetCenter( p1 );
        seg0.GetRadius( r1 );
        seg0.Detach();

        r2 = r0 + r1;
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        dx = dx*dx + dy*dy;
        dx = sqrt( dx );
        dx -= r2;
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

    DLL_MCAD_OUTLINE op( true );
    bool dud = false;
    op.AddSegment( drills->front(), dud );
    cutouts.push_back( op.GetRawPtr() );

    for( size_t i = 0; i < nd; ++i )
    {
        if( !op.AddOutline( dist[i].second, dud ) )
        {
            ERROR_IDF << "\n + [INFO] could not merge drill holes\n";
            return false;
        }
    }

    op.Detach();
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
    DLL_MCAD_SEGMENT seg0( false );

    while( sD != eD )
    {
        seg0.Attach( *sD );
        seg0.DelSegment();
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
    DLL_MCAD_OUTLINE out0( false );

    while( sO != eO )
    {
        out0.Attach( *sO );
        out0.DelOutline();
        ++sO;
    }

    cutouts.clear();
    return;
}


bool initColors( DLL_IGES& model, IGES_ENTITY_314** colors )
{
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

    DLL_IGES_ENTITY_314 dE( model, false );

    for( int i = 0; i < NCOLORS; ++i )
    {
        dE.Detach();
        dE.NewEntity();

        dE.SetColor( cdef[i][0] * 100.0 / 255.0,
                     cdef[i][1] * 100.0 / 255.0,
                     cdef[i][2] * 100.0 / 255.0 );
        dE.SetName( cname[i].c_str() );

        colors[i] = (IGES_ENTITY_314*)dE.GetRawPtr();

        if( !colors[i] )
        {
            cerr << "*** could not cast pointer to color entity\n";
            dE.DelEntity();

            for( int j = 0; j < i; ++j )
            {
                model.DelEntity( (IGES_ENTITY*)colors[j] );
                colors[j] = NULL;
            }

            return false;
        }
    }

    return true;
}


bool MakeComponents( IDF3_BOARD& board, DLL_IGES& model )
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

            DLL_IGES_ENTITY_408 e408( model, true );
            e408.SetTransform( (IGES_ENTITY*)tx );
            e408.SetDE( outline->second );
            e408.SetLabel( sc->second->GetRefDes().c_str() );
            e408.Detach();

            ++so;
        }

        ++sc;
    }

    return true;
}


bool MakeOtherOutlines( IDF3_BOARD& board, DLL_IGES& model )
{
    double bt = 0.5 * board.GetBoardThickness();

    // Add the component outlines
    const std::map< std::string, OTHER_OUTLINE* >*const comp = board.GetOtherOutlines();
    std::map< std::string, OTHER_OUTLINE* >::const_iterator sc = comp->begin();
    std::map< std::string, OTHER_OUTLINE* >::const_iterator ec = comp->end();

    DLL_IGES_GEOM_PCB otln( true );     // main outline for this section

    while( sc != ec )
    {
        if( sc->second->OutlinesSize() < 1 )
        {
            ++sc;
            continue;
        }

        DLL_MCAD_OUTLINE  loop( false );    // individual outlines
        MCAD_OUTLINE* pIGS;
        list< MCAD_OUTLINE* > cutouts;

        list< IDF_OUTLINE* >::const_iterator scont = sc->second->GetOutlines()->begin();
        list< IDF_OUTLINE* >::const_iterator econt = sc->second->GetOutlines()->end();
        list< IDF_OUTLINE* >::const_iterator rcont = scont;

        // extract the outline and cutouts
        while( scont != econt )
        {
            if( scont == rcont )
            {
                pIGS = otln.GetRawPtr();
            }
            else
            {
                loop.Detach();
                loop.NewOutline();
                pIGS = loop.GetRawPtr();
            }

            if( !convertOln( pIGS, *scont ) )
            {
                if( scont != rcont )
                    loop.DelOutline();

                return false;
            }

            if( scont != rcont )
                cutouts.push_back( pIGS );

            ++scont;
        }

        loop.Detach();

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

        IGES_ENTITY_144** surfs = NULL;
        int nSurfs = 0;
        double th = sc->second->GetThickness();

        if( bottom )
        {
            otln.GetVerticalSurface( model.GetRawPtr(), dud, surfs, nSurfs, -(th + bt), -bt );
            otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, -(th + bt) );
            otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, -bt );
        }
        else
        {
            otln.GetVerticalSurface( model.GetRawPtr(), dud, surfs, nSurfs, th + bt, bt );
            otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, th + bt );
            otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, bt );
        }

        DLL_IGES_ENTITY_308 subfig( model, true );

        if( !subfig.IsValid() )
        {
            otln.Detach();
            delete [] surfs;
            ERROR_IDF << "\n + could not create a subfigure entity\n";
            return false;
        }

        int cidx = GetComponentColor();

        DLL_IGES_ENTITY_144 e144( model, false );

        for( int i = 0; i < nSurfs; ++ i )
        {
            e144.Attach( (IGES_ENTITY*)surfs[i] );
            e144.SetColor( (IGES_ENTITY*) globs.colors[cidx] );
            e144.Detach();
            subfig.AddDE( (IGES_ENTITY *)surfs[i] );
        }

        delete [] surfs;

        // add the name
        subfig.SetName( sc->first.c_str() );

        DLL_IGES_ENTITY_408 e408( model, true );
        e408.SetDE( (IGES_ENTITY_308*)subfig.GetRawPtr() );
        e408.SetLabel( sc->first.c_str() );
        e408.Detach();
        subfig.Detach();
        ++sc;
    }

    otln.Detach();
    return true;
}


bool newSubfigure( DLL_IGES& model, IGES_ENTITY_308** aNewSubfig )
{
    *aNewSubfig = NULL;

    DLL_IGES_ENTITY_308 e308( model, true );

    if( !e308.IsValid() )
    {
        ERRMSG << "\n + [INFO] could not create Subfigure Definition Entity\n";
        return false;
    }

    *aNewSubfig = (IGES_ENTITY_308*)e308.GetRawPtr();
    e308.Detach();
    return true;
}


// build a component part model from the given outline data
bool buildComponent( DLL_IGES& model, const IDF3_COMP_OUTLINE* idf, IGES_ENTITY_308** subfig )
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

    DLL_IGES_GEOM_PCB otln( true ); // component outline

    if( !convertOln( otln.GetRawPtr(), op ) )
    {
        ERRMSG << "\n + [INFO] could not convert component outline\n";
        return true;
    }

    // put in part definition, names, color and create the component model
    bool dud = false;
    IGES_ENTITY_144** surfs = NULL;
    int nSurfs = 0;

    otln.GetVerticalSurface( model.GetRawPtr(), dud, surfs, nSurfs, th, 0.0 );
    otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, th );
    otln.GetTrimmedPlane( model.GetRawPtr(), dud, surfs, nSurfs, 0.0 );
    otln.Detach();

    if( !newSubfigure( model, subfig ) )
    {
        ERROR_IDF << "\n + could not create a subfigure entity\n";
        return false;
    }

    int cidx = GetComponentColor();
    DLL_IGES_ENTITY_144 e144( model, false );
    DLL_IGES_ENTITY_308 e308( model, false );
    e308.Attach( (IGES_ENTITY*)*subfig );

    for( int i = 0; i < nSurfs; ++ i )
    {
        e144.Attach( (IGES_ENTITY*)surfs[i] );
        e144.SetColor( (IGES_ENTITY*) globs.colors[cidx] );
        e144.Detach();
        e308.AddDE((IGES_ENTITY *) surfs[i] );
    }

    // add the name; note this dirty trick to work around retrieval of the UID
    e308.SetName( ((IDF3_COMP_OUTLINE*)idf)->GetUID().c_str() );
    e308.Detach();

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


IGES_ENTITY_124* calcTransform( DLL_IGES& model, double dX, double dY, double dZ, double dA, bool bottom )
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
    DLL_IGES_ENTITY_124 e124( model, true );
    MCAD_TRANSFORM tx;
    tx.R = m1;
    tx.T.x = dX;
    tx.T.y = dY;
    tx.T.z = dZ;
    MCAD_TRANSFORM* tp = &tx;
    e124.SetRootTransform( tp );
    IGES_ENTITY_124* ep = (IGES_ENTITY_124*)e124.GetRawPtr();
    e124.Detach();

    return ep;
}
