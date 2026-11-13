add_executable(two_sat_test test.cpp)
target_compile_features(two_sat_test PRIVATE cxx_std_20)
set_target_properties(two_sat_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(two_sat_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME two_sat COMMAND two_sat_test)
set_tests_properties(two_sat PROPERTIES TIMEOUT 60)

add_executable(two_sat_example example.cpp)
target_compile_features(two_sat_example PRIVATE cxx_std_20)
set_target_properties(two_sat_example PROPERTIES CXX_EXTENSIONS OFF)
