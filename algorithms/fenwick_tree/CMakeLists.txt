add_executable(fenwick_tree_test test.cpp)
add_test(NAME fenwick_tree COMMAND fenwick_tree_test)
set_tests_properties(fenwick_tree PROPERTIES TIMEOUT 60)

add_executable(fenwick_tree_example example.cpp)
