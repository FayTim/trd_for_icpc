add_executable(polynomial_hashes_test test.cpp)
target_compile_features(polynomial_hashes_test PRIVATE cxx_std_20)
set_target_properties(polynomial_hashes_test PROPERTIES CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(polynomial_hashes_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME polynomial_hashes COMMAND polynomial_hashes_test)
set_tests_properties(polynomial_hashes PROPERTIES TIMEOUT 60)

add_executable(polynomial_hashes_example example.cpp)
target_compile_features(polynomial_hashes_example PRIVATE cxx_std_20)
set_target_properties(polynomial_hashes_example PROPERTIES CXX_EXTENSIONS OFF)