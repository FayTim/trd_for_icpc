add_executable(combinatorics_test test.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(combinatorics_test PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_test(NAME combinatorics COMMAND combinatorics_test)
set_tests_properties(combinatorics PROPERTIES TIMEOUT 60)

add_executable(combinatorics_example example.cpp)
