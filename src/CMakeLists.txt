# Project: libIGES (IGESv6)
#
project( libIGES CXX C )

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

# Version information
set( LIBIGES_VERSION_MAJOR 0 )

set( LIBIGES_VERSION_MINOR 1 )

option( USE_SISL "Use the SINTEF SISL library; note license restrictions on cloud services" OFF )

if( NOT DEFINED SISL_INCLUDE_DIR )
message( FATAL_ERROR "SISL_INCLUDE_DIR is not defined" )
endif()

if( NOT EXISTS "${SISL_INCLUDE_DIR}/sisl.h" )
message( FATAL_ERROR "sisl.h was not found in '${SISL_INCLUDE_DIR}'" )
endif()

if( NOT DEFINED SISL_LIB_DIR )
message( FATAL_ERROR "SISL_LIB_DIR is not defined" )
endif()

if( NOT EXISTS "${SISL_LIB_DIR}/libsisl.so" )
message( FATAL_ERROR "libsisl was not found in '${SISL_LIB_DIR}'" )
endif()

set( LIBIGES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
set( LIBIGES_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    set( LIBIGES_LIBDIR RUNTIME DESTINATION bin )
else()
    set( LIBIGES_LIBDIR LIBRARY DESTINATION lib/iges )
endif()

# create a config file
configure_file(
    "${LIBIGES_SOURCE_DIR}/libigesconf.h.in"
    "${LIBIGES_BINARY_DIR}/libigesconf.h"
    )

# ensure that the newly created k3dconfig.h can be found
include_directories(
    "${LIBIGES_BINARY_DIR}"
    "${LIBIGES_SOURCE_DIR}/include"
    "${LIBIGES_SOURCE_DIR}/include/iges"
    "${LIBIGES_SOURCE_DIR}/include/geom"
    "${SISL_INCLUDE_DIR}"
    )

#set( CMAKE_CXX_FLAGS "-O2 -Wall -DDEBUG -lm -lboost_filesystem -lboost_system" )
set( CMAKE_CXX_FLAGS "-g -Wall -DDEBUG -lm -lboost_filesystem -lboost_system" )

# pull in the sources
#add_subdirectory ( entities )
#add_subdirectory ( iges )

set( SRC_ENT "${CMAKE_CURRENT_SOURCE_DIR}/entities" )
set( SRC_IGS "${CMAKE_CURRENT_SOURCE_DIR}/iges" )
set( SRC_GEOM "${CMAKE_CURRENT_SOURCE_DIR}/geom" )

add_library( iges STATIC
    "${SRC_ENT}/iges_entity.cpp"
    "${SRC_ENT}/iges_curve.cpp"
    "${SRC_ENT}/entityNULL.cpp"
    "${SRC_ENT}/entity100.cpp"
    "${SRC_ENT}/entity102.cpp"
    "${SRC_ENT}/entity110.cpp"
    "${SRC_ENT}/entity120.cpp"
    "${SRC_ENT}/entity122.cpp"
    "${SRC_ENT}/entity124.cpp"
    "${SRC_ENT}/entity126.cpp"
    "${SRC_ENT}/entity128.cpp"
    "${SRC_ENT}/entity142.cpp"
    "${SRC_ENT}/entity144.cpp"
    "${SRC_ENT}/entity154.cpp"
    "${SRC_ENT}/entity164.cpp"
    "${SRC_ENT}/entity180.cpp"
    "${SRC_ENT}/entity308.cpp"
    "${SRC_ENT}/entity314.cpp"
    "${SRC_ENT}/entity408.cpp"
    "${SRC_IGS}/iges_io.cpp"
    "${SRC_IGS}/iges.cpp"
    "${SRC_IGS}/iges_elements.cpp"
    "${SRC_IGS}/iges_helpers.cpp"
    "${SRC_GEOM}/geom_wall.cpp"
    "${SRC_GEOM}/geom_cylinder.cpp"
    "${SRC_GEOM}/geom_plane.cpp"
    "${SRC_GEOM}/geom_segment.cpp"
    "${SRC_GEOM}/geom_outline.cpp"
    )

target_link_libraries( iges "${SISL_LIB_DIR}/libsisl.so" )

add_executable( readtest
    "${LIBIGES_SOURCE_DIR}/tests/test_read.cpp"
    )

add_executable( mergetest
    "${LIBIGES_SOURCE_DIR}/tests/test_merge.cpp"
    )

add_executable( curvetest
    "${LIBIGES_SOURCE_DIR}/tests/test_curves.cpp"
    )

add_executable( segtest
    "${LIBIGES_SOURCE_DIR}/tests/test_segs.cpp"
    )

target_link_libraries( readtest iges )
target_link_libraries( mergetest iges )
target_link_libraries( curvetest iges )
target_link_libraries( segtest iges )
