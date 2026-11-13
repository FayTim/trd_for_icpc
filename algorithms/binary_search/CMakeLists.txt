add_executable(binary_search_test test.cpp)
add_test(NAME binary_search COMMAND binary_search_test)
set_tests_properties(binary_search PROPERTIES TIMEOUT 60)

add_executable(binary_search_example example.cpp)
