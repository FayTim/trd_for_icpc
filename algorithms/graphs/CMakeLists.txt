add_executable(graphs_test test.cpp)
target_compile_features(graphs_test PRIVATE cxx_std_20)
set_target_properties(graphs_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(graphs_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME graphs COMMAND graphs_test)
set_tests_properties(graphs PROPERTIES TIMEOUT 60)

add_executable(graphs_example example.cpp)
target_compile_features(graphs_example PRIVATE cxx_std_20)
set_target_properties(graphs_example PROPERTIES CXX_EXTENSIONS OFF)