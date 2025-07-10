cmake_minimum_required(VERSION 3.16)

set(TARGET_NAME CSJVisualScene)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${TARGET_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QTDIR $ENV{QTDIR_6_8_0})
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR ${THIRDPARTS_DIR})

if (CSJ_BUILD_DEBUG)
    set(QTDIR ${QTDIR}/debug)
    #set(OUTPUT_DIR "${ROOT_DIR}/output/Debug/")
    #file(MAKE_DIRECTORY ${OUTPUT_DIR})
else()
    set(QTDIR ${QTDIR}/release)
    #set(OUTPUT_DIR "${ROOT_DIR}/output/Release/")
    #file(MAKE_DIRECTORY ${OUTPUT_DIR})
endif()

set(QT_DEBUG_DIR "${QTDIR}" CACHE PATH "QT debug Directory")
set(QT_RELEASE_DIR "${QTDIR}" CACHE PATH "QT release Directory")

set(CMAKE_PREFIX_PATH 
    "$<IF:$<CONFIG:Debug>, ${QT_DEBUG_DIR}, $<QT_RELEASE_DIR>>")

# set the resource directory in the output directory
set(OUTPUT_RESOURCE_DIR "${OUTPUT_DIR}/resource")

set(DYNAMIC_LIBS "")

# set Qt environment variables
if (CMAKE_HOST_WIN32)
    set(ENV{PATH} "${QTDIR}/bin${QTDIR}/lib;$ENV{PATH}")
elseif (CMAKE_HOST_APPLE)
    set(ENV{PATH} "${QTDIR}/bin:${QTDIR}/lib:$ENV{PATH}:$ENV{VULKAN_DIR}/lib:$ENV{PATH}")
endif()

set(QT_DIR $ENV{QTDIR}/lib/cmake/Qt6)
set(QT6_DIR $ENV{QTDIR}/lib/cmake/Qt6)

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(TINYOBJLOADER_IMPLEMENTATION)
add_definitions("-Wall -g")

set(JSON_INCLUDE ${THIRD_PARTY_DIR}/json11)
set(GLM_INCLUDE ${THIRD_PARTY_DIR})
add_library(json11 ${JSON_INCLUDE}/json11.cpp)
set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui)

include_directories(${ROOT_DIR} ${THIRD_PARTY_DIR})

set(PROJECT_SOURCES
    main/main.cpp
    main/mainwindow.cpp
    main/mainwindow.h
    CSJSceneEditor/CSJSceneEditorWindow.h
    CSJSceneEditor/CSJSceneEditorWindow.cpp
)

set(CSJGL_SOURCES
    CSJGLRender/CSJGLSceneBase.h
    CSJGLRender/CSJGLSceneBase.cpp
    CSJGLRender/CSJGLPresetScenes/CSJGLShadowScene.h
    CSJGLRender/CSJGLPresetScenes/CSJGLShadowScene.cpp
)

file(GLOB_RECURSE RUNTIME_SOURCE_FILE "${ROOT_DIR}/CSJSceneRuntime/*")
file(GLOB TOOL_SOURCE_FILES "${ROOT_DIR}/Utils/*")
 
if(WIN32)
    set(Vulkan_DIR $ENV{VULKAN_SDK})
    set(vulkan_Lib_DIR ${Vulkan_DIR}/Lib/cmake)
    set(vulkan_lib ${Vulkan_DIR}/Lib/vulkan-1.lib)
elseif(APPLE)
    set(Vulkan_DIR $ENV{VULKAN_DIR})
    set(vulkan_lib ${Vulkan_DIR}lib/libvulkan.1.dylib)

    set(MACOSX_BUNDLE_BUNDLE_NAME "${TARGET_NAME}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.xhz.visualscene")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${TARGET_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CSJGL_SOURCES}
        ${CSJRENDER_SOURCES}
        ${RUNTIME_SOURCE_FILE}
        ${TOOL_SOURCE_FILES}
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
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)                        
target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader)
target_link_libraries(${TARGET_NAME} PRIVATE Qt::Widgets Qt::Core Qt::Gui)
target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:json11>)

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${JSON_INCLUDE}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${GLM_INCLUDE}>)
target_include_directories(${TARGET_NAME} PUBLIC ${vulkan_DIR}/Include)

add_custom_command(TARGET ${TARGET_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${OUTPUT_DIR}
                   COMMENT "Copy target ${TARGET} to output directory" )

if (CMAKE_HOST_WIN32)
foreach(file ${DYNAMIC_LIBS})
    add_custom_command(TARGET ${TARGET_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${file} ${OUTPUT_DIR})
endforeach()
endif()

if(CMAKE_HOST_WIN32)
    install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib
    )

    install(DIRECTORY ${RESOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/resources
    )
elseif(CMAKE_HOST_APPLE)
    if(NOT CSJ_BUILD_PACKAGE)
        add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${RESOURCE_DIR} 
                "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Resources"
                COMMENT "Copy resource diretory ..."
        )
    else()
        set_target_properties(${TARGET_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH ON
            INSTALL_RPATH "@executable_path/../Frameworks"
        )

        set_target_properties(${TARGET_NAME} PROPERTIES
            MACOSX_RPATH ON
            INSTALL_NAME_DIR "@rpath"
        )

        install(TARGETS ${TARGET_NAME}
            BUNDLE DESTINATION bin
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

        install(FILES ${vulkan_lib} ${Vulkan_DIR}lib/libvulkan.1.4.304.dylib
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app/Contents/Frameworks
            COMPONENT Runtime
        )

        install(CODE "
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app/Contents/Frameworks/libvulkan.1.4.304.dylib
                ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app/Contents/Frameworks/libvulkan.1.dylib

                COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app/Contents/Frameworks/libvulkan.1.dylib
                ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app/Contents/Frameworks/libvulkan.dylib
            )"
        )

        install(CODE "
            execute_process(
                COMMAND ${QTDIR}/bin/macdeployqt \"${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app\"
                        -always-overwrite
                        -verbose=1
                WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
                RESULT_VARIABLE deploy_result)
                
                if(NOT deploy_result EQUAL 0)
                    message(STATUS \"Failed to run macdeployqt: ${deploy_result})\")
                endif()"
            COMPONENT Runtime
        )    

        install(DIRECTORY ${RESOURCE_DIR}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app/Contents/Resources
            COMPONENT Runtime
        )

        # Apple Development: zxhas@foxmail.com (Q4LVL56W34)
        set(DEVELOPER_ID "Apple Development: zxhas@foxmail.com (Q4LVL56W34)")
        # 构建完成后执行签名
        # add_custom_command(
        #     TARGET ${TARGET_NAME}
        #     POST_BUILD
        #     COMMAND(codesign --force --deep --sign "${DEVELOPER_ID}"
        #             "${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.app"
        #         COMMENT "Signing ${TARGET_NAME}.app with developer certificate"
        #     )
        # )
    endif()
else()
endif()