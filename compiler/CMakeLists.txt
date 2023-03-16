include_directories("include")

add_subdirectory("scanner")
add_subdirectory("symbol")
add_subdirectory("syntactic")
add_subdirectory("semantic")

aux_source_directory(. COMPILER_DIR)
add_library(Compiler ${COMPILER_DIR})

target_link_libraries(Compiler Scanner Symbol Syntactic Semantic)