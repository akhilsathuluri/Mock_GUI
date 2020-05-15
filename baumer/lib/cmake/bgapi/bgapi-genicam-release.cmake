#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bgapi::bgapi2_genicam" for configuration "Release"
set_property(TARGET bgapi::bgapi2_genicam APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bgapi::bgapi2_genicam PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbgapi2_genicam.so"
  IMPORTED_SONAME_RELEASE "libbgapi2_genicam.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgapi::bgapi2_genicam )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgapi::bgapi2_genicam "${_IMPORT_PREFIX}/lib/libbgapi2_genicam.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
