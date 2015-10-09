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

// Format:
// (optional) unit: in/mm
// file: "quoted filename"
// (optional) orient: w, x, y, z, dX, dY, dZ;
// pos: zrot, flip (0,1), Xoffset, Yoffset, Zoffset;
// [more 'pos:'] lines if multiple instances are desired
// [more 'file:' + 'orient' + 'pos:' lines for additional parts and subassemblies]

#include <cmath>
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <dll_iges.h>
#include <mcad_helpers.h>
#include <all_api_entities.h>
#include <error_macros.h>

#ifdef STATIC_IGES
    #include <iges_io.h>
#else
    bool ParseLString( const std::string& data, int& idx, std::string& param,
                       bool& eor, char pd, char rd );

    bool ParseReal( const std::string& data, int& idx, double& param, bool& eor,
                    char pd, char rd, double* ddefault = NULL );

    bool ParseInt(const std::string& data, int& idx, int& param, bool& eor,
                  char pd, char rd, int* idefault = NULL);
#endif

using namespace std;

#define ONAME "test_out_merge.igs"

struct ORIENT
{
    double w;
    double x;
    double y;
    double z;

    double dX;
    double dY;
    double dZ;

    ORIENT()
    {
        w = 0.0;
        x = 0.0;
        y = 0.0;
        z = 1.0;
        dX = 0.0;
        dY = 0.0;
        dZ = 0.0;
    }
};

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

    void GetTransform( MCAD_TRANSFORM& T );
};

// merge the model with the given filename 'modelOut' and instantiate
// the new model with the given list of transforms
bool merge( DLL_IGES& modelOut, const std::string fname, list<TPARAMS>*pos, vector<pair<string, ORIENT > >& o );
// parse a line an update the model/placement data
void parseLine( std::vector< std::pair< std::string, std::list< TPARAMS >* > >& models,
                std::vector< std::pair< std::string, ORIENT > >& orients,
                const std::string& iline );
// parse a filename and add the result to the file list
void parseFile( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline );
// parse a position line and add the result to the list
void parsePos( vector<pair<string, list<TPARAMS>* > >& models, const std::string& iline );
// parse an orientation line and add the result to the list
void parseOrient( const std::string& fname, vector<pair<string, ORIENT > >& orients, const std::string& iline );
// parse a UNIT line
void parseUnit( const std::string& iline );
// create a rotation matrix around X
void rotateX( MCAD_MATRIX& mat, double angle );
// create a rotation matrix around Y
void rotateY( MCAD_MATRIX& mat, double angle );
// create a rotation matrix around Z
void rotateZ( MCAD_MATRIX& mat, double angle );


IGES_UNIT unit = UNIT_END;

string AUNIT[10] = {
    "in",
    "mm",
    "ft",
    "mi",
    "m",
    "km",
    "mil",
    "micron",
    "cm",
    "microinch"
};

IGES_UNIT IUNIT[10] = {
    UNIT_INCH,
    UNIT_MILLIMETER,
    UNIT_FOOT,
    UNIT_MILE,
    UNIT_METER,
    UNIT_KILOMETER,
    UNIT_MIL,
    UNIT_MICRON,
    UNIT_CENTIMETER,
    UNIT_MICROINCH
};


int main( int argc, char** argv )
{
    DLL_IGES modelOut;
    vector<pair<string, list<TPARAMS>* > > modelNames;
    vector<pair<string, ORIENT > > orients;

    if( argc != 2 )
    {
        cerr << "*** Invocation: mergetest inputFilename\n";
        cerr << "*** Sample input file:\n";
        cerr << "unit: mm\n";
        cerr << "file: \"modelA.igs\"\n";
        cerr << "orient: -90,1,0,0,0,0,1;\n";
        cerr << "pos: 0,0,0,0,0.8;\n";
        cerr << "pos: 0,0,10,10,0.8;\n";
        cerr << "file: \"modelB.igs\"\n";
        cerr << "pos: 90,1,10,10,0.8;\n";
        cerr << "\n\nParameters:\n";
        cerr << "unit: (optional) one of 'in' (inches) or 'mm'\n";
        cerr << "file: (required) name of the model to include in the assembly\n";
        cerr << "orient: (optional) transform data to put the model into its nominal (0,0,0) orientation\n";
        cerr << "        param 1: rotation (degrees)\n";
        cerr << "        param 2: x magnitude of rotation vector\n";
        cerr << "        param 3: y magnitude of rotation vector\n";
        cerr << "        param 4: z magnitude of rotation vector\n";
        cerr << "        param 5: x translation\n";
        cerr << "        param 6: y translation\n";
        cerr << "        param 7: z translation\n";
        cerr << "pos: (required) Z axis rotation and translation for each instance of the model\n";
        cerr << "     param 1: z rotation (degrees)\n";
        cerr << "     param 2: 0/1 = top side/bottom side\n";
        cerr << "     param 3: x translation\n";
        cerr << "     param 4: y translation\n";
        cerr << "     param 5: z translation\n\n";
        cerr << "note: orientation of a part on the bottom side is determined according to the\n";
        cerr << "      IDFv3 rules; the part is rotated along the Y axis and the z rotation is\n";
        cerr << "      in the reverse direction from what it would be if the part were on the top.\n\n";

        return -1;
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
            parseLine( modelNames, orients, iline );
    }
    if( modelNames.empty() )
    {
        cerr << "Nothing to do; no valid model/position data\n";
        return 0;
    }

    if( unit == UNIT_END )
        unit = UNIT_MILLIMETER;

    modelOut.SetUnitsFlag( unit );
    bool fail = false;

    for( size_t i = 0; i < modelNames.size(); ++i )
    {
        if( !merge( modelOut, modelNames[i].first, modelNames[i].second, orients ) )
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

    while( !modelNames.empty() )
    {
        delete modelNames.back().second;
        modelNames.pop_back();
    }

    return 0;
}


bool merge( DLL_IGES& modelOut, const std::string fname, list<TPARAMS>*pos, vector<pair<string, ORIENT > >& o )
{

    if( pos->empty() )
    {
        cerr << "[WARNING] no position data for file '" << fname << "'\n";
        return true;
    }

    DLL_IGES modelA;

    if( !modelA.Read( fname.c_str() ) )
    {
        cerr << "Could not load model '" << fname << "'\n";
        return false;
    }

    DLL_IGES_ENTITY* ep;
    MCAD_TRANSFORM* pO = NULL;

    // determine if there is an transform to associate with the basic model
    if( !o.empty() )
    {
        vector<pair<string, ORIENT > >::iterator sbeg = o.begin();
        vector<pair<string, ORIENT > >::iterator send = o.end();

        while( sbeg != send )
        {
            if( !sbeg->first.compare( fname ) )
            {
                ORIENT od = sbeg->second;
                pO = new MCAD_TRANSFORM;

                if( !pO )
                {
                    cout << "Could not instantiate a transform for '" << fname << "'\n";
                    return false;
                }

                // set the translation parameters
                pO->T.x = od.dX;
                pO->T.y = od.dY;
                pO->T.z = od.dZ;

                // set the rotation matrix;
                pO->R.v[0][0] = 1.0 - 2.0 * ( od.y*od.y + od.z*od.z );
                pO->R.v[0][1] = 2.0 * ( od.x*od.y - od.z*od.w );
                pO->R.v[0][2] = 2.0 * ( od.x*od.z + od.y*od.w );

                pO->R.v[1][0] = 2.0 * ( od.x*od.y + od.z*od.w );
                pO->R.v[1][1] = 1.0 - 2.0 * ( od.x*od.x + od.z*od.z );
                pO->R.v[1][2] = 2.0 * ( od.y*od.z - od.x*od.w );

                pO->R.v[2][0] = 2.0 * ( od.x*od.z - od.y*od.w );
                pO->R.v[2][1] = 2.0 * ( od.y*od.z + od.x*od.w );
                pO->R.v[2][2] = 1.0 - 2.0 * ( od.x*od.x + od.y*od.y );

                break;
            }

            ++sbeg;
        }
    }

    IGES_ENTITY_308* p308 = NULL;
    DLL_IGES_ENTITY_408* p408;
    DLL_IGES_ENTITY_124* p124;

    list<TPARAMS>::iterator sPos = pos->begin();
    list<TPARAMS>::iterator ePos = pos->end();

    while( sPos != ePos )
    {
        if( NULL == p308 )
        {
            if( !modelA.Export( &modelOut, &p308 ) || !p308 )
            {
                if( pO )
                    delete pO;

                cout << "Could not export model '" << fname << "'\n";
                return false;
            }
        }

        modelOut.NewAPIEntity( ENT_TRANSFORMATION_MATRIX, ep );
        p124 = (DLL_IGES_ENTITY_124*)ep;
        MCAD_TRANSFORM TX;
        MCAD_TRANSFORM* pTX;
        sPos->GetTransform( TX );

        if( pO )
            TX = TX * (*pO);

        p124->SetRootTransform( pTX );

        modelOut.NewAPIEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, ep );
        p408 = (DLL_IGES_ENTITY_408*)ep;
        p408->SetTransform( p124->GetRawPtr() );
        p408->SetSubfigure( p308 );

        ++sPos;
    }

    if( pO )
        delete pO;

    return true;
}

void TPARAMS::GetTransform( MCAD_TRANSFORM& T )
{
    /* Calculate 3D shape rotation:
     * this is the rotation parameters, with an additional 180 deg rotation
     * for footprints that are flipped
     * When flipped, axis rotation is the horizontal axis (X axis)
     */
    double rotx = 0.0;
    double rotz = zRot * M_PI / 180.0;

    if( flip )
    {
        rotx += M_PI;
        rotz += M_PI;
    }


    MCAD_MATRIX mx;
    MCAD_MATRIX mz;

    rotateX( mx, rotx );
    rotateZ( mz, rotz );

    T.R = mx * mz;

    if( flip )
        T.T = MCAD_POINT( xOff, yOff, -zOff );
    else
        T.T = MCAD_POINT( xOff, yOff, zOff );

    return;
}

#ifndef STATIC_IGES

bool ParseReal( const std::string& data, int& idx, double& param, bool& eor, char pd, char rd, double* ddefault )
{
    std::string tmp;
    int tidx = idx;

    if( !ParseLString( data, idx, tmp, eor, pd, rd ) )
    {
        ERRMSG << "[BAD DATA]\n";
        return false;
    }

    if( tmp.empty() )
    {
        if( ddefault )
        {
            param = *ddefault;
            return true;
        }

        ERRMSG << "\n + [BAD DATA]: empty field for non-default parameter\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    size_t dex = tmp.find_first_of( 'D' );

    if( dex != string::npos )
        tmp[dex] = 'E';

    const char* cp = tmp.c_str();
    char* rp;

    errno = 0;
    double d = strtod( cp, &rp );

    if( errno || cp == rp )
    {
        ERRMSG << "\n + [BAD DATA]: invalid floating point number\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    if( rp - cp != (int)tmp.length() )
    {
        ERRMSG << "\n + [WARNING]: extra characters at end of floating point number\n";
        cerr << "Float value: " << setprecision(12) << d << setprecision(0) << "\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
    }

    param = d;
    return true;
}

bool ParseInt( const std::string& data, int& idx, int& param, bool& eor, char pd, char rd, int* idefault )
{
    std::string tmp;
    int tidx = idx;

    if( !ParseLString( data, idx, tmp, eor, pd, rd ) )
    {
        ERRMSG << "[BAD DATA]\n";
        return false;
    }

    if( tmp.empty() )
    {
        if( idefault )
        {
            param = *idefault;
            return true;
        }

        ERRMSG << "\n + [BAD DATA]: empty field for non-default parameter\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    const char* cp = tmp.c_str();
    char* rp;

    errno = 0;
    int i = strtol( cp, &rp, 10 );

    if( errno || cp == rp )
    {
        ERRMSG << "\n + [BAD DATA]: invalid integer\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
        return false;
    }

    if( rp - cp != (int)tmp.length() )
    {
        ERRMSG << "\n + [WARNING]: extra characters at end of integer\n";
        cerr << "Integer value: " << i << "\n";
        cerr << "Data: " << data.substr(tidx) << "\n";
    }

    param = i;
    return true;
}

bool ParseLString( const std::string& data, int& idx, std::string& param, bool& eor, char pd, char rd )
{
    param.clear();
    int tidx = idx;

    if( idx >= (int)data.length() )
    {
        ERRMSG << "\n + [BUG] out of bounds\n";
        return false;
    }

    if( data[idx] == pd || data[idx] == rd )
    {
        if( data[idx] == rd )
            eor = true;

        ++idx;
        return true;
    }

    size_t strEnd = data.find_first_of( pd, idx );
    size_t strEnd2 = data.find_first_of( rd, idx );

    if( strEnd == string::npos || (strEnd2 != string::npos && strEnd > strEnd2) )
    {
        if( strEnd2 == string::npos )
        {
            ERRMSG << "\n + [BAD DATA] no Parameter or Record delimeter found in data\n";
            cerr << "Data: " << data.substr( idx ) << "\n";
            return false;
        }

        strEnd = strEnd2;
    }

    param = data.substr( idx, strEnd - idx );
    idx += (int)param.length();

    if( data[idx] == rd )
    {
        ++idx;
        eor = true;
        return true;
    }

    if( data[idx] == pd )
    {
        ++idx;
        return true;
    }

    ERRMSG << "\n + [BAD DATA]: invalid record; no Parameter or Record delimeter after string\n";
    cerr << "Data: " << data.substr(tidx) << "\n";
    return false;
}
#endif  // ifndef( STATIC_IGES )

void parseLine( vector<pair<string, list<TPARAMS>* > >& models,
                vector<pair<string, ORIENT > >& orients,
                const std::string& iline )
{
    if( iline.find("file:") != string::npos )
        parseFile( models, iline );
    else if( iline.find("pos:") != string::npos && !models.empty() )
        parsePos( models, iline );
    else if( iline.find("unit:") != string::npos && unit == UNIT_END )
        parseUnit( iline );
    else if( iline.find("orient:") != string::npos && !models.empty() )
        parseOrient( models.back().first, orients, iline );

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

// create a rotation matrix around X
void rotateX( MCAD_MATRIX& mat, double angle )
{
    double cosN = cos( angle );
    double sinN = sin( angle );
    mat.v[1][1] = cosN;
    mat.v[1][2] = -sinN;
    mat.v[2][1] = sinN;
    mat.v[2][2] = cosN;

    return;
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


// parse an orientation line, convert angle/direction to quaternion and add the result to the list
void parseOrient( const std::string& fname, vector<pair<string, ORIENT > >& orients, const std::string& iline )
{
    if( !orients.empty() && !fname.compare( orients.back().first ) )
    {
        cerr << "+ [WARNING]: multiple 'orient' lines for file '" << fname << "'\n";
        return;
    }

    int sp = int(iline.find_first_of( ':' )) + 1;

    double tR;
    double ang;
    bool eor = false;
    ORIENT arg;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    ang = M_PI * tR / 360.0;
    arg.w = cos( ang );
    double sang = sin( ang );

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    arg.x = tR;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    arg.y = tR;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    arg.z = tR;

    if( !CheckNormal( arg.x, arg.y, arg.z ) )
    {
        cerr << "Invalid orientation line (bad direction vector): '" << iline << "'\n";
        return;
    }

    arg.x *= sang;
    arg.y *= sang;
    arg.z *= sang;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    arg.dX = tR;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    arg.dY = tR;

    if( !ParseReal( iline, sp, tR, eor, ',', ';' ) )
    {
        cerr << "Invalid orientation line: '" << iline << "'\n";
        return;
    }

    arg.dZ = tR;
    orients.push_back(pair<string, ORIENT>(fname, arg) );

    return;
}

// parse a UNIT line
void parseUnit( const std::string& iline )
{
    size_t sp = int(iline.find_first_of( ':' )) + 1;
    size_t fp = iline.find_first_of( "imfkc", sp );

    if( fp == string::npos )
    {
        cerr << "+ [WARNING]: no unit data in '" << iline << "'\n";
        return;
    }

    size_t lp = iline.find_last_of( "nmtilh" );

    if( lp == string::npos || lp < fp )
    {
        cerr << "+ [WARNING]: no unit data in '" << iline << "'\n";
        return;
    }

    string aline = iline.substr( fp, lp - fp + 1 );

    for( int i = 0; i < 10; ++i )
    {
        if( !aline.compare( AUNIT[i] ) )
        {
            unit = IUNIT[i];
            return;
        }
    }

    return;
}
