set( LIBIGES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( LIBIGES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )


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
    "${LIBIGES_SOURCE_DIR}/include/api"
    )




set( SRC_ENT "${CMAKE_CURRENT_SOURCE_DIR}/entities" )
set( SRC_IGS "${CMAKE_CURRENT_SOURCE_DIR}/iges" )
set( SRC_DLL "${CMAKE_CURRENT_SOURCE_DIR}/dllapi" )
set( SRC_GEOM "${CMAKE_CURRENT_SOURCE_DIR}/geom" )

if( HAS_NURBS_LIB )
    set( NURBS_DEPS
            "${SRC_GEOM}/geom_wall.cpp"
            "${SRC_GEOM}/geom_cylinder.cpp"
            "${SRC_GEOM}/iges_geom_pcb.cpp"
            "${SRC_GEOM}/mcad_segment.cpp"
            "${SRC_GEOM}/mcad_outline.cpp"
            "${SRC_DLL}/dll_mcad_segment.cpp"
            "${SRC_DLL}/dll_mcad_outline.cpp"
            "${SRC_DLL}/dll_iges_geom_pcb.cpp"
    )
endif( HAS_NURBS_LIB )

set( IGES_SOURCES
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
    "${SRC_IGS}/mcad_utils.cpp"
    "${SRC_DLL}/dll_iges.cpp"
    "${SRC_DLL}/dll_iges_entity.cpp"
    "${SRC_DLL}/dll_iges_curve.cpp"
    "${SRC_DLL}/dll_entity100.cpp"
    "${SRC_DLL}/dll_entity102.cpp"
    "${SRC_DLL}/dll_entity104.cpp"
    "${SRC_DLL}/dll_entity110.cpp"
    "${SRC_DLL}/dll_entity120.cpp"
    "${SRC_DLL}/dll_entity122.cpp"
    "${SRC_DLL}/dll_entity124.cpp"
    "${SRC_DLL}/dll_entity126.cpp"
    "${SRC_DLL}/dll_entity128.cpp"
    "${SRC_DLL}/dll_entity142.cpp"
    "${SRC_DLL}/dll_entity144.cpp"
    "${SRC_DLL}/dll_entity308.cpp"
    "${SRC_DLL}/dll_entity314.cpp"
    "${SRC_DLL}/dll_entity408.cpp"
    "${SRC_GEOM}/mcad_elements.cpp"
    "${SRC_GEOM}/mcad_helpers.cpp"
    ${NURBS_DEPS}
    )

if( STATIC_IGES )
    add_library( iges_static STATIC ${IGES_SOURCES} )
    set( IGES_LIBS iges_static )
else()
    add_library( iges SHARED ${IGES_SOURCES} )
    set( IGES_LIBS iges )
endif()

if( USE_SISL AND SISL_NEEDS_BUILD )

    # CAVEAT: even when we build SISL we do not install it;
    # the user is responsible for copying the appropriate
    # files to the appropriate directories.

    add_dependencies( ${IGES_LIBS} sisl_submod )

endif()

# Note that putting quotes around the variables here will make linking
# fail on windows target bulds, both GCC cross compilation and MSVC
# native.
if( USE_SISL )
    target_link_libraries( ${IGES_LIBS} ${SISL_LIBRARIES} )
endif()

install( TARGETS ${IGES_LIBS}
        ARCHIVE DESTINATION ${LIBIGES_LIBDIR}
        LIBRARY DESTINATION ${LIBIGES_LIBDIR}
        CONFIGURATIONS Release
    )

add_executable( readtest
    "${LIBIGES_SOURCE_DIR}/tests/test_read.cpp"
    )

add_executable( mergetest
    "${LIBIGES_SOURCE_DIR}/tests/test_merge.cpp"
    )

target_link_libraries( readtest ${IGES_LIBS} )
target_link_libraries( mergetest ${IGES_LIBS} )

if( HAS_NURBS_LIB )
    add_executable( curvetest
            "${LIBIGES_SOURCE_DIR}/tests/test_curves.cpp"
    )
    target_link_libraries( curvetest ${IGES_LIBS} )

    add_executable( segtest
            "${LIBIGES_SOURCE_DIR}/tests/test_segs.cpp"
    )
    target_link_libraries( segtest ${IGES_LIBS} )

    add_executable( olntest
            "${LIBIGES_SOURCE_DIR}/tests/test_outline.cpp"
    )
    target_link_libraries( olntest ${IGES_LIBS} )

    add_executable( planetest
            "${LIBIGES_SOURCE_DIR}/tests/test_plane.cpp"
    )
    target_link_libraries( planetest ${IGES_LIBS} )

    add_executable( circles
            "${LIBIGES_SOURCE_DIR}/tests/test_circle.cpp"
    )
    target_link_libraries( circles ${IGES_LIBS} )

    # build the idf2igs tool
    add_subdirectory( idf )

endif(HAS_NURBS_LIB)


# Installation of header files
set( HDR_CORE include/iges/core )
set( HDR_API include/iges )
set( INC_IGES "${CMAKE_CURRENT_SOURCE_DIR}/include/iges" )
set( INC_API "${CMAKE_CURRENT_SOURCE_DIR}/include/api" )
set( INC_GEOM "${CMAKE_CURRENT_SOURCE_DIR}/include/geom" )

# files essential to the IGES core; some users may want
# these for direct interaction with the core objects
# in statically linked programs
set( CORE_FILES
        ${INC_IGES}/all_entities.h
        ${INC_IGES}/entity100.h
        ${INC_IGES}/entity102.h
        ${INC_IGES}/entity104.h
        ${INC_IGES}/entity110.h
        ${INC_IGES}/entity120.h
        ${INC_IGES}/entity122.h
        ${INC_IGES}/entity124.h
        ${INC_IGES}/entity126.h
        ${INC_IGES}/entity128.h
        ${INC_IGES}/entity142.h
        ${INC_IGES}/entity144.h
        ${INC_IGES}/entity154.h
        ${INC_IGES}/entity164.h
        ${INC_IGES}/entity180.h
        ${INC_IGES}/entity184.h
        ${INC_IGES}/entity186.h
        ${INC_IGES}/entity308.h
        ${INC_IGES}/entity314.h
        ${INC_IGES}/entity408.h
        ${INC_IGES}/entity430.h
        ${INC_IGES}/entity502.h
        ${INC_IGES}/entity504.h
        ${INC_IGES}/entity508.h
        ${INC_IGES}/entity510.h
        ${INC_IGES}/entity514.h
        ${INC_IGES}/entityNULL.h
        ${INC_IGES}/iges_curve.h
        ${INC_IGES}/iges_entity.h
        ${INC_IGES}/iges.h
    )

# files essential to the API layer as well as the core layer
set( API_FILES
        ${INC_IGES}/iges_base.h
        ${INC_IGES}/mcad_utils.h
        ${INC_API}/all_api_entities.h
        ${INC_API}/dll_entity100.h
        ${INC_API}/dll_entity102.h
        ${INC_API}/dll_entity104.h
        ${INC_API}/dll_entity110.h
        ${INC_API}/dll_entity120.h
        ${INC_API}/dll_entity122.h
        ${INC_API}/dll_entity124.h
        ${INC_API}/dll_entity126.h
        ${INC_API}/dll_entity128.h
        ${INC_API}/dll_entity142.h
        ${INC_API}/dll_entity144.h
        ${INC_API}/dll_entity308.h
        ${INC_API}/dll_entity314.h
        ${INC_API}/dll_entity408.h
        ${INC_API}/dll_iges_curve.h
        ${INC_API}/dll_iges_entity.h
        ${INC_API}/dll_iges.h
    )

# core files which are only present when built with SISL support
set( EXTRA_CORE_FILES
        ${INC_GEOM}/iges_geom_pcb.h
        ${INC_GEOM}/mcad_outline.h
        ${INC_GEOM}/mcad_segment.h
    )

# API layer files which are only present when built with SISL support
set( EXTRA_API_FILES
        ${INC_GEOM}/geom_cylinder.h
        ${INC_GEOM}/geom_wall.h
        ${INC_GEOM}/mcad_elements.h
        ${INC_GEOM}/mcad_helpers.h
        ${INC_API}/dll_mcad_outline.h
        ${INC_API}/dll_mcad_segment.h
        ${INC_API}/dll_iges_geom_pcb.h
)


install( FILES
    ${API_FILES}
    DESTINATION ${HDR_API}
    CONFIGURATIONS Release
)

install( FILES
    ${CORE_FILES}
    DESTINATION ${HDR_CORE}
    CONFIGURATIONS Release
)

if( USE_SISL )
    install( FILES
        ${EXTRA_API_FILES}
        DESTINATION ${HDR_API}
        CONFIGURATIONS Release
    )

    install( FILES
        ${EXTRA_CORE_FILES}
        DESTINATION ${HDR_CORE}
        CONFIGURATIONS Release
    )
endif()
