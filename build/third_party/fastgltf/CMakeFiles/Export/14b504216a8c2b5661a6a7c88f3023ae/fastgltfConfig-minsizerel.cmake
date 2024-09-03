#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fastgltf::fastgltf" for configuration "MinSizeRel"
set_property(TARGET fastgltf::fastgltf APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(fastgltf::fastgltf PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/fastgltf.lib"
  )

list(APPEND _cmake_import_check_targets fastgltf::fastgltf )
list(APPEND _cmake_import_check_files_for_fastgltf::fastgltf "${_IMPORT_PREFIX}/lib/fastgltf.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
