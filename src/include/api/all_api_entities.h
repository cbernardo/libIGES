/*
 * file: all_api_entities.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES top level object for input, output and manipulation
 * of IGES entity data via a DLL interface
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


#ifndef ALL_API_ENTITIES_H
#define ALL_API_ENTITIES_H

// declare the core IGES classes
class IGES_ENTITY;
class IGES_ENTITY_100;
class IGES_ENTITY_102;
class IGES_ENTITY_104;
class IGES_ENTITY_110;
class IGES_ENTITY_120;
class IGES_ENTITY_122;
class IGES_ENTITY_124;
class IGES_ENTITY_126;
class IGES_ENTITY_128;
class IGES_ENTITY_142;
class IGES_ENTITY_144;
class IGES_ENTITY_308;
class IGES_ENTITY_314;
class IGES_ENTITY_408;

#include <dll_entity100.h>
#include <dll_entity102.h>
#include <dll_entity104.h>
#include <dll_entity110.h>
#include <dll_entity120.h>
#include <dll_entity122.h>
#include <dll_entity124.h>
#include <dll_entity126.h>
#include <dll_entity128.h>
#include <dll_entity142.h>
#include <dll_entity144.h>
/*
#include <dll_entity154.h>
#include <dll_entity164.h>
#include <dll_entity180.h>
#include <dll_entity186.h>
*/
#include <dll_entity308.h>
#include <dll_entity314.h>
#include <dll_entity408.h>
/*
#include <dll_entity502.h>
#include <dll_entity504.h>
#include <dll_entity508.h>
#include <dll_entity510.h>
#include <dll_entity514.h>
*/

#endif  // ALL_API_ENTITIES_H
