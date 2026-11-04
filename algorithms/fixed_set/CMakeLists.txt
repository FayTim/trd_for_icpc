add_executable(fixed_set_test test.cpp)
target_compile_features(fixed_set_test PRIVATE cxx_std_20)
set_target_properties(fixed_set_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(fixed_set_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME fixed_set COMMAND fixed_set_test)
set_tests_properties(fixed_set PROPERTIES TIMEOUT 60)

add_executable(fixed_set_example example.cpp)
target_compile_features(fixed_set_example PRIVATE cxx_std_20)
set_target_properties(fixed_set_example PROPERTIES CXX_EXTENSIONS OFF)
