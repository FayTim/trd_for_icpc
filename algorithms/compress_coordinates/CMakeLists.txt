add_executable(compress_coordinates_test test.cpp)
target_compile_features(compress_coordinates_test PRIVATE cxx_std_20)
set_target_properties(compress_coordinates_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(compress_coordinates_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME compress_coordinates COMMAND compress_coordinates_test)
set_tests_properties(compress_coordinates PROPERTIES TIMEOUT 60)

add_executable(compress_coordinates_example example.cpp)
target_compile_features(compress_coordinates_example PRIVATE cxx_std_20)
set_target_properties(compress_coordinates_example PROPERTIES CXX_EXTENSIONS OFF)
