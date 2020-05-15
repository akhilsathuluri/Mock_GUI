
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was CMakeConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(bgapi_VERSION_MAJOR "2")
set(bgapi_VERSION_MINOR "10")
set(bgapi_VERSION_PATCH "0")

include(FindPackageHandleStandardArgs)

set_and_check(bgapi_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include")
set_and_check(bgapi_LIBRARY_DIR "${PACKAGE_PREFIX_DIR}/lib")

foreach(__component ${bgapi_FIND_COMPONENTS})
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/bgapi-${__component}.cmake")
        include("${CMAKE_CURRENT_LIST_DIR}/bgapi-${__component}.cmake")
        set(bgapi_${__component}_FOUND TRUE)
        set(bgapi_${__component}_LIBRARY bgapi::bgapi2_${__component})
    endif()
endforeach()

find_package_handle_standard_args(bgapi REQUIRED_VARS PACKAGE_PREFIX_DIR HANDLE_COMPONENTS)

check_required_components(bgapi)
