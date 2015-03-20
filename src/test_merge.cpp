/*
 * file: test_merge.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: File merging utility for testing the libIGES
 * functions. Assembly models can be generated from multiple
 * IGES files using this utility and an input file with the
 * parameters as described below. If you encounter a part
 * which cannot be correctly merged into an assembly please
 * submit an issue report as described in the file README.testing.
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

// NOTE: Rotation appears to be incorrect; +Angle = Clockwise rather than CCW

// Format:
// file: "quoted filename"
// pos: zrot, flip (0,1), Xoffset, Yoffset, Zoffset;
// [more 'pos:'] lines if multiple instances are desired
// [more 'file:' + 'pos:' lines for additional parts and subassemblies]

#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <iges.h>
#include <iges_io.h>
#include <iges_helpers.h>
#include <iges_elements.h>
#include "all_entities.h"

using namespace std;

#define ONAME "test_out_merge.igs"


struct TPARAMS
{
    double zRot;
    bool   flip;
    double xOff;
    double yOff;
    double zOff;

    TPARAMS()
    {
        zRot = 0.0;
        flip = false;
        xOff = 0.0;
        yOff = 0.0;
        zOff = 0.0;
    }

    void GetTransform( IGES_TRANSFORM& T );
};

// merge the model with the given filename 'modelOut' and instantiate
// the new model with the given list of transforms
bool merge( IGES& modelOut, const std::string fname, list<TPARAMS>*pos );

// parse a line an update the model/placement data
void parseLine( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline );
// parse a filename and add the result to the file list
void parseFile( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline );
// parse a position line and add the result to the list
void parsePos( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline );

int main( int argc, char** argv )
{
    IGES modelOut;
    modelOut.globalData.unitsFlag = UNIT_MILLIMETER;
    vector<pair<string, list<TPARAMS>* > > modelNames;

    if( argc != 2 )
    {
        cerr << "*** Invocation: mergetest inputFilename\n";
        cerr << "*** Sample input file:\n";
        cerr << "file: \"modelA.igs\"\n";
        cerr << "pos: 0,0,0,0,0.8;\n";
        cerr << "pos: 0,0,10,10,0.8;\n";
        cerr << "file: \"modelB.igs\"\n";
        cerr << "pos: 90,1,10,10,0.8;\n";
    }

    ifstream ifile;
    ifile.open( argv[1] );

    if( !ifile.is_open() )
    {
        cerr << "Could not open input file: '" << argv[1] << "'\n";
        return -1;
    }

    string iline;
    while( !ifile.eof() && ifile.good() )
    {
        iline.clear();
        getline( ifile, iline );

        if( !iline.empty() )
            parseLine( modelNames, iline );
    }

    if( modelNames.empty() )
    {
        cerr << "Nothing to do; no valid model/position data\n";
        return 0;
    }

    bool fail = false;

    for( size_t i = 0; i < modelNames.size(); ++i )
    {
        if( !merge( modelOut, modelNames[i].first, modelNames[i].second ) )
        {
            fail = true;
            break;
        }
    }

    if( !fail )
    {
        modelOut.Cull();
        modelOut.Write( ONAME, true );
    }

    return 0;
}


bool merge( IGES& modelOut, const std::string fname, list<TPARAMS>*pos )
{

    if( pos->empty() )
    {
        cerr << "[WARNING] no position data for file '" << fname << "'\n";
        return true;
    }

    IGES modelA;

    if( !modelA.Read( fname.c_str() ) )
    {
        cerr << "Could not load model '" << fname << "'\n";
        return false;
    }

    IGES_ENTITY* ep;
    IGES_ENTITY_308* p308 = NULL;
    IGES_ENTITY_408* p408;
    IGES_ENTITY_124* p124;

    list<TPARAMS>::iterator sPos = pos->begin();
    list<TPARAMS>::iterator ePos = pos->end();

    while( sPos != ePos )
    {
        if( NULL == p308 )
        {
            if( !modelA.Export( &modelOut, &p308 ) || !p308 )
            {
                cout << "Could not export model '" << fname << "'\n";
                return false;
            }
        }

        modelOut.NewEntity( ENT_TRANSFORMATION_MATRIX, &ep );
        p124 = (IGES_ENTITY_124*)ep;

        sPos->GetTransform( p124->T );

        modelOut.NewEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, &ep );
        p408 = (IGES_ENTITY_408*)ep;
        p408->SetTransform( p124 );
        p408->SetDE( p308 );

        ++sPos;
    }

    return true;
}

void TPARAMS::GetTransform( IGES_TRANSFORM& T )
{
    /* Calculate 3D shape rotation:
     * this is the rotation parameters, with an additional 180 deg rotation
     * for footprints that are flipped
     * When flipped, axis rotation is the horizontal axis (X axis)
     */
    double rotx = 0.0;
    double roty = 0.0;
    double rotz = zRot * M_PI / 180.0;

    if( flip )
    {
        rotx += M_PI;
        roty = -roty;
        rotz = -rotz;
    }

    IGES_MATRIX mx;
    IGES_MATRIX my;
    IGES_MATRIX mz;

    // rotation about x:
    double cosN = cos( rotx );
    double sinN = sin( rotx );
    mx.v[1][1] = cosN;
    mx.v[1][2] = -sinN;
    mx.v[2][1] = sinN;
    mx.v[2][2] = cosN;

    // rotation about y:
    cosN = cos( roty );
    sinN = sin( roty );
    my.v[0][0] = cosN;
    my.v[0][2] = sinN;
    my.v[2][0] = -sinN;
    my.v[2][2] = cosN;

    // rotation about z:
    cosN = cos( rotz );
    sinN = sin( rotz );
    mz.v[0][0] = cosN;
    mz.v[0][1] = -sinN;
    mz.v[1][0] = sinN;
    mz.v[1][1] = cosN;

    T.R = mx * my * mz;

    if( flip )
        T.T = IGES_POINT( xOff, yOff, -zOff );
    else
        T.T = IGES_POINT( xOff, yOff, zOff );

    return;
}

void parseLine( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline )
{
    if( iline.find("file:") != string::npos )
        parseFile( models, iline );
    else if( iline.find("pos:") != string::npos && !models.empty() )
        parsePos( models, iline );

    return;
}

void parseFile( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline )
{
    size_t sp = iline.find_first_of( '"' );
    size_t ep = iline.find_last_of( '"' );

    if( sp == string::npos || ep == sp )
        cerr << "[ERROR] filename is not quoted\n";

    if( sp + 6 > ep )
        cerr << "[ERROR] no valid filename\n";

    ++sp;
    string fname = iline.substr( sp, ep - sp );

    list<TPARAMS>* pp = new list<TPARAMS>;

    models.push_back(pair<string, list<TPARAMS>* >(fname, pp) );
}

// parse a position line and add the result to the list
void parsePos( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline )
{
    int sp = int(iline.find_first_of( ':' )) + 1;

    int tI;
    double tR;
    bool eor = false;

    TPARAMS arg;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid position line: '" << iline << "'\n";
        return;
    }

    arg.zRot = tR;

    if( !ParseInt( iline, sp, tI, eor, ',', ';' ) )
    {
        cerr << "Invalid position line: '" << iline << "'\n";
        return;
    }

    if( 0 == tI )
        arg.flip = false;
    else
        arg.flip = true;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid position line: '" << iline << "'\n";
        return;
    }

    arg.xOff = tR;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid position line: '" << iline << "'\n";
        return;
    }

    arg.yOff = tR;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid position line: '" << iline << "'\n";
        return;
    }

    arg.zOff = tR;

    models.back().second->push_back( arg );

    return;
}
