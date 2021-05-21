include(GoogleTest)
set(PHT_TEST_SOURCES
   bitvector_utils_test.cpp
   farzan_munro_test.cpp
   hypersuccinct_tree_test.cpp
        list_utils_test.cpp
   node_test.cpp
   unordered_tree_test.cpp
        huffman_test.cpp
)
list(TRANSFORM PHT_TEST_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_executable(pht_test)
target_sources(pht_test PRIVATE ${PHT_TEST_SOURCES})
target_link_libraries(pht_test PRIVATE pht_lib gtest gmock gtest_main gmock_main)
gtest_discover_tests(pht_test)