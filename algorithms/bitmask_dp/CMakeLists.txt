add_executable(bitmask_dp_test test.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(bitmask_dp_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME bitmask_dp COMMAND bitmask_dp_test)
set_tests_properties(bitmask_dp PROPERTIES TIMEOUT 60)

add_executable(bitmask_dp_example example.cpp)
