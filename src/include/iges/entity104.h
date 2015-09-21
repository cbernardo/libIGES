/*
 * file: entity104.h
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
 * Description: IGES Entity 104: Conic Arc, Section 4.5, p.74+ (102+)
 */

#ifndef ENTITY_104_H
#define ENTITY_104_H

#include <libigesconf.h>
#include <iges_curve.h>

// NOTE:
//
// A Conic Arc, like a Circle, is constrained to a plane parallel to Xt-Yt.
// The curve is a subset of the general equation:
// AX^2 + BXY + CY^2 + DX + EY + F = 0
// The axis of the conic must be parallel to Xt or Yt; any other orientation
// is achieved via the Transformation Matrix. Degenerate forms such as
// Point and Line are prohibited. Form 0, allowed in previous versions of
// the specification, is deprecated.
//
// The associated parameter data are:
// + A: Real: Coefficient A
// + B: Real: Coefficient A
// + C: Real: Coefficient A
// + D: Real: Coefficient A
// + E: Real: Coefficient A
// + F: Real: Coefficient A
// + ZT: Real: Zt displacement of the arc from the Xt-Yt plane
// + X1: Real: Start point, abscissa
// + Y1: Real: Start point, ordinate
// + X2: Real: End point, abscissa
// + Y2: Real: End point, ordinate
//
// Forms: (note: Form 0 does not exist)
//  1: ellipse
//  2: hyperbola
//  3: parabola
//
// Unused DE items:
// + Structure
//


/**
 * Class IGES_ENTITY_104
 * represents a general parameterized conic section
 */
class IGES_ENTITY_104 : public IGES_CURVE
{
private:
    int getForm();
    bool getPtEllipse( MCAD_POINT& pt0, double var );
    bool getPtHyperbola( MCAD_POINT& pt0, double var );
    bool getPtParabola( MCAD_POINT& pt0, double var );

protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

public:
    // public functions for libIGES only
    virtual bool associate(std::vector<IGES_ENTITY *> *entities);
    virtual bool unlink(IGES_ENTITY *aChild);
    virtual bool isOrphaned( void );
    virtual bool addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate);
    virtual bool delReference(IGES_ENTITY *aParentEntity);
    virtual bool readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar);
    virtual bool readPD(std::ifstream &aFile, int &aSequenceVar);

public:
    MCAD_API IGES_ENTITY_104( IGES* aParent );
    virtual MCAD_API ~IGES_ENTITY_104();

    // public variables
    double MCAD_API A;   //< 1st general conic parameter
    double MCAD_API B;   //< 2nd general conic parameter
    double MCAD_API C;   //< 3rd general conic parameter
    double MCAD_API D;   //< 4th general conic parameter
    double MCAD_API E;   //< 5th general conic parameter
    double MCAD_API F;   //< 6th general conic parameter
    double MCAD_API ZT;  //< Z coordinate of plane in which conic is defined
    double MCAD_API X1;  //< X value, start point, abscissa
    double MCAD_API Y1;  //< Y value, start point, abscissa
    double MCAD_API X2;  //< X value, terminal point, abscissa
    double MCAD_API Y2;  //< Y value, terminal point, abscissa

    // Inherited from IGES_ENTITY
    virtual MCAD_API bool SetEntityForm(int aForm);
    virtual MCAD_API bool SetHierarchy( IGES_STAT_HIER aHierarchy );

    // Inherited from IGES_CURVE
    virtual MCAD_API bool GetStartPoint( MCAD_POINT& pt, bool xform = true );
    virtual MCAD_API bool GetEndPoint( MCAD_POINT& pt, bool xform = true );
    virtual MCAD_API int GetNSegments( void );
    virtual MCAD_API bool IsClosed( void );
    virtual MCAD_API int GetNCurves( void );
    virtual MCAD_API IGES_CURVE* GetCurve( int index );
    virtual MCAD_API bool Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform = true );
};

#endif  // ENTITY_104_H
