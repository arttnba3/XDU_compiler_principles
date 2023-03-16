cmake_minimum_required(VERSION 3.20)
project(A3DrawLang)

set(CMAKE_CXX_STANDARD 17)

AUX_SOURCE_DIRECTORY(. SRC_DIRS)

set(CMAKE_CXX_STANDARD 17)
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("include")

add_subdirectory("lib")
add_subdirectory("compiler")

add_executable(A3DrawLang main.cpp)

# Options for debugging only
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

target_link_libraries(A3DrawLang Lib Compiler)