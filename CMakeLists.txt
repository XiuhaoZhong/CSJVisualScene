cmake_minimum_required(VERSION 3.16)

set(TARGET_NAME CSJVisualScene)

option(CSJ_BUILD_DEBUG "Build ON Debug" ON)
option(CSJ_BUILD_PACKAGE "Build Package" OFF)

project(${TARGET_NAME} VERSION 0.1 LANGUAGES CXX)

set(PROJ_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# set the resource directory in the output directory
set(BUILD_DIR "${PROJ_ROOT_DIR}/build")

set(OUTPUT_DIR "${PROJ_ROOT_DIR}/output")
file(MAKE_DIRECTORY ${OUTPUT_DIR})
# set(OUTPUT_RESOURCE_DIR "${OUTPUT_DIR}/resource")
set(THIRDPARTS_DIR "${PROJ_ROOT_DIR}/src/3rdParts")

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

if(CSJ_BUILD_DEBUG)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Debug/install" CACHE PATH "INSTALL_DIR" FORCE)
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Release/install" CACHE PATH "INSTALL_DIR" FORCE)
endif()
add_subdirectory(src/3rdParts)
add_subdirectory(src)