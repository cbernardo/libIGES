include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
  )

add_library( idf3 STATIC
    idf_helpers.cpp idf_common.cpp idf_outlines.cpp
    idf_parser.cpp )

add_executable( idf2igs idf2igs.cpp )
target_link_libraries( idf2igs iges idf3 "${Boost_LIBRARIES}" )
