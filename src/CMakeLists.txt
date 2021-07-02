set(Python_USE_STATIC_LIBS TRUE)
find_package(Python COMPONENTS Interpreter Development)

message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
message("Python_LIBRARIES:${Python_INCLUDE_DIRS}")
message("CMAKE_ROLE:${CMAKE_ROLE}")
set(PHT_LIB_SOURCES
    xml_reader.cpp
    hst_output.cpp
    hypersuccinct_tree.cpp
    bitvector_utils.cpp
    hypersuccinct_tree_queries.cpp
    logger.cpp
    timer.cpp
)
list(TRANSFORM PHT_LIB_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_library(pht_lib STATIC)
target_sources(pht_lib PRIVATE ${PHT_LIB_SOURCES})
target_link_libraries(pht_lib PRIVATE irrxml ${Python_LIBRARIES})
target_include_directories(pht_lib PUBLIC "${PROJECT_SOURCE_DIR}/include" "${Python_INCLUDE_DIRS}")