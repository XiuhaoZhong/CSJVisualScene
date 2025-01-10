cmake_minimum_required(VERSION 3.16)

project(CSJVisualScene VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#if(NOT DEFINED INSTALL_EXAMPLESDIR)
#    set(INSTALL_EXAMPLESDIR "examples")
#endif()

#set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/widgets/widgets/analogclock")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGLWidgets OpenGL Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets OpenGL Core)

set(PROJECT_SOURCES
    main/main.cpp
    main/mainwindow.cpp
    main/mainwindow.h
    UIModules/CSJSceneRenderWindow.h
    UIModules/CSJSceneRenderWindow.cpp
    UIModules/CSJGLRenderWidget.h
    UIModules/CSJGLRenderWidget.cpp
)

set(CSJRENDER_SOURCES
    render/CSJRenderGuidManager.h
    render/CSJRenderSource.h
    render/CSJRenderSource.cpp
    render/CSJRenderObjectData.h
    render/CSJRenderObjectData.cc
    render/CSJModelImporter.h
    render/CSJModelImporter.cpp
    render/CSJAssetManager.h
    render/CSJAssetManager.cc
) 

set(CSJGL_SOURCES
    CSJGLRender/CSJGLProgram.h
    CSJGLRender/CSJGLProgram.cpp
    CSJGLRender/CSJGLSceneBase.h
    CSJGLRender/CSJGLSceneBase.cpp
    CSJGLRender/CSJGLPresetScenes/CSJGLShadowScene.h
    CSJGLRender/CSJGLPresetScenes/CSJGLShadowScene.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 3rdParts)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CSJVisualScene
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CSJGL_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CSJVisualScene APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CSJVisualScene SHARED
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

set_target_properties(CSJVisualScene PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(CSJVisualScene PRIVATE Qt::Widgets 
                                             Qt::OpenGLWidgets
                                             Qt::OpenGL
                                             Qt::Core)

#install(TARGETS analogclock
#    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
#    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
#    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
#)