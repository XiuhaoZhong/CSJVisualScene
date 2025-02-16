cmake_minimum_required(VERSION 3.16)

set(TARGET_NAME CSJVisualScene)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${TARGET_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(TINYOBJLOADER_IMPLEMENTATION)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdParts)

set(JSON_INCLUDE ${THIRD_PARTY_DIR}/json11)
set(GLM_INCLUDE ${THIRD_PARTY_DIR})
add_library(json11 ${JSON_INCLUDE}/json11.cpp)
set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

#if(NOT DEFINED INSTALL_EXAMPLESDIR)
#    set(INSTALL_EXAMPLESDIR "examples")
#endif()

#set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/widgets/widgets/analogclock")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui)

include_directories(${ROOT_DIR}/src ${THIRD_PARTY_DIR})

set(PROJECT_SOURCES
    src/main/main.cpp
    src/main/mainwindow.cpp
    src/main/mainwindow.h
    src/CSJSceneEditor/CSJSceneEditorWindow.h
    src/CSJSceneEditor/CSJSceneEditorWindow.cpp
)

set(CSJGL_SOURCES
    src/CSJGLRender/CSJGLSceneBase.h
    src/CSJGLRender/CSJGLSceneBase.cpp
    src/CSJGLRender/CSJGLPresetScenes/CSJGLShadowScene.h
    src/CSJGLRender/CSJGLPresetScenes/CSJGLShadowScene.cpp
)

file(GLOB_RECURSE RUNTIME_SOURCE_FILE "${ROOT_DIR}/src/CSJSceneRuntime/*")

if(WIN32)
set(vulkan_lib )
elseif(APPLE)
set(vulkan_lib "/Users/xiuhaozhong/DevEnvironments/VulkanSDK/1.4.304.0/macOS/lib/libvulkan.1.dylib")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${TARGET_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CSJGL_SOURCES}
        ${CSJRENDER_SOURCES}
        ${RUNTIME_SOURCE_FILE}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CSJVisualScene APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${TARGET_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CSJVisualScene
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${TARGET_NAME} PRIVATE ${vulkan_lib})
target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader)
target_link_libraries(${TARGET_NAME} PRIVATE Qt::Widgets Qt::Core Qt::Gui)
target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:json11>)

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${JSON_INCLUDE}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${GLM_INCLUDE}>)

add_subdirectory(${THIRD_PARTY_DIR})

#install(TARGETS analogclock
#    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
#    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
#    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
#)