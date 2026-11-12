add_executable(scc_test test.cpp)
target_compile_features(scc_test PRIVATE cxx_std_20)
set_target_properties(scc_test PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(scc_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME scc COMMAND scc_test)
set_tests_properties(scc PROPERTIES TIMEOUT 60)

add_executable(scc_example example.cpp)
target_compile_features(scc_example PRIVATE cxx_std_20)
set_target_properties(scc_example PROPERTIES CXX_EXTENSIONS OFF)
