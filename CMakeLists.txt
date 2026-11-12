cmake_minimum_required(VERSION 3.20)
project(trd_for_icpc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
add_subdirectory(algorithms/fixed_set)
add_subdirectory(algorithms/segment_tree)
add_subdirectory(algorithms/scc)
add_subdirectory(algorithms/two_sat)
add_subdirectory(algorithms/z_function)
add_subdirectory(algorithms/polynomial_hashes)
