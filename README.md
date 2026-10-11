# graphkit-cpp

A C++ graph library built from scratch, implementing common graph algorithms on a weighted, undirected graph.

> **Status:** work in progress. Graph basics, Dijkstra, DFS/BFS traversal, DFS/BFS route finding and `.dot` export are working. A*, MST and Bellman-Ford are planned.

## Features
- Weighted adjacency-list graph (nodes are integer IDs, created on first use)
- Undirected edges; an edge added without a weight gets weight 1
- Dijkstra's shortest path distances
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
This builds two programs in `build/`:
- `GraphKit`: the demo. It compares DFS and BFS routes on a few graphs and prints Dijkstra distances.
- `Bench`: times Dijkstra on a random connected graph of 50,000 nodes.

## Project layout
```
src/
  graph.h, graph.cpp     the library
  main.cpp               demo program
  sample_graph.h         60-node graph used by the demo
bench/
  bench_dijkstra.cpp     Dijkstra benchmark
```

## Notes
- `dfs` and `bfs` return the **order in which nodes are visited**, not a route. Consecutive entries aren't necessarily connected by an edge.
- `path_dfs` returns **a** route from start to destination, which is usually not the shortest. `path_bfs` returns a route with the **fewest edges**.
- Neither `path_dfs` nor `path_bfs` uses edge weights. For the lowest total weight, use `dijkstra`, which currently returns distances only, not routes.
- If the start or destination node isn't in the graph, the traversal and route functions print a message and return an empty vector. An unreachable destination also gives an empty vector.
- In `dijkstra`, nodes that can't be reached from the source keep the value `std::numeric_limits<int>::max()`.
- `add_edge` doesn't check for duplicates, so adding the same edge twice stores two parallel edges.

## Complexity
| Function | Time |
|---|---|
| `dfs`, `bfs`, `path_dfs`, `path_bfs` | O(V + E) |
| `dijkstra` | O((V + E) log V) |

## Roadmap
- [x] Weighted graph + adjacency list
- [x] Dijkstra's algorithm
- [x] `.dot` export
- [x] DFS and BFS traversal
- [x] Route finding (`path_dfs`, `path_bfs`)
- [ ] Dijkstra route extraction (return the path, not just distances)
- [ ] A* search
- [ ] Minimum spanning tree (Prim's / Kruskal's)
- [ ] Bellman-Ford (negative weights)