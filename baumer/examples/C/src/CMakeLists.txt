cmake_minimum_required(VERSION 2.8)
project(Baumer_GAPI_SDK_Examples_C)

# search for bgapi package at specific destinations
# 1. test if local package
# 2. test for environment entry
# 3. search for system package (not on windows)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(BGAPI_CONFIG_SUBDIR Dev/C++/Lib/cmake/bgapi)
else()
    set(BGAPI_CONFIG_SUBDIR lib/cmake/bgapi)
endif()
if(IS_DIRECTORY ../../../${BGAPI_CONFIG_SUBDIR})
    get_filename_component(BGAPI_CONFIG_SUBDIR ${PROJECT_SOURCE_DIR}/../../../${BGAPI_CONFIG_SUBDIR} ABSOLUTE)
elseif($ENV{BGAPI2_SDK_PATH})
    get_filename_component(BGAPI_CONFIG_SUBDIR $ENV{BGAPI2_SDK_PATH}/${BGAPI_CONFIG_SUBDIR} ABSOLUTE)
elseif(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(BGAPI_CONFIG_SUBDIR /opt/baumer-gapi-sdk/${BGAPI_CONFIG_SUBDIR})
endif()
set(bgapi_DIR ${BGAPI_CONFIG_SUBDIR} CACHE STRING "Path to the cmake subfolder of bgapi-sdk installation directory")
find_package(bgapi 2.10.0 REQUIRED COMPONENTS genicam)

# collect all BGAPI libraries and producer for later use
file(GLOB_RECURSE BGAPI2_SDK
    ${bgapi_LIBRARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${bgapi_LIBRARY_DIR}/*.cti
    ${bgapi_LIBRARY_DIR}/*.xml
)

# handle multi config projects
if(NOT CMAKE_BUILD_TYPE)
    set(CFG_OUT_FOLDER $<CONFIGURATION>)
endif()

# set some specific compiler switches
if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -fexpensive-optimizations -fomit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funroll-loops -pipe -fexceptions -Wpointer-arith -Wcast-align -Wsign-compare -pedantic")
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--enable-stdcall-fixup")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--enable-stdcall-fixup")
    else()
        add_definitions(-D_GNULINUX)
    endif()
    if(MMX_SUPPORT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmmx -msse")
    endif()
    get_filename_component(C_COMPILER_DIR ${CMAKE_C_COMPILER} DIRECTORY)
    configure_file(${CMAKE_SOURCE_DIR}/.vscodein/tasks.json ${CMAKE_SOURCE_DIR}/.vscode/tasks.json @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/.vscodein/launch.json ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/.vscodein/c_cpp_properties.json ${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json @ONLY)
endif()

get_filename_component(SHARED_DIR ${PROJECT_SOURCE_DIR}/Shared ABSOLUTE)
message(STATUS "SHARED_DIR (whole project) = ${SHARED_DIR}")

# Full list of examples
# Remove an example from build by commenting with '#'
set(EXAMPLE_LIST
    0_Common/001_ImageCaptureMode_Polling
    0_Common/002_CameraParameterTree
    0_Common/003_CameraConfiguration
    0_Common/004_PartialScan_ExternalBuffer
    0_Common/005_PixelTransformation
    0_Common/006_DeviceEventMode_Polling
    0_Common/007_Chunk
    0_Common/008_Trace
    0_Common/009_ShortExample
    0_Common/010_PnPEventMode_Polling
    0_Common/011_ImageCaptureMode_Handler
    0_Common/012_DeviceEventMode_Handler
    0_Common/013_PnPEventMode_Handler

    1_GigE/101_HeartbeatTimeout
    1_GigE/102_ForceIP
)

foreach(EXAMPLE ${EXAMPLE_LIST})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${EXAMPLE})
        add_subdirectory(${EXAMPLE})
    endif()
endforeach()
