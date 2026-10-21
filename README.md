# graphkit-cpp

A C++ graph library built from scratch, implementing common graph algorithms on a weighted, undirected graph.

> **Status:** work in progress. Graph basics, Dijkstra (distances and routes), A*, DFS/BFS traversal, DFS/BFS route finding and `.dot` export are working, with assert-based tests. MST and Bellman-Ford are planned.

## Features
- Weighted adjacency-list graph (nodes are integer IDs, created on first use)
- Undirected edges; an edge added without a weight gets weight 1
- Dijkstra's shortest path: distances to every node (`dijkstra`) and the lowest-weight route to one node (`path_dijkstra`)
- A* search with a caller-supplied heuristic (`path_astar`)
- Depth-first and breadth-first traversal (`dfs`, `bfs`)
- Route finding between two nodes (`path_dfs`, `path_bfs`)
- Export to Graphviz `.dot` format for visualization

## Usage
```cpp
graph g;
g.add_edge(1, 2, 7);
g.add_edge(2, 3, 5);
g.add_edge(1, 3);                       // no weight given, defaults to 1

// Dijkstra: lowest total weight from node 1 to every node
auto distances = g.dijkstra(1);
for (const auto& [node, dist] : distances) {
    std::cout << "1 -> " << node << " : " << dist << "\n";
}

// Traversal: the order in which nodes are visited
std::vector<int> dfs_order = g.dfs(1);
std::vector<int> bfs_order = g.bfs(1);

// Routes: an actual start-to-destination path (empty if unreachable)
std::vector<int> any_route      = g.path_dfs(1, 3);   // a route, not necessarily short
std::vector<int> fewest_edges   = g.path_bfs(1, 3);   // fewest edges

// Lowest-weight route, with its total weight
graph::PathResult best = g.path_dijkstra(1, 3);
// best.path     the route from 1 to 3 (empty if unreachable)
// best.distance its total weight

// A*: the same lowest-weight route, guided by an estimate of the remaining distance.
// The estimate takes a node ID and returns a number. Here it is a Manhattan distance
// on a grid whose node IDs are y * width + x (`grid` is a graph built that way;
// see bench/bench_astar.cpp for a full example).
int width = 20, goal = 117;
auto manhattan = [=](int id) {
    return std::abs(id % width - goal % width) + std::abs(id / width - goal / width);
};
graph::PathResult route = grid.path_astar(103, goal, manhattan);
// Without a heuristic, path_astar(start, goal) behaves like path_dijkstra.

g.export_dot();                         // writes graph.dot
```

To render the exported graph as an image (requires [Graphviz](https://graphviz.org)):
```
dot -Tpng -Grankdir=LR graph.dot -o graph.png
```

## Build
Requires CMake 3.20+ and a C++20 compiler.
```
cmake -S . -B build
cmake --build build
```
This builds four programs in `build/`:
- `GraphKit`: the demo. It shows DFS and BFS traversal, DFS/BFS/Dijkstra/A* routes on a 60-node graph, and an A* example on a grid.
- `Bench`: times Dijkstra on a random connected graph of 50,000 nodes.
- `BenchAStar`: compares A* (Manhattan heuristic) with Dijkstra on grid graphs: the routes on a small map, and the total time over 100 routes on a 200x200 grid with obstacles.
- `Tests`: the automated tests (see below).

Timings are only meaningful in an optimized build. Configure with `-DCMAKE_BUILD_TYPE=Release` before running the benchmarks.

## Tests
The tests are in `tests/test_graph.cpp`. They use plain `assert` and cover Dijkstra, A*, DFS and BFS: routes, distances, visit order, unreachable and missing nodes, and edge cases (start equal to destination, zero-weight edges, duplicate edges, self-loops).
```
cmake --build build --target Tests
./build/Tests          # on Windows: build\Tests.exe
```
If every assertion holds, the program exits with code 0. A failing assertion stops the program and prints the expression and line number. The test file starts with `#undef NDEBUG`, so the assertions stay active even in a Release build.

## Project layout
```
src/
  graph.h, graph.cpp     the library
  main.cpp               demo program
  sample_graph.h         60-node graph used by the demo
bench/
  bench_dijkstra.cpp     Dijkstra benchmark
  bench_astar.cpp        A* vs Dijkstra comparison on grids
tests/
  test_graph.cpp         assert-based tests
```

## Notes
- `dfs` and `bfs` return the **order in which nodes are visited**, not a route. Consecutive entries aren't necessarily connected by an edge.
- `path_dfs` returns **a** route from start to destination, which is usually not the shortest. `path_bfs` returns a route with the **fewest edges**. Neither uses edge weights.
- `path_dijkstra` returns the route with the **lowest total weight**, plus that weight. It can differ from the `path_bfs` route: the fewest edges isn't always the lowest weight.
- `path_astar` returns the same lowest-weight route as `path_dijkstra`, but it examines fewer nodes when the heuristic is informative. The heuristic is a `std::function<int(int)>`: a node ID in, an estimate of the remaining distance to the destination out, in the same units as the edge weights. The graph has no coordinates, so the caller supplies it.
- For `path_astar` to return the lowest-weight route, the heuristic must never overestimate the true remaining distance, must return the same value every time it is called with the same node, and must be 0 at the destination. An overestimating heuristic can be faster but may return a longer route. The default heuristic is 0.
- `dijkstra`, `path_dijkstra` and `path_astar` require **non-negative weights**. Negative weights aren't validated yet. On an undirected graph a negative weight can make the search give wrong results or never finish.
- If the start or destination node isn't in the graph, the traversal and route functions print a message and return an empty result. An unreachable destination also gives an empty result.
- For `path_dijkstra` and `path_astar`, an empty `path` means there is no route, and `distance` is then `std::numeric_limits<int>::max()`. Check `path.empty()` first.
- In `dijkstra`, nodes that can't be reached from the source keep the value `std::numeric_limits<int>::max()`.
- `path_dijkstra` and `path_astar` initialize every node on each call, so each call costs O(V) before the search starts. This limits how much faster A* is on large graphs.
- `add_edge` doesn't check for duplicates, so adding the same edge twice stores two parallel edges.

## Complexity
| Function | Time |
|---|---|
| `dfs`, `bfs`, `path_dfs`, `path_bfs` | O(V + E) |
| `dijkstra`, `path_dijkstra` | O((V + E) log V) |
| `path_astar` | O((V + E) log V) worst case; far fewer nodes are examined with a good heuristic |

## Roadmap
- [x] Weighted graph + adjacency list
- [x] Dijkstra's algorithm
- [x] `.dot` export
- [x] DFS and BFS traversal
- [x] Route finding (`path_dfs`, `path_bfs`)
- [x] Dijkstra route extraction (`path_dijkstra`)
- [x] A* search (`path_astar`)
- [x] Automated tests (assert-based)
- [ ] Minimum spanning tree (Prim's / Kruskal's)
- [ ] Bellman-Ford (negative weights)

## Development notes
This is a learning project.

The library code in `src/graph.h`, `src/graph.cpp` and `tests/test_graph.cpp` is my own: I wrote and traced each algorithm by hand, using an AI assistant for code review and test cases. `src/main.cpp`, `src/sample_graph.h` and the programs in `bench/` were generated by AI, and so was the first draft of this README.