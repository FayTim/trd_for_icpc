add_executable(contest_template ordinary.cpp)
add_executable(interactive_template interactive.cpp)
foreach(kind contest interactive)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${kind}_template PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  add_test(NAME ${kind}_template_self_test COMMAND ${kind}_template --test)
  set_tests_properties(${kind}_template_self_test PROPERTIES TIMEOUT 10)
endforeach()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME contest_template_processes
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/process_test.py
    $<TARGET_FILE:contest_template> $<TARGET_FILE:interactive_template>)
set_tests_properties(contest_template_processes PROPERTIES TIMEOUT 30)
