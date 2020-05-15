cmake_minimum_required(VERSION 2.8)

message(STATUS "## ${CMAKE_CURRENT_BINARY_DIR} ##")
get_filename_component(EXAMPLE_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
string(SUBSTRING ${EXAMPLE_NAME} 0 3 TARGET_NAME)
project(${TARGET_NAME})

set(SOURCE_FILES ${EXAMPLE_NAME}.c)
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME} bgapi::bgapi2_genicam)

set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME ${EXAMPLE_NAME})
set_property(TARGET ${TARGET_NAME} PROPERTY PROJECT_LABEL ${EXAMPLE_NAME})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(USER_FILE ${TARGET_NAME}.vcxproj.user)
    configure_file(UserTemplate.user ${USER_FILE} @ONLY)

    foreach(BGPAI2_FILE ${BGAPI2_SDK})
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BGPAI2_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${CFG_OUT_FOLDER})
    endforeach()
endif()
