# Project: libIGES (IGESv6)
#
project( libIGES CXX C )

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

set( CMAKE_VERBOSE_MAKEFILE ON )

# Version information
set( LIBIGES_VERSION_MAJOR 0 )
set( LIBIGES_VERSION_MINOR 1 )

include( ExternalProject )
set( SISL_ROOT "${PROJECT_SOURCE_DIR}/sisl" )
set( SISL_INCLUDE_DIR "${SISL_ROOT}/include" )
set( SISL_LIB_DIR "${SISL_ROOT}" )

ExternalProject_Add( sisl
    SOURCE_DIR      ${SISL_ROOT}
    GIT_SUBMODULES "sisl"
    TIMEOUT         1200            # 20 minutes

    PATCH_COMMAND git checkout CMakeLists.txt
        COMMAND patch < ${PROJECT_SOURCE_DIR}/patches/sisl.patch

    BUILD_IN_SOURCE 1

    BUILD_COMMAND   make
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
    )

find_package( Boost 1.55 REQUIRED COMPONENTS filesystem system )

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
    "${Boost_INCLUDE_DIRS}"
    "${LIBIGES_BINARY_DIR}"
    "${LIBIGES_SOURCE_DIR}/include"
    "${LIBIGES_SOURCE_DIR}/include/iges"
    "${LIBIGES_SOURCE_DIR}/include/geom"
    "${SISL_INCLUDE_DIR}"
    )

set( CMAKE_CXX_FLAGS "-g -Wall -DDEBUG" )

set( SRC_ENT "${CMAKE_CURRENT_SOURCE_DIR}/entities" )
set( SRC_IGS "${CMAKE_CURRENT_SOURCE_DIR}/iges" )
set( SRC_GEOM "${CMAKE_CURRENT_SOURCE_DIR}/geom" )

add_library( iges STATIC
    "${SRC_ENT}/iges_entity.cpp"
    "${SRC_ENT}/iges_curve.cpp"
    "${SRC_ENT}/entityNULL.cpp"
    "${SRC_ENT}/entity100.cpp"
    "${SRC_ENT}/entity102.cpp"
    "${SRC_ENT}/entity104.cpp"
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
    "${SRC_ENT}/entity186.cpp"
    "${SRC_ENT}/entity308.cpp"
    "${SRC_ENT}/entity314.cpp"
    "${SRC_ENT}/entity408.cpp"
    "${SRC_ENT}/entity502.cpp"
    "${SRC_ENT}/entity504.cpp"
    "${SRC_ENT}/entity508.cpp"
    "${SRC_ENT}/entity510.cpp"
    "${SRC_ENT}/entity514.cpp"
    "${SRC_IGS}/iges_io.cpp"
    "${SRC_IGS}/iges.cpp"
    "${SRC_GEOM}/mcad_elements.cpp"
    "${SRC_GEOM}/mcad_helpers.cpp"
    "${SRC_GEOM}/geom_wall.cpp"
    "${SRC_GEOM}/geom_cylinder.cpp"
    "${SRC_GEOM}/iges_geom_pcb.cpp"
    "${SRC_GEOM}/mcad_segment.cpp"
    "${SRC_GEOM}/mcad_outline.cpp"
    )

add_dependencies( iges sisl )
target_link_libraries( iges "${SISL_LIB_DIR}/libsisl.so" "${Boost_LIBRARIES}" )

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

add_executable( olntest
    "${LIBIGES_SOURCE_DIR}/tests/test_outline.cpp"
    )

add_executable( planetest
    "${LIBIGES_SOURCE_DIR}/tests/test_plane.cpp"
    )

target_link_libraries( readtest iges )
target_link_libraries( mergetest iges )
target_link_libraries( curvetest iges )
target_link_libraries( segtest iges )
target_link_libraries( olntest iges )
target_link_libraries( planetest iges )

# build the idf2igs tool
add_subdirectory( idf )
