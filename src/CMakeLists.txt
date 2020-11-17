#Static version
#file(GLOB pstlib_source CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/pht/*.h" "${CMAKE_SOURCE_DIR}/include/pht/*.hpp" "${CMAKE_SOURCE_DIR}/include/pht/*.c" "${CMAKE_SOURCE_DIR}/include/pht/*.cpp" "${CMAKE_SOURCE_DIR}/include/pht/*.cc" "*.h" "*.hpp" "*.c" "*.cpp" "*.cc")
#add_library(pst_lib STATIC ${pstlib_source})
#target_include_directories(pst_lib PUBLIC "${CMAKE_SOURCE_DIR}/include")
#target_compile_features(pst_lib PUBLIC cxx_std_17)

#Header-only version
file(GLOB pstlib_headers CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/pht/*.h" "${CMAKE_SOURCE_DIR}/include/pht/*.hpp" "*.h" "*.hpp")
add_library(pst_lib INTERFACE)
target_sources(pst_lib INTERFACE ${pstlib_headers})
target_include_directories(pst_lib INTERFACE "${CMAKE_SOURCE_DIR}/include")