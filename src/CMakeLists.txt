set(PHT_LIB_SOURCES
    xml_reader.cpp
    hst_node.cpp
    hst_output.cpp
    hypersuccinct_tree.cpp
    bitvector_utils.cpp
    hypersuccinct_tree_queries.cpp
    logger.cpp
    timer.cpp
)
list(TRANSFORM PHT_LIB_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_library(pht_lib SHARED)
target_sources(pht_lib PRIVATE ${PHT_LIB_SOURCES})
target_link_libraries(pht_lib PRIVATE irrxml)
target_link_libraries(pht_lib PUBLIC succinct_bv)
target_link_libraries(pht_lib PUBLIC thread-pool-2.0.0)
target_include_directories(pht_lib PUBLIC "${PROJECT_SOURCE_DIR}/include")
