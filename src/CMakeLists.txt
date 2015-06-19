# Project: libIGES (IGESv6)
#
project( libIGES CXX C )

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

set( CMAKE_VERBOSE_MAKEFILE ON )

# Version information
set( LIBIGES_VERSION_MAJOR 0 )
set( LIBIGES_VERSION_MINOR 1 )

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules )

#NOTE: the SISL_LIBRARIES path has only been checked on Linux;
#I don't know if it will work on other platforms.
if( NOT SISL_INCLUDE_DIR OR NOT SISL_LIBRARIES )
    find_package( SISL )

    if( NOT SISL_FOUND )
        message( WARNING "SISL could not be found; attempting to build from git submodule" )

        include( ExternalProject )
        set( SISL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/sisl" )
        find_file( SISL_INCLUDE_DIR sisl.h PATHS "${SISL_SOURCE_DIR}/include" NO_DEFAULT_PATH )

        if( ${SISL_INCLUDE_DIR} MATCHES SISL_INCLUDE_DIR-NOTFOUND )
            execute_process( COMMAND git submodule init
                COMMAND git submodule update
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                RESULT_VARIABLE SISL_SRC_OK
                )

            if( NOT ${SISL_SRC_OK} EQUAL 0 )
                message( FATAL_ERROR "Could not initialize/update SISL source" )
            endif()

            set( SISL_INCLUDE_DIR "${SISL_SOURCE_DIR}/include" CACHE PATH "SISL includes" FORCE )

        endif()

        if( CMAKE_C_COMPILER_ID MATCHES "MSVC" )
            ExternalProject_Add( sisl_submod
                PREFIX sisl_submod
                SOURCE_DIR  .
                BINARY_DIR sisl_submod
                BUILD_IN_SOURCE 0
                CONFIGURE_COMMAND cmake -G "${CMAKE_GENERATOR}" -DBUILD_SHARED_LIBS:BOOL=TRUE -DCMAKE_SHARED_LINKER_FLAGS="/DEF:../../sisl_exports.def" ${SISL_SOURCE_DIR}
                BUILD_COMMAND msbuild sisl.sln /p:Configuration=Release
                INSTALL_COMMAND ""
            )

            if( CMAKE_BUILD_TYPE MATCHES "Debug" )
                set( SISL_LIBRARIES ${CMAKE_BINARY_DIR}/sisl_submod/Debug/${CMAKE_SHARED_LIBRARY_PREFIX}sisl.lib )
            else()
                set( SISL_LIBRARIES ${CMAKE_BINARY_DIR}/sisl_submod/Release/${CMAKE_SHARED_LIBRARY_PREFIX}sisl.lib )
            endif()

            add_definitions( /D SISLNEEDPROTOTYPES )

        elseif( WIN32 )

            ExternalProject_Add( sisl_submod
                PREFIX sisl_submod
                SOURCE_DIR  .
                BINARY_DIR sisl_submod
                BUILD_IN_SOURCE 0
                CONFIGURE_COMMAND cmake -G "${CMAKE_GENERATOR}" -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_AR=${CMAKE_AR} -DBUILD_SHARED_LIBS:BOOL=TRUE ${SISL_SOURCE_DIR}
                INSTALL_COMMAND ""
            )

            set( SISL_LIBRARIES ${CMAKE_BINARY_DIR}/sisl_submod/${CMAKE_SHARED_LIBRARY_PREFIX}sisl${CMAKE_SHARED_LIBRARY_SUFFIX} )

        else()

            ExternalProject_Add( sisl_submod
                PREFIX sisl_submod
                SOURCE_DIR  .
                BINARY_DIR sisl_submod
                BUILD_IN_SOURCE 0
                CONFIGURE_COMMAND cmake -DBUILD_SHARED_LIBS:BOOL=TRUE ${SISL_SOURCE_DIR}
                INSTALL_COMMAND ""
            )

            set( SISL_LIBRARIES ${CMAKE_BINARY_DIR}/sisl_submod/${CMAKE_SHARED_LIBRARY_PREFIX}sisl${CMAKE_SHARED_LIBRARY_SUFFIX} )

        endif()

        set( SISL_NEEDS_BUILD TRUE )

    endif()

else()

    message( WARNING "Using user-provided SISL_INCLUDE_DIR and NOT SISL_LIBRARIES" )

endif()


find_package( Boost 1.55 REQUIRED COMPONENTS filesystem system date_time )

set( LIBIGES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( LIBIGES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

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

if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
elseif( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    add_definitions( -D_USE_MATH_DEFINES -DBOOST_ALL_DYN_LINK -DBOOST_EXCEPTION_DISABLE )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4" )
endif()

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release CACHE STRING
        "Build type, options are Debug or Release (default)" FORCE )
endif()

if( CMAKE_BUILD_TYPE MATCHES Debug )
    add_definitions( -DDEBUG )
endif()

set( SRC_ENT "${CMAKE_CURRENT_SOURCE_DIR}/entities" )
set( SRC_IGS "${CMAKE_CURRENT_SOURCE_DIR}/iges" )
set( SRC_GEOM "${CMAKE_CURRENT_SOURCE_DIR}/geom" )

add_library( iges SHARED
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

if( SISL_NEEDS_BUILD )
    add_dependencies( iges sisl_submod )
endif()

# Note that putting quotes around the variables here will make linking
# fail on windows target bulds, both GCC cross compilation and MSVC
# native.
target_link_libraries( iges ${SISL_LIBRARIES} ${Boost_LIBRARIES} )

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
