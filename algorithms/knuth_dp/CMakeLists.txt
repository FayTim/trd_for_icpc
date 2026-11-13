add_executable(knuth_dp_test test.cpp)
add_test(NAME knuth_dp COMMAND knuth_dp_test)
set_tests_properties(knuth_dp PROPERTIES TIMEOUT 60)

add_executable(knuth_dp_example example.cpp)
