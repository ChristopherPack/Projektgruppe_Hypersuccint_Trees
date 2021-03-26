set(PHT_LIB_SOURCES
   ""
)
list(TRANSFORM PHT_LIB_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_library(pht_lib INTERFACE) #STATIC
target_sources(pht_lib INTERFACE ${PHT_LIB_SOURCES}) #PRIVATE
target_include_directories(pht_lib INTERFACE "${PROJECT_SOURCE_DIR}/include") #PUBLIC