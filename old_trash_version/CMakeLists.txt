cmake_minimum_required(VERSION 3.20)
project(A3DrawLang)

set(CMAKE_CXX_STANDARD 17)

AUX_SOURCE_DIRECTORY(. SRC_DIRS)

set(CMAKE_CXX_STANDARD 17)
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(message)
add_subdirectory(parser)
add_subdirectory(scanner)
add_subdirectory(semantic)

add_executable(A3DrawLang main.cpp)

target_link_libraries(A3DrawLang Message Parser Scanner Semantic)