cmake_minimum_required(VERSION 3.14.0)

set (CMAKE_CXX_STANDARD 20)

# Find VST SDK
set(vst3sdk_SOURCE_DIR C:/Users/Jeroen/source/repos/VST_SDK/vst3sdk)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

set(PRJ_NAME "Test")

project(${PRJ_NAME})

set(SRC "${${PRJ_NAME}_SOURCE_DIR}/")

file(GLOB_RECURSE SOURCE
  "${SRC}source/*.cpp"
  "${SRC}include/*.hpp"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(${PRJ_NAME}
    ${SOURCE}
)

target_include_directories(${PRJ_NAME} PUBLIC
  include/
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
    target_sources(${PRJ_NAME}
        PRIVATE
            resource/myplugineditor.uidesc
    )
    target_link_libraries(${PRJ_NAME}
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(${PRJ_NAME}
        RESOURCES
            "resource/myplugineditor.uidesc"
    )
endif(SMTG_ADD_VSTGUI)
# -------------------

smtg_target_add_plugin_snapshots (${PRJ_NAME}
    RESOURCES
        resource/D1BF90BC4C2C5456A9FE86ABF5770A29_snapshot.png
        resource/D1BF90BC4C2C5456A9FE86ABF5770A29_snapshot_2.0x.png
)

target_link_libraries(${PRJ_NAME}
    PRIVATE
        sdk
)

source_group(TREE ${SRC} FILES ${SOURCE})

target_precompile_headers(${PRJ_NAME} PUBLIC
  "${SRC}include/pch.hpp"
)

if(SMTG_MAC)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
    smtg_target_set_bundle(${PRJ_NAME}
        BUNDLE_IDENTIFIER me.kaixo.test
        INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS
    )
    smtg_target_set_debug_executable(${PRJ_NAME}
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(${PRJ_NAME} PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PRJ_NAME})

        smtg_target_set_debug_executable(${PRJ_NAME}
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
