cmake_minimum_required(VERSION 2.8)
project(jamtoysynth)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

find_package(SDL)
find_package(OpenGL)

include_directories(${SDL_INCLUDE_DIR})
include_directories(${OpenGL_INCLUDE_DIR})
find_library(GLEW_LIBRARY GLEW)

link_libraries(${OpenGL_gl_LIBRARY})
link_libraries(${SDL_LIBRARY})
link_libraries(${GLEW_LIBRARY})

add_subdirectory(src)
