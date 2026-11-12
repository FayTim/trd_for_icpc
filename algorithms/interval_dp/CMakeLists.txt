add_executable(interval_dp_test test.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(interval_dp_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME interval_dp COMMAND interval_dp_test)
set_tests_properties(interval_dp PROPERTIES TIMEOUT 60)

add_executable(interval_dp_example example.cpp)
