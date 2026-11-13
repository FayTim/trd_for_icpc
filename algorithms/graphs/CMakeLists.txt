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

add_executable(euler_test euler_test.cpp)
add_test(NAME euler COMMAND euler_test)
set_tests_properties(euler PROPERTIES TIMEOUT 60)
add_executable(euler_example euler_example.cpp)

# Each public header is compiled without preceding includes. Linking these
# objects with test.cpp also catches definitions that violate the ODR.
set(graphs_header_sources)
foreach(header inf dfs bfs dijkstra bellman floyd toposort dsu kruskal bridges lca ternary euler)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/${header}_standalone.cpp")
    file(WRITE "${source}" "#include \"${header}.h\"\n")
    list(APPEND graphs_header_sources "${source}")
endforeach()
add_library(graphs_header_checks OBJECT ${graphs_header_sources})
target_compile_features(graphs_header_checks PRIVATE cxx_std_20)
target_include_directories(graphs_header_checks PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(graphs_test PRIVATE $<TARGET_OBJECTS:graphs_header_checks>)
