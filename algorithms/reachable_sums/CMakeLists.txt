add_executable(reachable_sums_test test.cpp)
target_compile_features(reachable_sums_test PRIVATE cxx_std_20)
set_target_properties(reachable_sums_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(reachable_sums_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME reachable_sums COMMAND reachable_sums_test)
set_tests_properties(reachable_sums PROPERTIES TIMEOUT 60)

add_executable(reachable_sums_example example.cpp)
target_compile_features(reachable_sums_example PRIVATE cxx_std_20)
set_target_properties(reachable_sums_example PROPERTIES CXX_EXTENSIONS OFF)
