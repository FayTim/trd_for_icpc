add_executable(z_function_test test.cpp)
target_compile_features(z_function_test PRIVATE cxx_std_20)
set_target_properties(z_function_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(z_function_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME z_function COMMAND z_function_test)
set_tests_properties(z_function PROPERTIES TIMEOUT 60)

add_executable(z_function_example example.cpp)
target_compile_features(z_function_example PRIVATE cxx_std_20)
set_target_properties(z_function_example PROPERTIES CXX_EXTENSIONS OFF)