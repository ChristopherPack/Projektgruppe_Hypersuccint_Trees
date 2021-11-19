#cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
#project(thread-pool-2.0.0)

#set (CMAKE_CXX_STANDARD 17)

file(GLOB thread-pool_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.h" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.hpp" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.c" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.cpp" "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0/*.cc" "*.h" "*.hpp" "*.c" "*.cpp" "*.cc")
add_library(thread-pool-2.0.0 STATIC ${thread-pool_src})
target_include_directories(thread-pool-2.0.0 PUBLIC "${CMAKE_SOURCE_DIR}/extern/thread-pool-2.0.0")
target_compile_features(thread-pool-2.0.0 PUBLIC cxx_std_17)