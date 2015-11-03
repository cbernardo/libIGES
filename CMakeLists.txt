# Project: libIGES (IGESv6)
#
project( libIGES CXX C )

cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CMAKE_VERBOSE_MAKEFILE ON )

# Version information
set( LIBIGES_VERSION_MAJOR 0 )
set( LIBIGES_VERSION_MINOR 2 )

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules )

if( USE_SISL )
    #NOTE: the SISL_LIBRARIES path has only been checked on Linux;
    #I don't know if it will work on other platforms.
    if( NOT SISL_INCLUDE_DIR OR NOT SISL_LIBRARIES )
        find_package( SISL )

        if( NOT SISL_FOUND )
            message( WARNING "SISL could not be found; attempting to build from git submodule" )

            include( ExternalProject )
            set( SISL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/sisl" )
            find_path( SISL_INCLUDE_DIR sisl.h PATHS "${SISL_SOURCE_DIR}/include" NO_DEFAULT_PATH )

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
                    CONFIGURE_COMMAND
                        COMMAND git submodule init
                        COMMAND git submodule update
                        COMMAND cmake -G "${CMAKE_GENERATOR}" -DBUILD_SHARED_LIBS:BOOL=TRUE -DCMAKE_SHARED_LINKER_FLAGS="/DEF:../../sisl_exports.def" ${SISL_SOURCE_DIR}
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
                    CONFIGURE_COMMAND
                        COMMAND cmake -G "${CMAKE_GENERATOR}" -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_AR=${CMAKE_AR} -DBUILD_SHARED_LIBS:BOOL=TRUE ${SISL_SOURCE_DIR}
                    INSTALL_COMMAND ""
                )

                set( SISL_LIBRARIES ${CMAKE_BINARY_DIR}/sisl_submod/${CMAKE_SHARED_LIBRARY_PREFIX}sisl${CMAKE_SHARED_LIBRARY_SUFFIX} )

            else()

                ExternalProject_Add( sisl_submod
                    PREFIX sisl_submod
                    SOURCE_DIR  .
                    BINARY_DIR sisl_submod
                    BUILD_IN_SOURCE 0
                    CONFIGURE_COMMAND
                        COMMAND git submodule init
                        COMMAND git submodule update
                        COMMAND cmake -DBUILD_SHARED_LIBS:BOOL=TRUE ${SISL_SOURCE_DIR}
                    INSTALL_COMMAND ""
                )

                set( SISL_LIBRARIES ${CMAKE_BINARY_DIR}/sisl_submod/${CMAKE_SHARED_LIBRARY_PREFIX}sisl${CMAKE_SHARED_LIBRARY_SUFFIX} )

            endif()

            set( SISL_NEEDS_BUILD TRUE )

        endif()

    else( NOT SISL_INCLUDE_DIR OR NOT SISL_LIBRARIES )

        message( WARNING "Using user-provided SISL_INCLUDE_DIR and NOT SISL_LIBRARIES" )

    endif( NOT SISL_INCLUDE_DIR OR NOT SISL_LIBRARIES )

    set( HAS_NURBS_LIB 1 )

endif( USE_SISL )

if( USE_SISL )
    include_directories( "${SISL_INCLUDE_DIR}" )
endif()

if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
elseif( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    add_definitions( -D_USE_MATH_DEFINES )
endif()

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release CACHE STRING
        "Build type, options are Debug or Release (default)" FORCE )
endif()

if( CMAKE_BUILD_TYPE MATCHES Debug )
    add_definitions( -DDEBUG )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    set( LIBIGES_LIBDIR bin )
    set( LIBIGES_BINDIR bin )
else()
    set( LIBIGES_LIBDIR lib )
    set( LIBIGES_BINDIR bin )
endif()

add_subdirectory( src )
