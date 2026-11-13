add_executable(grouped_knapsack_test test.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(grouped_knapsack_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME grouped_knapsack COMMAND grouped_knapsack_test)
set_tests_properties(grouped_knapsack PROPERTIES TIMEOUT 60)

add_executable(grouped_knapsack_example example.cpp)
