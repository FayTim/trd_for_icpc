add_executable(segment_tree_test test.cpp)
target_compile_features(segment_tree_test PRIVATE cxx_std_20)
set_target_properties(segment_tree_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(segment_tree_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME segment_tree COMMAND segment_tree_test)
set_tests_properties(segment_tree PROPERTIES TIMEOUT 60)

add_executable(segment_tree_example example.cpp)
target_compile_features(segment_tree_example PRIVATE cxx_std_20)
set_target_properties(segment_tree_example PROPERTIES CXX_EXTENSIONS OFF)

add_executable(range_add_min_example range_add_min_example.cpp)
target_compile_features(range_add_min_example PRIVATE cxx_std_20)
set_target_properties(range_add_min_example PROPERTIES CXX_EXTENSIONS OFF)
