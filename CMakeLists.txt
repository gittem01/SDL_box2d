cmake_minimum_required(VERSION 3.5)

project(SDL_box2d VERSION 2.0.0)

set(APP_NAME SDL_box2d)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-DGL_SILENCE_DEPRECATION)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/box2d/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/external/box2d/include)

file(GLOB sourceFiles ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB headerFiles ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(${APP_NAME} Source.cpp ${sourceFiles} ${headerFiles})

target_link_libraries(${APP_NAME} box2d SDL2-static)
