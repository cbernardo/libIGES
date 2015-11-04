# Project: libIGES (IGESv6)
#
project( IGES_TUTORIAL CXX C )

cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CMAKE_VERBOSE_MAKEFILE ON )

if( NOT IGES_INCLUDES )
    set( IGES_INCLUDES $ENV{HOME}/usr/include/iges )
endif()

if( NOT IGES_LIB_PATH )
    set( IGES_LIB_PATH $ENV{HOME}/usr/lib )
endif()

include_directories( ${IGES_INCLUDES} )
link_directories( ${IGES_LIB_PATH} )

add_executable( tutorial1 tutorial1.cpp )
target_link_libraries( tutorial1 iges )
