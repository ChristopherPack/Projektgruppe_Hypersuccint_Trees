find_package(Python COMPONENTS Development.Embed REQUIRED)
message("Python_FOUND: ${Python_FOUND}")
message("Python_Interpreter_FOUND: ${Python_Interpreter_FOUND}")
message("Python_EXECUTABLE: ${Python_EXECUTABLE}")
message("Python_INTERPRETER_ID: ${Python_INTERPRETER_ID}")
message("Python_STDLIB: ${Python_STDLIB}")
message("Python_STDARCH: ${Python_STDARCH}")
message("Python_SITELIB: ${Python_SITELIB}")
message("Python_SITEARCH: ${Python_SITEARCH}")
message("Python_SOABI: ${Python_SOABI}")
message("Python_Development_FOUND: ${Python_Development_FOUND}")
message("Python_Development: ${Python_Development}")
message("Python_INCLUDE_DIRS: ${Python_INCLUDE_DIRS}")
message("Python_LINK_OPTIONS: ${Python_LINK_OPTIONS}")
message("Python_LIBRARIES: ${Python_LIBRARIES}")
message("Python_LIBRARY_DIRS: ${Python_LIBRARY_DIRS}")
message("Python_RUNTIME_LIBRARY_DIRS: ${Python_RUNTIME_LIBRARY_DIRS}")
message("Python_VERSION: ${Python_VERSION}")
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
target_link_libraries(pht_lib PRIVATE irrxml)
target_link_libraries(pht_lib PUBLIC Python::Python)
target_include_directories(pht_lib PUBLIC "${PROJECT_SOURCE_DIR}/include")
