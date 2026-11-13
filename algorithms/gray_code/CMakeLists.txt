add_executable(gray_code_test test.cpp)
add_test(NAME gray_code COMMAND gray_code_test)
set_tests_properties(gray_code PROPERTIES TIMEOUT 60)

add_executable(gray_code_example example.cpp)
