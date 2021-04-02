#Static version
file(GLOB phtlib_source CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/pht/*.h" "${CMAKE_SOURCE_DIR}/include/pht/*.hpp" "${CMAKE_SOURCE_DIR}/include/pht/*.c" "${CMAKE_SOURCE_DIR}/include/pht/*.cpp" "${CMAKE_SOURCE_DIR}/include/pht/*.cc" "*.h" "*.hpp" "*.c" "*.cpp" "*.cc")
add_library(pht_lib STATIC ${phtlib_source})
target_include_directories(pht_lib PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_compile_features(pht_lib PUBLIC cxx_std_17)
target_link_libraries(pht_lib PRIVATE irrxml)