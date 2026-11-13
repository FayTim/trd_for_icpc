add_executable(mo_test mo_test.cpp)
add_test(NAME mo COMMAND mo_test)

add_executable(mo_advanced_test advanced_test.cpp)
add_test(NAME mo_advanced COMMAND mo_advanced_test)

add_executable(mo_usecases_test usecases_test.cpp)
add_test(NAME mo_usecases COMMAND mo_usecases_test)

set_tests_properties(mo mo_advanced mo_usecases PROPERTIES TIMEOUT 60)

add_executable(mo_example example.cpp)
add_executable(mo_variants_example variants_example.cpp)
