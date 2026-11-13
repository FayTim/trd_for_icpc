add_executable(tree_dp_test test.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(tree_dp_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME tree_dp COMMAND tree_dp_test)
set_tests_properties(tree_dp PROPERTIES TIMEOUT 60)

add_executable(tree_dp_example example.cpp)
