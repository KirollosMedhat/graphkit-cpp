# graphkit-cpp

A C++ graph library built from scratch, implementing common graph algorithms on a weighted, undirected graph.

> **Status:** work in progress. Graph basics, Dijkstra (distances and routes), A*, DFS/BFS traversal, DFS/BFS route finding, `.dot` export and input validation are working, with assert-based tests. Bellman-Ford and directed edges are planned.

## Features
- Weighted adjacency-list graph. Nodes are integer IDs (`graph::NodeID`) created on first use or with `add_node`, so a node can exist with no edges
- Edge weights are `double` (`graph::Weight`), finite and non-negative. An edge added without a weight gets weight 1
- Undirected edges
- Dijkstra's shortest path: distances to every node (`dijkstra`) and the lowest-weight route to one node (`path_dijkstra`)
- A* search with a caller-supplied heuristic (`path_astar`)
- Depth-first and breadth-first traversal (`dfs`, `bfs`)
- Route finding between two nodes (`path_dfs`, `path_bfs`)
- Input validation: nodes that don't exist and invalid weights are reported with `std::invalid_argument`
- Export to Graphviz `.dot` format for visualization
- Everything lives in the `graphkit` namespace, and the library doesn't pull anything from `std` into your code

## Usage
The library is in the `graphkit` namespace. In a `.cpp` file you can write `graphkit::graph` everywhere, or bring the one name in with `using graphkit::graph;` (put that line in `.cpp` files only, never in a header). The examples below, and the rest of this README, use the short form `graph`.

```cpp
#include "graph.h"

using graphkit::graph;                  // or write graphkit::graph below

graph g;
g.add_edge(1, 2, 7);
g.add_edge(2, 3, 5.5);
g.add_edge(1, 3);                       // no weight given, defaults to 1
g.add_node(9);                          // a node with no edges

// Dijkstra: lowest total weight from node 1 to every node
auto distances = g.dijkstra(1);
for (const auto& [node, dist] : distances) {
    std::cout << "1 -> " << node << " : " << dist << "\n";   // unreachable nodes print as inf
}

// Traversal: the order in which nodes are visited
std::vector<int> dfs_order = g.dfs(1);
std::vector<int> bfs_order = g.bfs(1);

// Routes: an actual start-to-destination path (empty if unreachable)
std::vector<int> any_route      = g.path_dfs(1, 3);   // a route, not necessarily short
std::vector<int> fewest_edges   = g.path_bfs(1, 3);   // fewest edges

// Lowest-weight route, with its total weight
graph::PathResult best = g.path_dijkstra(1, 3);
// best.path                  the route from 1 to 3 (empty if unreachable)
// best.distance              its total weight (graph::unreachable if there is no route)
// best.numberOfExploredNodes how many entries the search took from its priority queue

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

// A node that isn't in the graph is an error, not an empty result
try {
    g.path_dijkstra(1, 99);             // there is no node 99
} catch (const std::invalid_argument& e) {
    std::cout << e.what() << "\n";      // No Node in graph with ID: 99
}

g.export_dot("graph");                  // writes graph.dot (the .dot extension is added)
```

To render the exported graph as an image (requires [Graphviz](https://graphviz.org)):
```
dot -Tpng -Grankdir=LR graph.dot -o graph.png
```

## Error handling
- **A node that doesn't exist.** `dijkstra`, `dfs`, `bfs`, `path_dfs`, `path_bfs`, `path_dijkstra` and `path_astar` throw `std::invalid_argument` if a node you pass in isn't in the graph. The message names the ID, for example `No Node in graph with ID: 99`. The route functions check the start and the destination separately.
- **No route is not an error.** If both nodes exist but nothing connects them, the route functions return an empty path, and `path_dijkstra` and `path_astar` set `distance` to `graph::unreachable` (positive infinity). A node created with `add_node` and given no edges is such a case. `dijkstra` holds `graph::unreachable` for every node it can't reach.
- **Weights.** `add_edge` throws `std::invalid_argument` for a weight that is negative, NaN or infinite. Zero is allowed. Dijkstra and A* only work with non-negative weights, so this is checked when the edge is added.
- **A rejected call changes nothing.** `add_edge` validates before it modifies the graph, so a call that throws doesn't leave a half-added edge or node behind.
- **Node IDs** can be any `int`, including negative ones. They aren't validated.

## Build
Requires CMake 3.20+ and a C++20 compiler.
```
cmake -S . -B build
cmake --build build
```
This builds four programs in `build/`:
- `GraphKit`: the demo. It shows DFS and BFS traversal, DFS/BFS/Dijkstra/A* routes on a 60-node graph, and an A* example on a grid.
- `BenchDijkstra`: times Dijkstra on a random connected graph of 50,000 nodes.
- `BenchAStar`: compares A* (Manhattan heuristic) with Dijkstra on grid graphs: the routes on a small map, and the time and the nodes explored over 100 routes on a 200x200 grid with obstacles.
- `Tests`: the automated tests (see below).

Timings are only meaningful in an optimized build. Configure with `-DCMAKE_BUILD_TYPE=Release` before running the benchmarks.

## Tests
The tests are in `tests/test_graph.cpp`. They use plain `assert` and cover Dijkstra, A*, DFS and BFS: routes, distances, visit order, unreachable nodes, isolated nodes (`add_node`), the exceptions for missing nodes and invalid weights, negative node IDs, and edge cases (start equal to destination, zero-weight edges, duplicate edges, self-loops).
```
cmake --build build --target Tests
./build/Tests          # on Windows: build\Tests.exe
```
If every assertion holds, the program prints `All tests passed` and exits with code 0. A failing assertion stops the program and prints the expression and line number. The test file starts with `#undef NDEBUG`, so the assertions stay active even in a Release build.

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
- All names are in the `graphkit` namespace. Nested names such as `graph::PathResult`, `graph::NodeID`, `graph::Weight` and `graph::unreachable` are written here without the `graphkit::` prefix, as if `using graphkit::graph;` were in effect. Without that line, write `graphkit::graph::PathResult` and so on.
- `dfs` and `bfs` return the **order in which nodes are visited**, not a route. Consecutive entries aren't necessarily connected by an edge.
- `path_dfs` returns **a** route from start to destination, which is usually not the shortest. `path_bfs` returns a route with the **fewest edges**. Neither uses edge weights.
- `path_dijkstra` returns the route with the **lowest total weight**, plus that weight. It can differ from the `path_bfs` route: the fewest edges isn't always the lowest weight.
- `path_astar` returns the same lowest-weight route as `path_dijkstra`, but it examines fewer nodes when the heuristic is informative. The heuristic is a `std::function<graph::Weight(graph::NodeID)>`: a node ID in, an estimate of the remaining distance to the destination out, in the same units as the edge weights. The graph has no coordinates, so the caller supplies it.
- For `path_astar` to return the lowest-weight route, the heuristic must never overestimate the true remaining distance, must return the same value every time it is called with the same node, and must be 0 at the destination. An overestimating heuristic can be faster but may return a longer route. The default heuristic is 0.
- `numberOfExploredNodes` counts the entries `path_dijkstra` and `path_astar` take from their priority queue, including outdated entries that are skipped. It measures work without depending on the speed of the machine, so it is a fair way to compare the two algorithms.
- `path_dijkstra` and `path_astar` set up distances lazily: only nodes the search reaches are touched, so a call costs time proportional to the search and not to the whole graph.
- `add_edge` doesn't check for duplicates, so adding the same edge twice stores two parallel edges.
- `export_dot(name)` writes `name.dot`. Pass the name without the extension: `export_dot("graph.dot")` creates `graph.dot.dot`.

## Performance
`bench/bench_astar.cpp` (the `BenchAStar` target) compares Dijkstra and A* with a Manhattan heuristic on 100 random routes across a 200x200 grid with 25% obstacles. The grid and the routes are fixed by a seed, so every run uses the same ones, and both algorithms return the same distance for every route.

| Algorithm | Total time for 100 routes | Nodes explored (100 routes) |
|---|---|---|
| Dijkstra | 3022 ms | 1,556,459 |
| A* (Manhattan heuristic) | 382 ms | 189,681 |

A* is about 8 times faster here, because the heuristic steers it toward the goal and it explores about 8 times fewer nodes. The absolute times depend on the machine and the build, so look at the ratio, and use a Release build when you run it yourself.

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
- [x] `add_node` and isolated nodes
- [x] Input validation (exceptions for missing nodes and invalid weights)
- [x] `graphkit` namespace
- [ ] Directed edges
- [ ] Bellman-Ford (negative weights)

## Development notes
This is a learning project.

The library code in `src/graph.h`, `src/graph.cpp` and `tests/test_graph.cpp` is my own: I wrote and traced each algorithm by hand, using an AI assistant for code review and test cases. `src/main.cpp`, `src/sample_graph.h` and the programs in `bench/` were generated by AI, and so was the first draft of this README.