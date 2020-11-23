cmake_minimum_required(VERSION 3.17)
project(ProjektSuccinctTrees VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(gtest_force_shared_crt True)

add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(tests)
add_subdirectory(extern/googletest)
add_subdirectory(extern/irrxml)