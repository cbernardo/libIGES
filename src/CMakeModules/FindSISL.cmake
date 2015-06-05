find_path( SISL_INCLUDE_DIR sisl.h PATH_SUFFIXES include )

if( NOT SISL_LIBRARIES )
    find_library( SISL_LIBRARY_RELEASE NAMES sisl PATH_SUFFIXES lib )
    find_library( SISL_LIBRARY_DEBUG NAMES sisld PATH_SUFFIXES lib )

    include( SelectLibraryConfigurations )
    SELECT_LIBRARY_CONFIGURATIONS( SISL )
endif()

# handle the QUIETLY and REQUIRED arguments and set SISL_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( SISL
                                   REQUIRED_VARS SISL_LIBRARIES SISL_INCLUDE_DIR )

mark_as_advanced( SISL_INCLUDE_DIR )
