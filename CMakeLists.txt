cmake_minimum_required(VERSION 3.17)
project(ProjektSuccinctTrees VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_subdirectory(src pht)
add_subdirectory(example_service)
add_subdirectory(tests)
add_subdirectory(extern/googletest)
add_subdirectory(extern/irrxml)