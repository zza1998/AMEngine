#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fastgltf::fastgltf_simdjson" for configuration "Release"
set_property(TARGET fastgltf::fastgltf_simdjson APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fastgltf::fastgltf_simdjson PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/fastgltf_simdjson.lib"
  )

list(APPEND _cmake_import_check_targets fastgltf::fastgltf_simdjson )
list(APPEND _cmake_import_check_files_for_fastgltf::fastgltf_simdjson "${_IMPORT_PREFIX}/lib/fastgltf_simdjson.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
