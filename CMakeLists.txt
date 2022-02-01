cmake_minimum_required(VERSION 3.16.1)
project(project)
# Project setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# file(COPY etc  DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  add_compile_options(
    -Wall
    -Wpedantic
    -Wextra
    -Wconversion
    -Wsign-conversion
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O3>)
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif(CCACHE_FOUND)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if(BUILD_SHARED_LIBS)
  set(LIBRARY_LINKAGE SHARED)
else()
  set(LIBRARY_LINKAGE STATIC)
  set(Boost_USE_STATIC_LIBS ON)
endif()

include_directories(/usr/include)
include_directories(include)

include_directories(/usr/local/include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(3rdparty)

add_subdirectory(src)
