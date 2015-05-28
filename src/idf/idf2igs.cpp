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
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cerrno>
#include <list>
#include <utility>
#include <clocale>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <libgen.h>
#include <unistd.h>

#include <idf_helpers.h>
#include <idf_common.h>
#include <idf_parser.h>

#include <iges.h>
#include <geom_wall.h>
#include <geom_cylinder.h>
#include <mcad_segment.h>
#include <iges_geom_pcb.h>

extern char* optarg;
extern int   optopt;

using namespace std;

#define CLEANUP do { \
setlocale( LC_ALL, "C" ); \
} while( 0 );

// define colors
struct VRML_COLOR
{
    double diff[3];
    double emis[3];
    double spec[3];
    double ambi;
    double tran;
    double shin;
};

struct VRML_IDS
{
    int colorIndex;
    std::string objectName;
    bool used;
    bool bottom;
    double dX, dY, dZ, dA;

    VRML_IDS()
    {
        colorIndex = 0;
        used = false;
        bottom = false;
        dX = 0.0;
        dY = 0.0;
        dZ = 0.0;
        dA = 0.0;
    }
};

#define NCOLORS 7
VRML_COLOR colors[NCOLORS] =
{
    { { 0, 0.82, 0.247 },       { 0, 0, 0 },    { 0, 0.82, 0.247 },         0.9, 0, 0.1 },
    { { 1, 0, 0 },              { 1, 0, 0 },    { 1, 0, 0 },                0.9, 0, 0.1 },
    { { 0.659, 0, 0.463 },      { 0, 0, 0 },    { 0.659, 0, 0.463 },        0.9, 0, 0.1 },
    { { 0.659, 0.294, 0 },      { 0, 0, 0 },    { 0.659, 0.294, 0 },        0.9, 0, 0.1 },
    { { 0, 0.918, 0.659 },      { 0, 0, 0 },    { 0, 0.918, 0.659 },        0.9, 0, 0.1 },
    { { 0.808, 0.733, 0.071 },  { 0, 0, 0 },    { 0.808, 0.733 , 0.071 },   0.9, 0, 0.1 },
    { { 0.102, 1, 0.984 },      { 0, 0, 0 },    { 0.102, 1, 0.984 },        0.9, 0, 0.1 }
};

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

#ifdef GOOBLE0
bool PopulateVRML( VRML_LAYER& model, const std::list< IDF_OUTLINE* >* items, bool bottom,
                   double scale, double dX = 0.0, double dY = 0.0, double angle = 0.0 );
bool AddSegment( VRML_LAYER& model, IDF_SEGMENT* seg, int icont, int iseg );
bool WriteTriangles( std::ofstream& file, VRML_IDS* vID, VRML_LAYER* layer, bool plane,
                     bool top, double top_z, double bottom_z, int precision, bool compact );
inline void TransformPoint( IDF_SEGMENT& seg, double frac, bool bottom,
                            double dX, double dY, double angle );
VRML_IDS* GetColor( std::map<std::string, VRML_IDS*>& cmap,
                      int& index, const std::string& uid );
#endif

void PrintUsage( void )
{
    cout << "-\nUsage: idf2vrml -f input_file.emn -s scale_factor {-k} {-d} {-z} {-m}\n";
    cout << "flags:\n";
    cout << "       -k: produce KiCad-friendly VRML output; default is compact VRML\n";
    cout << "       -d: suppress substitution of default outlines\n";
    cout << "       -z: suppress rendering of zero-height outlines\n";
    cout << "       -m: print object mapping to stdout for debugging purposes\n";
    cout << "example to produce a model for use by KiCad: idf2vrml -f input.emn -s 0.3937008 -k\n\n";
    return;
}

bool nozeroheights;
bool showObjectMapping;

int main( int argc, char **argv )
{
    // IDF implicitly requires the C locale
    setlocale( LC_ALL, "C" );

    // Essential inputs:
    // 1. IDF file

    std::string inputFilename;
    int    ichar;

    while( ( ichar = getopt( argc, argv, ":f:" ) ) != -1 )
    {
        switch( ichar )
        {
        case 'f':
            inputFilename = optarg;
            break;

        default:
            cerr << "* Unexpected option: '-";

            if( ichar == '?' )
                cerr << ((char) optopt) << "'\n";
            else
                cerr << ((char) ichar) << "'\n";

            PrintUsage();
            return -1;
            break;
        }
    }

    if( inputFilename.empty() )
    {
        cerr << "* no IDF filename supplied\n";
        PrintUsage();
        return -1;
    }

    IDF3_BOARD pcb( IDF3::CAD_ELEC );

    cout << "** Reading file: " << inputFilename << "\n";

    if( !pcb.ReadFile( FROM_UTF8( inputFilename.c_str() ), true ) )
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

    // Create the VRML file and write the header
    char* bnp = (char*) malloc( inputFilename.size() + 1 );
    strcpy( bnp, inputFilename.c_str() );

    std::string fname = basename( bnp );
    free( bnp );
    std::string::iterator itf = fname.end();
    *(--itf) = 's';
    *(--itf) = 'g';
    *(--itf) = 'i';

    cout << "Output file: '" << fname << "'\n";

    // STEP 1: Render the PCB alone
    MakeBoard( pcb, model );

    // STEP 2: Render the components
    //MakeComponents( pcb, model );

    // STEP 3: Render the OTHER outlines
    //MakeOtherOutlines( pcb, model );

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

    // XXX - put in part and solid instance, names, and color
    // create the PCB model
    vector<IGES_ENTITY_144*> surfs;
    double th = 0.5 * board.GetBoardThickness();
    otln.GetVerticalSurface( &model, dud, surfs, th, -th );
    otln.GetTrimmedPlane( &model, dud, surfs, th );
    otln.GetTrimmedPlane( &model, dud, surfs, -th );

    // XXX - CODE FOR VRML

#ifdef GOOBLE0
    // set the arc parameters according to output scale
    int tI;
    double tMin, tMax;
    vpcb.GetArcParams( tI, tMin, tMax );
    vpcb.SetArcParams( tI, tMin * scale, tMax * scale );

    if( !PopulateVRML( vpcb, board.GetBoardOutline()->GetOutlines(), false, board.GetUserScale() ) )
    {
        return false;
    }

    vpcb.EnsureWinding( 0, false );

    int nvcont = vpcb.GetNContours() - 1;

    while( nvcont > 0 )
        vpcb.EnsureWinding( nvcont--, true );

    // Add the drill holes
    const std::list<IDF_DRILL_DATA*>* drills = &board.GetBoardDrills();

    std::list<IDF_DRILL_DATA*>::const_iterator sd = drills->begin();
    std::list<IDF_DRILL_DATA*>::const_iterator ed = drills->end();

    while( sd != ed )
    {
        vpcb.AddCircle( (*sd)->GetDrillXPos() * scale, (*sd)->GetDrillYPos() * scale,
                        (*sd)->GetDrillDia() * scale / 2.0, true );
        ++sd;
    }

    std::map< std::string, IDF3_COMPONENT* >*const comp = board.GetComponents();
    std::map< std::string, IDF3_COMPONENT* >::const_iterator sc = comp->begin();
    std::map< std::string, IDF3_COMPONENT* >::const_iterator ec = comp->end();

    while( sc != ec )
    {
        drills = sc->second->GetDrills();
        sd = drills->begin();
        ed = drills->end();

        while( sd != ed )
        {
            vpcb.AddCircle( (*sd)->GetDrillXPos() * scale, (*sd)->GetDrillYPos() * scale,
                            (*sd)->GetDrillDia() * scale / 2.0, true );
            ++sd;
        }

        ++sc;
    }

    // tesselate and write out
    vpcb.Tesselate( NULL );

    double thick = board.GetBoardThickness() / 2.0 * scale;

    VRML_IDS tvid;
    tvid.colorIndex = 0;

    WriteTriangles( file, &tvid, &vpcb, false, false,
                    thick, -thick, board.GetUserPrecision(), false );
#endif  // GOOBLE0

    return true;
}


// convert IDF outline to IGS outline
bool convertOln( MCAD_OUTLINE* olnIGS, IDF_OUTLINE* olnIDF )
{
    if( olnIDF->size() < 1 )
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
                            // XXX - something went wrong
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
                // XXX - something went wrong
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
            // XXX - something went wrong
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
            // XXX - something went wrong
            cerr << "XXX: QWERTY - we're screwed!\n";
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

// XXX - OLD VRML CODE
#ifdef GOOBLE0

bool PopulateVRML( VRML_LAYER& model, const std::list< IDF_OUTLINE* >* items, bool bottom, double scale,
                   double dX, double dY, double angle )
{
    // empty outlines are not unusual so we fail quietly
    if( items->size() < 1 )
        return false;

    int nvcont = 0;
    int iseg   = 0;

    std::list< IDF_OUTLINE* >::const_iterator scont = items->begin();
    std::list< IDF_OUTLINE* >::const_iterator econt = items->end();
    std::list<IDF_SEGMENT*>::iterator sseg;
    std::list<IDF_SEGMENT*>::iterator eseg;

    IDF_SEGMENT lseg;

    while( scont != econt )
    {
        nvcont = model.NewContour();

        if( nvcont < 0 )
        {
            ERROR_IDF << "\n";
            cerr << "* cannot create an outline\n";
            return false;
        }

        if( (*scont)->size() < 1 )
        {
            ERROR_IDF << "invalid contour: no vertices\n";
            return false;
        }

        sseg = (*scont)->begin();
        eseg = (*scont)->end();

        iseg = 0;
        while( sseg != eseg )
        {
            lseg = **sseg;
            TransformPoint( lseg, scale, bottom, dX, dY, angle );

            if( !AddSegment( model, &lseg, nvcont, iseg ) )
                return false;

            ++iseg;
            ++sseg;
        }

        ++scont;
    }

    return true;
}


bool AddSegment( VRML_LAYER& model, IDF_SEGMENT* seg, int icont, int iseg )
{
    // note: in all cases we must add all but the last point in the segment
    // to avoid redundant points

    if( seg->angle != 0.0 )
    {
        if( seg->IsCircle() )
        {
            if( iseg != 0 )
            {
                ERROR_IDF << "adding a circle to an existing vertex list\n";
                return false;
            }

            return model.AppendCircle( seg->center.x, seg->center.y, seg->radius, icont );
        }
        else
        {
            return model.AppendArc( seg->center.x, seg->center.y, seg->radius,
                                    seg->offsetAngle, seg->angle, icont );
        }
    }

    if( !model.AddVertex( icont, seg->startPoint.x, seg->startPoint.y ) )
        return false;

    return true;
}


bool WriteTriangles( std::ofstream& file, VRML_IDS* vID, VRML_LAYER* layer, bool plane,
                     bool top, double top_z, double bottom_z, int precision, bool compact )
{
    if( vID == NULL || layer == NULL )
        return false;

    file << "Transform {\n";

    if( compact && !vID->objectName.empty() )
    {
        file << "translation " << setprecision( precision ) << vID->dX;
        file << " " << vID->dY << " ";

        if( vID->bottom )
        {
            file << -vID->dZ << "\n";

            double tx, ty;

            // calculate the rotation axis and angle
            tx = cos( M_PI2 - vID->dA / 2.0 );
            ty = sin( M_PI2 - vID->dA / 2.0 );

            file << "rotation " << setprecision( precision );
            file << tx << " " << ty << " 0 ";
            file << setprecision(5) << M_PI << "\n";
        }
        else
        {
            file << vID->dZ << "\n";
            file << "rotation 0 0 1 " << setprecision(5) << vID->dA << "\n";
        }

        file << "children [\n";

        if( vID->used )
        {
            file << "USE " << vID->objectName << "\n";
            file << "]\n";
            file << "}\n";
            return true;
        }

        file << "DEF " << vID->objectName << " Transform {\n";

        if( !plane && top_z <= bottom_z )
        {
            // the height specification is faulty; make the component
            // a bright red to highlight it
            vID->colorIndex = 1;
            // we don't know the scale, but 5 units is huge in most situations
            top_z = bottom_z + 5.0;
        }

    }

    VRML_COLOR* color = &colors[vID->colorIndex];

    vID->used = true;

    file << "children [\n";
    file << "Group {\n";
    file << "children [\n";
    file << "Shape {\n";
    file << "appearance Appearance {\n";
    file << "material Material {\n";

    // material definition
    file << "diffuseColor " << setprecision(3) << color->diff[0] << " ";
    file << color->diff[1] << " " << color->diff[2] << "\n";
    file << "specularColor " << color->spec[0] << " " << color->spec[1];
    file << " " << color->spec[2] << "\n";
    file << "emissiveColor " << color->emis[0] << " " << color->emis[1];
    file << " " << color->emis[2] << "\n";
    file << "ambientIntensity " << color->ambi << "\n";
    file << "transparency " << color->tran << "\n";
    file << "shininess " << color->shin << "\n";

    file << "}\n";
    file << "}\n";
    file << "geometry IndexedFaceSet {\n";
    file << "solid TRUE\n";
    file << "coord Coordinate {\n";
    file << "point [\n";

    // Coordinates (vertices)
    if( plane )
    {
        if( !layer->WriteVertices( top_z, file, precision ) )
        {
            cerr << "* errors writing planar vertices to " << vID->objectName << "\n";
            cerr << "** " << layer->GetError() << "\n";
        }
    }
    else
    {
        if( !layer->Write3DVertices( top_z, bottom_z, file, precision ) )
        {
            cerr << "* errors writing 3D vertices to " << vID->objectName << "\n";
            cerr << "** " << layer->GetError() << "\n";
        }
    }

    file << "\n";

    file << "]\n";
    file << "}\n";
    file << "coordIndex [\n";

    // Indices
    if( plane )
        layer->WriteIndices( top, file );
    else
        layer->Write3DIndices( file );

    file << "\n";
    file << "]\n";
    file << "}\n";
    file << "}\n";
    file << "]\n";
    file << "}\n";
    file << "]\n";
    file << "}\n";

    if( compact && !vID->objectName.empty() )
    {
        file << "]\n";
        file << "}\n";
    }

    return !file.fail();
}

inline void TransformPoint( IDF_SEGMENT& seg, double frac, bool bottom,
                            double dX, double dY, double angle )
{
    dX *= frac;
    dY *= frac;

    if( bottom )
    {
        // mirror points on the Y axis
        seg.startPoint.x = -seg.startPoint.x;
        seg.endPoint.x = -seg.endPoint.x;
        seg.center.x = -seg.center.x;
        angle = -angle;
    }

    seg.startPoint.x *= frac;
    seg.startPoint.y *= frac;
    seg.endPoint.x *= frac;
    seg.endPoint.y *= frac;
    seg.center.x *= frac;
    seg.center.y *= frac;

    double tsin = 0.0;
    double tcos = 0.0;

    if( angle > MIN_ANG || angle < -MIN_ANG )
    {
        double ta = angle * M_PI / 180.0;
        double tx, ty;

        tsin = sin( ta );
        tcos = cos( ta );

        tx = seg.startPoint.x * tcos - seg.startPoint.y * tsin;
        ty = seg.startPoint.x * tsin + seg.startPoint.y * tcos;
        seg.startPoint.x = tx;
        seg.startPoint.y = ty;

        tx = seg.endPoint.x * tcos - seg.endPoint.y * tsin;
        ty = seg.endPoint.x * tsin + seg.endPoint.y * tcos;
        seg.endPoint.x = tx;
        seg.endPoint.y = ty;

        if( seg.angle != 0 )
        {
            tx = seg.center.x * tcos - seg.center.y * tsin;
            ty = seg.center.x * tsin + seg.center.y * tcos;
            seg.center.x = tx;
            seg.center.y = ty;
        }
    }

    seg.startPoint.x += dX;
    seg.startPoint.y += dY;
    seg.endPoint.x += dX;
    seg.endPoint.y += dY;
    seg.center.x += dX;
    seg.center.y += dY;

    if( seg.angle != 0 )
    {
        seg.radius *= frac;

        if( bottom )
        {
            if( !seg.IsCircle() )
            {
                seg.angle = -seg.angle;
                if( seg.offsetAngle > 0.0 )
                    seg.offsetAngle = 180 - seg.offsetAngle;
                else
                    seg.offsetAngle = -seg.offsetAngle - 180;
            }
        }

        if( angle > MIN_ANG || angle < -MIN_ANG )
            seg.offsetAngle += angle;
    }

    return;
}

bool MakeComponents( IDF3_BOARD& board, IGES& model )
{
    int cidx = 2;   // color index; start at 2 since 0,1 are special (board, NOGEOM_NOPART)

    VRML_LAYER vpcb;

    double scale = board.GetUserScale();
    double thick = board.GetBoardThickness() / 2.0;

    // set the arc parameters according to output scale
    int tI;
    double tMin, tMax;
    vpcb.GetArcParams( tI, tMin, tMax );
    vpcb.SetArcParams( tI, tMin * scale, tMax * scale );

    // Add the component outlines
    const std::map< std::string, IDF3_COMPONENT* >*const comp = board.GetComponents();
    std::map< std::string, IDF3_COMPONENT* >::const_iterator sc = comp->begin();
    std::map< std::string, IDF3_COMPONENT* >::const_iterator ec = comp->end();

    std::list< IDF3_COMP_OUTLINE_DATA* >::const_iterator so;
    std::list< IDF3_COMP_OUTLINE_DATA* >::const_iterator eo;

    double vX, vY, vA;
    double tX, tY, tZ, tA;
    double top, bot;
    bool   bottom;
    IDF3::IDF_LAYER lyr;

    std::map< std::string, VRML_IDS*> cmap;  // map colors by outline UID
    VRML_IDS* vcp;
    IDF3_COMP_OUTLINE* pout;

    while( sc != ec )
    {
        sc->second->GetPosition( vX, vY, vA, lyr );

        if( lyr == IDF3::LYR_BOTTOM )
            bottom = true;
        else
            bottom = false;

        so = sc->second->GetOutlinesData()->begin();
        eo = sc->second->GetOutlinesData()->end();

        while( so != eo )
        {
            if( (*so)->GetOutline()->GetThickness() < 0.00000001 && nozeroheights )
            {
                vpcb.Clear();
                ++so;
                continue;
            }

            (*so)->GetOffsets( tX, tY, tZ, tA );
            tX += vX;
            tY += vY;
            tA += vA;

            if( ( pout = (IDF3_COMP_OUTLINE*)((*so)->GetOutline()) ) )
            {
                vcp = GetColor( cmap, cidx, pout->GetUID() );
            }
            else
            {
                vpcb.Clear();
                ++so;
                continue;
            }

            if( !compact )
            {
                if( !PopulateVRML( vpcb, (*so)->GetOutline()->GetOutlines(), bottom,
                    board.GetUserScale(), tX, tY, tA ) )
                {
                    return false;
                }
            }
            else
            {
                if( !vcp->used && !PopulateVRML( vpcb, (*so)->GetOutline()->GetOutlines(), false,
                    board.GetUserScale() ) )
                {
                    return false;
                }

                vcp->dX = tX * scale;
                vcp->dY = tY * scale;
                vcp->dZ = tZ * scale;
                vcp->dA = tA * M_PI / 180.0;
            }

            if( !compact || !vcp->used )
            {
                vpcb.EnsureWinding( 0, false );

                int nvcont = vpcb.GetNContours() - 1;

                while( nvcont > 0 )
                    vpcb.EnsureWinding( nvcont--, true );

                vpcb.Tesselate( NULL );
            }

            if( !compact )
            {
                if( bottom )
                {
                    top = -thick - tZ;
                    bot = (top - (*so)->GetOutline()->GetThickness() ) * scale;
                    top *= scale;
                }
                else
                {
                    bot = thick + tZ;
                    top = (bot + (*so)->GetOutline()->GetThickness() ) * scale;
                    bot *= scale;
                }
            }
            else
            {
                bot = thick;
                top = (bot + (*so)->GetOutline()->GetThickness() ) * scale;
                bot *= scale;
            }

            vcp = GetColor( cmap, cidx, ((IDF3_COMP_OUTLINE*)((*so)->GetOutline()))->GetUID() );
            vcp->bottom = bottom;

            // note: this can happen because IDF allows some negative heights/thicknesses
            if( bot > top )
                std::swap( bot, top );

            WriteTriangles( file, vcp, &vpcb, false,
                            false, top, bot, board.GetUserPrecision(), compact );

            vpcb.Clear();
            ++so;
        }

        ++sc;
    }

    return true;
}


VRML_IDS* GetColor( std::map<std::string, VRML_IDS*>& cmap, int& index, const std::string& uid )
{
    static int refnum = 0;

    if( index < 2 )
        index = 2;   // 0 and 1 are special (BOARD, UID=NOGEOM_NOPART)

    std::map<std::string, VRML_IDS*>::iterator cit = cmap.find( uid );

    if( cit == cmap.end() )
    {
        VRML_IDS* id = new VRML_IDS;

        if( !uid.compare( "NOGEOM_NOPART" ) )
            id->colorIndex = 1;
        else
            id->colorIndex = index++;

        std::ostringstream ostr;
        ostr << "OBJECTn" << refnum++;
        id->objectName = ostr.str();

        if( showObjectMapping )
            cout << "* " << ostr.str() << " = '" << uid << "'\n";

        cmap.insert( std::pair<std::string, VRML_IDS*>(uid, id) );

        if( index >= NCOLORS )
            index = 2;

        return id;
    }

    return cit->second;
}


bool MakeOtherOutlines( IDF3_BOARD& board, IGES& model )
{
    int cidx = 2;   // color index; start at 2 since 0,1 are special (board, NOGEOM_NOPART)

    VRML_LAYER vpcb;

    double scale = board.GetUserScale();
    double thick = board.GetBoardThickness() / 2.0;

    // set the arc parameters according to output scale
    int tI;
    double tMin, tMax;
    vpcb.GetArcParams( tI, tMin, tMax );
    vpcb.SetArcParams( tI, tMin * scale, tMax * scale );

    // Add the component outlines
    const std::map< std::string, OTHER_OUTLINE* >*const comp = board.GetOtherOutlines();
    std::map< std::string, OTHER_OUTLINE* >::const_iterator sc = comp->begin();
    std::map< std::string, OTHER_OUTLINE* >::const_iterator ec = comp->end();

    double top, bot;
    bool   bottom;
    int nvcont;

    std::map< std::string, VRML_IDS*> cmap;  // map colors by outline UID
    VRML_IDS* vcp;
    OTHER_OUTLINE* pout;

    while( sc != ec )
    {
        pout = sc->second;

        if( pout->GetSide() == IDF3::LYR_BOTTOM )
            bottom = true;
        else
            bottom = false;

        if( pout->GetThickness() < 0.00000001 && nozeroheights )
        {
            vpcb.Clear();
            ++sc;
            continue;
        }

        vcp = GetColor( cmap, cidx, pout->GetOutlineIdentifier() );

        if( !PopulateVRML( vpcb, pout->GetOutlines(), bottom,
            board.GetUserScale(), 0, 0, 0 ) )
        {
            return false;
        }

        vpcb.EnsureWinding( 0, false );

        nvcont = vpcb.GetNContours() - 1;

        while( nvcont > 0 )
            vpcb.EnsureWinding( nvcont--, true );

        vpcb.Tesselate( NULL );

        if( bottom )
        {
            top = -thick;
            bot = ( top - pout->GetThickness() ) * scale;
            top *= scale;
        }
        else
        {
            bot = thick;
            top = (bot + pout->GetThickness() ) * scale;
            bot *= scale;
        }

        // note: this can happen because IDF allows some negative heights/thicknesses
        if( bot > top )
            std::swap( bot, top );

        vcp->bottom = bottom;
        WriteTriangles( file, vcp, &vpcb, false,
                        false, top, bot, board.GetUserPrecision(), false );

        vpcb.Clear();
        ++sc;
    }

    return true;
}

#endif  // GOOBLE0