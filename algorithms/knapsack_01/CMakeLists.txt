add_executable(knapsack_01_test test.cpp)
target_compile_features(knapsack_01_test PRIVATE cxx_std_20)
set_target_properties(knapsack_01_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(knapsack_01_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME knapsack_01 COMMAND knapsack_01_test)
set_tests_properties(knapsack_01 PROPERTIES TIMEOUT 60)

add_executable(knapsack_01_example example.cpp)
target_compile_features(knapsack_01_example PRIVATE cxx_std_20)
set_target_properties(knapsack_01_example PROPERTIES CXX_EXTENSIONS OFF)
