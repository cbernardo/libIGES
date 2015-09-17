/*
 * file: entity180.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 180: Boolean Tree, Section 4.46, p.209(237+)
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


#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity124.h>
#include <entity180.h>

using namespace std;

IGES_ENTITY_180::IGES_ENTITY_180( IGES* aParent ) : IGES_ENTITY( aParent )
{
    entityType = 180;
    form = 0;
    return;
}


IGES_ENTITY_180::~IGES_ENTITY_180()
{
    ClearNodes();
    return;
}


bool IGES_ENTITY_180::typeOK( int aTypeNum )
{
    // ALLOWED ENTITIES:
    // A. Primitives (CSG primitives)
    //      150 Block
    //      152 Right Angular Wedge
    //      154 Right Circular Cylinder
    //      156 Right Circular Cone Frustum
    //      158 Sphere
    //      160 Torus
    //      162 Solid of Revolution
    //      164 Solid of Linear Extrusion
    //      168 Ellipsoid
    // B. Binary Tree (180)
    // C. Solid Instance (430)
    // D. Manifold Solid BREP (186)

    if( aTypeNum == ENT_BLOCK
        || aTypeNum == ENT_RIGHT_ANGULAR_WEDGE
        || aTypeNum == ENT_RIGHT_CIRCULAR_CYLINDER
        || aTypeNum == ENT_RIGHT_CIRCULAR_CONE_FRUSTUM
        || aTypeNum == ENT_SPHERE
        || aTypeNum == ENT_TORUS
        || aTypeNum == ENT_SOLID_OF_REVOLUTION
        || aTypeNum == ENT_SOLID_OF_LINEAR_EXTRUSION
        || aTypeNum == ENT_ELLIPSOID
        || aTypeNum == ENT_BOOLEAN_TREE
        || aTypeNum == ENT_SOLID_INSTANCE
        || aTypeNum == ENT_MANIFOLD_SOLID_BREP )
    {
        return true;
    }

    return false;
}


void IGES_ENTITY_180::ClearNodes( void )
{
    if( !nodes.empty() )
    {
        std::list<BTREE_NODE*>::iterator rbeg = nodes.begin();
        std::list<BTREE_NODE*>::iterator rend = nodes.end();

        while( rbeg != rend )
        {
            if( !(*rbeg)->op )
            {
                IGES_ENTITY* ip = (*rbeg)->pEnt;

                if( ip && !ip->delReference(this) )
                {
                    ERRMSG << "\n + [BUG] could not delete reference from a child entity\n";
                }
            }

            delete *rbeg;
            ++rbeg;
        }

        nodes.clear();
    }
}


bool IGES_ENTITY_180::associate(std::vector<IGES_ENTITY *> *entities)
{
    if( !IGES_ENTITY::associate(entities) )
    {
        ERRMSG << "\n + [INFO] could not establish associations\n";
        return false;
    }

    structure = 0;

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->delReference(this);
        pStructure = NULL;
    }

    std::list<BTREE_NODE*>::iterator sn = nodes.begin();
    std::list<BTREE_NODE*>::iterator en = nodes.end();
    int sEnt = (int)entities->size();
    int iEnt;
    int tEnt;
    bool dup = false;

    while( sn != en )
    {
        if( !(*sn)->op )
        {
            iEnt = (*sn)->val >> 1;

            if( iEnt >= 0 && iEnt < sEnt )
            {
                // check that the entity type can be accepted in this list
                tEnt = (*entities)[iEnt]->GetEntityType();

                if( !typeOK( tEnt ) )
                {
                    ERRMSG << "\n + [BAD FILE] invalid entity type (" << tEnt << ")\n";
                    return false;
                }

                (*sn)->pEnt = (*entities)[iEnt];

                if( !(*entities)[iEnt]->addReference(this, dup) )
                {
                    ERRMSG << "\n + [INFO] unable to add reference to child entity\n";
                    return false;
                }

                if( dup )
                {
                    ERRMSG << "\n + [CORRUPT FILE]: adding duplicate entry\n";
                    return false;
                }

            }
            else
            {
                ERRMSG << "\n + [INFO] invalid DE sequence for child entity (";
                cerr << (*sn)->val << ")\n";
                return false;
            }
        }

        ++sn;
    }

    return true;
}


bool IGES_ENTITY_180::format( int &index )
{
    pdout.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    parameterData = index;
    int nNodes = (int)nodes.size();

    if( nNodes < 3 )
    {
        ERRMSG << "\n + [ERROR] too few nodes (<3)\n";
        return false;
    }

    std::string tstr;   // individual item to add to output
    std::string lstr;   // single line for output

    if( !parent )
    {
        ERRMSG << "\n + [INFO] method invoked with no parent IGES object\n";
        return false;
    }

    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << nNodes << pd;
    lstr = ostr.str();

    std::list<BTREE_NODE*>::iterator sNode = nodes.begin();
    std::list<BTREE_NODE*>::iterator eNode = nodes.end();

    int nOp = 0;
    int nArg = 0;

    while( sNode != eNode )
    {
        ostr.str("");

        if( (*sNode)->op )
        {
            ostr << (*sNode)->val;
            ++nOp;
        }
        else
        {
            if( !(*sNode)->pEnt )
            {
                ERRMSG << "\n + [BUG] invalid (NULL) pointer to child entity\n";
                return false;
            }

            ostr << (-(*sNode)->pEnt->getDESequence());
            ++nArg;
        }

        ++sNode;

        if( sNode == eNode && extras.empty() )
            ostr << rd;
        else
            ostr << pd;

        tstr = ostr.str();

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    if( (nArg -1) != nOp )
    {
        ERRMSG << "\n + [ERROR] #arguments -1 != #operators\n";
        return false;
    }

    if( !extras.empty() && !formatExtraParams( lstr, index, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] could not format optional parameters\n";
        pdout.clear();
        iExtras.clear();
        return false;
    }

    if( !formatComments( index ) )
    {
        ERRMSG << "\n + [INFO] could not format comments\n";
        pdout.clear();
        return false;
    }

    paramLineCount = index - parameterData;

    return true;
}


bool IGES_ENTITY_180::rescale( double sf )
{
    // there is nothing to scale
    return true;
}


bool IGES_ENTITY_180::unlink(IGES_ENTITY *aChildEntity)
{
    if(IGES_ENTITY::unlink(aChildEntity) )
        return true;

    // if one node is unlinked then we must relinquish
    // links to all entities

    std::list<BTREE_NODE*>::iterator rbeg = nodes.begin();
    std::list<BTREE_NODE*>::iterator rend = nodes.end();
    bool clear_all = false;

    while( rbeg != rend )
    {
        IGES_ENTITY* ip = (*rbeg)->pEnt;

        if( aChildEntity == ip )
        {
            clear_all = true;
            nodes.erase( rbeg );
            break;
        }

        ++rbeg;
    }

    if( clear_all )
        nodes.clear();

    return true;
}


bool IGES_ENTITY_180::isOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_180::addReference(IGES_ENTITY *aParentEntity, bool &isDuplicate)
{
    return IGES_ENTITY::addReference(aParentEntity, isDuplicate);
}


bool IGES_ENTITY_180::delReference(IGES_ENTITY *aParentEntity)
{
    return IGES_ENTITY::delReference(aParentEntity);
}


bool IGES_ENTITY_180::readDE(IGES_RECORD *aRecord, std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readDE(aRecord, aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    use = STAT_USE_GEOMETRY;        // fixed

    if( form != 0 && form != 1 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (";
        cerr << form << ") in Binary Tree\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_180::readPD(std::ifstream &aFile, int &aSequenceVar)
{
    if( !IGES_ENTITY::readPD(aFile, aSequenceVar) )
    {
        ERRMSG << "\n + [INFO] could not read data for Binary Trees Entity\n";
        pdout.clear();
        return false;
    }

    if( !nodes.empty() )
    {
        ERRMSG << "\n + [INFO] the Binary Tree Entity currently contains data\n";
        pdout.clear();
        return false;
    }

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

	idx = (int)pdout.find(pd);

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    int nNodes;

    if( !ParseInt( pdout, idx, nNodes, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read the number of nodes in the Binary Tree\n";
        pdout.clear();
        return false;
    }

    if( nNodes < 3 )
    {
        ERRMSG << "\n + [VIOLATION] number of nodes on the stack (";
        cerr << nNodes << ") is < 3)\n";
        pdout.clear();
        return false;
    }

    if( !(nNodes & 1) )
    {
        ERRMSG << "\n + [BAD FILE] invalid (even) number of nodes on the stack (";
        cerr << nNodes << ")\n";
        pdout.clear();
        return false;
    }

    BTREE_NODE* node = NULL;
    int ent;
    int nOp = 0;
    int nArg = 0;   // for a good file. nArg = nOp + 1

    for( int i = 0; i < nNodes; ++i )
    {
        if( !ParseInt( pdout, idx, ent, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read the entity DE index or operation code\n";
            pdout.clear();
            return false;
        }

        if( ent == 0 || ent > 3 || ent < -9999997
            || ( ent < 0 && (ent & 1) ) )
        {
            ERRMSG << "\n + [INFO] invalid value (" << ent << ")\n";
            pdout.clear();
            return false;
        }

        node = new BTREE_NODE;

        if( !node )
        {
            ERRMSG << "\n + [INFO] memory allocation failed\n";
            pdout.clear();
            return false;
        }

        if( ent > 0 )
        {
            node->op = true;
            node->val = ent;
            ++nOp;
        }
        else
        {
            ++nArg;
            node->val = -ent;
        }

        nodes.push_back( node );
    }

    if( (nArg -1) != nOp )
    {
        ERRMSG << "\n + [BAD FILE] #arguments -1 != #operations (";
        cerr << nArg << " vs " << nOp << ")\n";
        pdout.clear();
        return false;
    }

    if( !eor && !readExtraParams( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read optional pointers\n";
        pdout.clear();
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
        pdout.clear();
        return false;
    }

    pdout.clear();
    return true;
}


bool IGES_ENTITY_180::SetEntityForm( int aForm )
{
    if( aForm != 0 && aForm != 1 )
    {
        ERRMSG << "\n + [BUG] invalid form (";
        cerr << aForm << ") passed to Binary Tree\n";
        return false;
    }

    // note: a user can never change the form back to 0 but
    // ideally the object will determine its true form as
    // it executes the format() method
    if( aForm == 0 && form == 1 )
    {
        ERRMSG << "\n + [INFO] Form 1 cannot be manually changed to Form 0\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_180::SetDependency( IGES_STAT_DEPENDS aDependency )
{
    return IGES_ENTITY::SetDependency( aDependency );
}


bool IGES_ENTITY_180::SetEntityUse( IGES_STAT_USE aUseCase )
{
    if( aUseCase != STAT_USE_GEOMETRY )
    {
        ERRMSG << "\n + [BUG] invalid Use Case (";
        cerr << aUseCase << ") passed to Binary Tree\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_180::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    return IGES_ENTITY::SetHierarchy( aHierarchy );
}


bool IGES_ENTITY_180::AddOp( BTREE_OPERATOR op )
{
    if( op < OP_START || op >= OP_END )
    {
        ERRMSG << "\n + [BUG] invalid OPERATOR (" << op << ")\n";
        return false;
    }

    if( nodes.size() < 2 )
    {
        ERRMSG << "\n + [BUG] the first 2 items on the stack may not be operators\n";
        return false;
    }

    BTREE_NODE* np = new BTREE_NODE;

    if( !np )
    {
        ERRMSG << "\n + [BUG] memory allocation failed\n";
        return false;
    }

    np->op = true;
    np->val = op;
    nodes.push_back( np );

    return true;
}


bool IGES_ENTITY_180::AddArg( IGES_ENTITY* aOperand )
{
    int iEnt = aOperand->GetEntityType();

    if( !typeOK( iEnt ) )
    {
        ERRMSG << "\n + [BUG] invalid entity type (" << iEnt << ")\n";
        return false;
    }

    bool dup = false;

    if( !aOperand->addReference(this, dup) )
    {
        ERRMSG << "\n + [ERROR] could not add reference to child entity\n";
        return false;
    }

    if( dup )
    {
        ERRMSG << "\n + [BUG]: adding duplicate entry\n";
        return false;
    }

    BTREE_NODE* np = new BTREE_NODE;

    if( !np )
    {
        ERRMSG << "\n + [BUG] memory allocation failed\n";
        aOperand->delReference(this);
        return false;
    }

    np->pEnt = aOperand;
    nodes.push_back( np );

    return true;
}


int IGES_ENTITY_180::GetNNodes( void )
{
    return (int)nodes.size();
}


std::list<BTREE_NODE*>* IGES_ENTITY_180::GetNodes( void )
{
    return &nodes;
}
