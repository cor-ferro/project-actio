cmake_minimum_required(VERSION 3.0)

project(actio)

set(GRAPHIC_API OPENGL)
set(PHYSX_CONFIGURATION CHECKED)
set(PHYSX_ARCH x64)
set(BUILD_TYPE debug)
set(OPTIMIZE_LEVEL 0)
set(RESOURCE_DIR '"./resources"')
set(SHADERS_DIR '"./shaders"')
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(PROJECT_INSTALL "${CMAKE_SOURCE_DIR}/build")

option(COPY_LIBS "Copy all libs" OFF)

add_definitions(-DNDEBUG)
add_definitions(-DPROFILE_ENABLED)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGRAPHIC_API=${GRAPHIC_API})
add_definitions(-DGRAPHIC_API_OPENGL)
add_definitions(-DGLFW_INCLUDE_NONE)
add_definitions(-DRESOURCE_DIR="resources")
add_definitions(-DSHADERS_DIR="./shaders")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(vendor)
include_directories(vendor/glm)
include_directories(vendor/assimp/include)
include_directories(vendor/soil/src)
include_directories(vendor/DevIL/DevIL/include)
include_directories(vendor/glfw/include)
include_directories(vendor/glad/include)
include_directories(vendor/imgui)
include_directories(vendor/boost_1_65_1)
include_directories(vendor/PhysX-3.4/PhysX_3.4/Include)
include_directories(vendor/PhysX-3.4/PxShared/include)
include_directories(vendor/ozz-animation/include)
include_directories(vendor/entityx)
include_directories(vendor/yaml-cpp/include)
include_directories(vendor/luabridge/Source)
include_directories(include/lua/include)

file(GLOB actio_src
        src/*.cpp
        src/animation/*.cpp
        src/app/*.cpp
        src/cameras/*.cpp
        src/core/*.cpp
        src/game/*.cpp
        src/game/components/*.cpp
        src/game/desc/*.cpp
        src/game/events/*.cpp
        src/game/systems/*.cpp
        src/helpers/*.cpp
        src/lib/*.cpp
        src/lights/*.cpp
        src/materials/*.cpp
        src/math/*.cpp
        src/memory/*.cpp
        src/renderer/*.cpp
        src/renderer/opengl/*.cpp
        src/resources/*.cpp
        src/scenes/*.cpp
        )

add_executable(actio ${actio_src})

target_link_libraries(actio
        ${CMAKE_SOURCE_DIR}/bin/linux64/libSOIL.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libiniparser.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libimgui.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libPhysX3ExtensionsCHECKED.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_animation_offline.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_animation.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_base.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_geometry.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_options.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_animation_offline_anim_tools.a
        ${CMAKE_SOURCE_DIR}/bin/linux64/libozz_animation_offline_skel_tools.a)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads)

link_libraries(${X11_LIBRARIES})
link_libraries(${OPENGL_LIBRARIES})
link_libraries(${ZLIB_LIBRARIES})

SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH ${CMAKE_SOURCE_DIR}/libs/linux64)

foreach (lib glfw jasper entityx assimp IL ILU ILUT yaml-cpp lua53)
    add_library(${lib} SHARED IMPORTED)
    set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/linux64/lib${lib}.so")
endforeach (lib)

foreach (lib boost_thread boost_system boost_timer boost_chrono boost_date_time boost_filesystem boost_regex)
    add_library(${lib} SHARED IMPORTED)
    set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/linux64/lib${lib}.so")
endforeach (lib)

#PhysX3Gpu
foreach (lib PxFoundation PhysX3Common PhysX3 PhysX3Cooking PxPvdSDK PhysX3CharacterKinematic)
    add_library(${lib} SHARED IMPORTED src/game/systems/base.cpp)
    set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION "${PROJECT_INSTALL}/lib${lib}${PHYSX_CONFIGURATION}_${PHYSX_ARCH}.so")
    target_link_libraries(actio ${lib})
endforeach (lib)

target_link_libraries(actio
        ${CMAKE_DL_LIBS}
        glfw
        jasper
        IL
        ILU
        ILUT
        entityx
        assimp
        yaml-cpp
        lua53
        boost_thread
        boost_system
        boost_timer
        boost_chrono
        boost_date_time
        boost_filesystem
        boost_regex
        -lpthread -lm)

install(TARGETS actio DESTINATION ${PROJECT_INSTALL})
