cmake_minimum_required(VERSION 3.14.0)

set (CMAKE_CXX_STANDARD 20)

# Find VST SDK
set(vst3sdk_SOURCE_DIR C:/Users/Jeroen/source/repos/VST_SDK/vst3sdk)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

set(PRJ_NAME "CMBNEX")

project(${PRJ_NAME})

set(SRC "${${PRJ_NAME}_SOURCE_DIR}/")

file(GLOB_RECURSE SOURCE
  "${SRC}source/*.cpp"
  "${SRC}include/*.hpp"
  "${SRC}resource/*.rc"
  "${SRC}resource/*.h"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(${PRJ_NAME}
    ${SOURCE}
)

target_include_directories(${PRJ_NAME} PUBLIC
  include/
  resource/
)

target_link_libraries(${PRJ_NAME}
    PRIVATE
        vstgui_support
)

target_link_libraries(${PRJ_NAME}
    PRIVATE
        sdk
)

source_group(TREE ${SRC} FILES ${SOURCE})

target_precompile_headers(${PRJ_NAME} PUBLIC
  "${SRC}include/pch.hpp"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PRJ_NAME})
