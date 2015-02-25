# Project: libIGES (IGESv6)
#
project( libIGES CXX C )

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

# Version information
set( LIBIGES_VERSION_MAJOR 0 )

set( LIBIGES_VERSION_MINOR 1 )

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
    )

set( CMAKE_CXX_FLAGS "-O2 -Wall -DDEBUG -lm" )

# pull in the sources
#add_subdirectory ( entities )
#add_subdirectory ( iges )

set( SRC_ENT "${CMAKE_CURRENT_SOURCE_DIR}/entities" )
set( SRC_IGS "${CMAKE_CURRENT_SOURCE_DIR}/iges" )

#add_library( iges SHARED
#    "${SRC_ENT}/iges_entity.cpp"
#    "${SRC_ENT}/entityNULL.cpp"
#    "${SRC_ENT}/entity100.cpp"
#    "${SRC_IGS}/iges_io.cpp"
#    "${SRC_IGS}/iges.cpp"
#    )

 add_executable( testiges
    "${SRC_ENT}/iges_entity.cpp"
    "${SRC_ENT}/entityNULL.cpp"
    "${SRC_ENT}/entity100.cpp"
    "${SRC_IGS}/iges_io.cpp"
    "${SRC_IGS}/iges.cpp"
    "${LIBIGES_SOURCE_DIR}/test.cpp"
    )
