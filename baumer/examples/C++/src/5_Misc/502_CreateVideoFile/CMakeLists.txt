cmake_minimum_required(VERSION 2.8)

message(STATUS "## ${CMAKE_CURRENT_BINARY_DIR} ##")
get_filename_component(EXAMPLE_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
string(SUBSTRING ${EXAMPLE_NAME} 0 3 TARGET_NAME)
project(${EXAMPLE_NAME})

set(SOURCE_FILES 
    ${EXAMPLE_NAME}.cpp
    ${SHARED_DIR}/Arguments.cpp
    ${SHARED_DIR}/Arguments.h
    ${SHARED_DIR}/HelperFunctions.cpp
    ${SHARED_DIR}/HelperFunctions.h
    UserTemplate.user
)
include_directories(${SHARED_DIR})

# =================== Add openCV support! ==============================
if(ENABLE_OPENCV)
    message(STATUS "\n======================= Configure OpenCV library =======================")
    find_package(OpenCV QUIET COMPONENTS core highgui imgproc)  # some packages in OpenCV obsolete ?!?
    if(OpenCV_FOUND)
        message(STATUS "OpenCV Version: '${OpenCV_VERSION}'")
        message(STATUS "OpenCV LibDir:  '${OpenCV_LIB_PATH}'")

        if(OpenCV_VERSION_MAJOR GREATER "2")
            add_definitions(-DUSE_OCL_COMPONENT=3)
            find_package(OpenCV COMPONENTS videoio imgcodecs)
            if(NOT OpenCV_FOUND)
                message(STATUS "OpenCV has no Video support!")
            endif()
        elseif (OpenCV_VERSION_MAJOR LESS "2")
            option(ENABLE_OPENCV "Enable OpenCV Support in SDK example" OFF)
            option(OpenCV_FOUND "OpenCV Found" OFF)
            message(STATUS "OpenCV with Version 1.x not supported!")
        else()  # OpenCV Version 2.x
            find_package(OpenCV COMPONENTS ocl)  # this package in OpenCV obsolete ?!?
        endif()
    else()
        message(STATUS "OpenCV not supported!")
    endif()

    if(OpenCV_FOUND AND ENABLE_OPENCV)
        add_definitions(-DUSE_OPENCV=1)
        list(APPEND INCLUDE_DIR ${OpenCV_INCLUDE_DIRS})
        message(STATUS "OpenCV Libs:  '${OpenCV_LIBS}'")
    else()
        message(STATUS "This means program '${PROJECT_NAME}' couldn't run properly!")
        message(STATUS "Please set the correct search path for the OpenCV package and/or use OpenCV > 3.0!")
        message(STATUS "=====================================================================\n")
    endif()
endif()

message(STATUS "## OpenCV is configured: ${PROJECT_NAME}! ##\n")
message(STATUS "=====================================================================\n")

add_executable(${TARGET_NAME} ${SOURCE_FILES})
if(OpenCV_FOUND)
    target_link_libraries(${TARGET_NAME} "${OpenCV_LIBS}")
endif()

set_property(TARGET ${TARGET_NAME} PROPERTY PROJECT_LABEL ${PROJECT_NAME})
set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME ${PROJECT_NAME})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(OpenCV_FOUND AND OpenCV_SHARED)
        get_target_property(OpenCV_DLL_DEBUG   opencv_world IMPORTED_LOCATION_DEBUG)
        get_target_property(OpenCV_DLL_RELEASE opencv_world  IMPORTED_LOCATION_RELEASE)
        set(OpenCV_WorldLib  "$<$<CONFIG:debug>:${OpenCV_DLL_DEBUG}>$<$<CONFIG:release>:${OpenCV_DLL_RELEASE}>")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OpenCV_WorldLib}"  $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMENT "copy OpenCV library to output directory")
    endif()

    set(USER_FILE ${TARGET_NAME}.vcxproj.user)
    configure_file(UserTemplate.user ${USER_FILE} @ONLY)
endif()
