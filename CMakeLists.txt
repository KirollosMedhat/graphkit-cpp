cmake_minimum_required(VERSION 3.20)
project(GraphKit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GraphKit src/main.cpp src/graph.cpp)

add_executable(BenchDijkstra bench/bench_dijkstra.cpp src/graph.cpp)
target_include_directories(BenchDijkstra PRIVATE src)

add_executable(BenchAStar bench/bench_astar.cpp src/graph.cpp)
target_include_directories(BenchAStar PRIVATE src)

add_executable(Tests tests/test_graph.cpp src/graph.cpp)
target_include_directories(Tests PRIVATE src)