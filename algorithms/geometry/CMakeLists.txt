foreach(module point segments rectangles polygons)
  add_executable(geometry_${module}_test ${module}_test.cpp)
  target_compile_features(geometry_${module}_test PRIVATE cxx_std_20)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(geometry_${module}_test PRIVATE
      -Wall -Wextra -Wpedantic)
  endif()
  add_test(NAME geometry_${module} COMMAND geometry_${module}_test)
  set_tests_properties(geometry_${module} PROPERTIES TIMEOUT 60)

  add_executable(geometry_${module}_example ${module}_example.cpp)
  target_compile_features(geometry_${module}_example PRIVATE cxx_std_20)
endforeach()
