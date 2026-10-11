# graphkit-cpp

A C++ graph library built from scratch, implementing common graph algorithms on a weighted, undirected graph structure.

> **Status:** work in progress. Graph basics, Dijkstra, DFS, BFS and `.dot` export are working. Path extraction, A*, MST and Bellman-Ford are planned.

## Features
- Weighted adjacency-list graph representation (nodes are integer IDs)
- Undirected edges; unweighted edges default to weight 1
- Dijkstra's shortest path algorithm
- Depth-first and breadth-first traversal
- Export to Graphviz `.dot` format for visualization
- More algorithms (path extraction, MST, A*, etc.) planned

## Usage
```cpp
graph g;
g.add_edge(1, 2, 7);
g.add_edge(2, 3, 5);
g.add_edge(1, 3);          // no weight given, defaults to 1

auto distances = g.dijkstra(1);
for (auto& [node, dist] : distances) {
    std::cout << "1 -> " << node << " : " << dist << "\n";
}

std::vector<int> order = g.dfs(1);   // visit order, not a path
std::vector<int> levels = g.bfs(1);  // visit order, nearest nodes first

g.export_dot(); // writes graph.dot, viewable with Graphviz
```

## Notes
- `dfs` and `bfs` return the **order in which nodes are visited**, not a route between two nodes. Consecutive entries aren't necessarily connected by an edge.
- Calling `dfs` or `bfs` with a start node that isn't in the graph prints a message and returns an empty vector.
- In `dijkstra`, nodes that can't be reached from the source keep the value `std::numeric_limits<int>::max()`.

## Roadmap
- [x] Weighted graph + adjacency list
- [x] Dijkstra's algorithm
- [x] `.dot` export
- [x] DFS traversal
- [x] BFS traversal
- [ ] Path extraction (`dfs_path` / `bfs_path`, start to goal)
- [ ] Minimum spanning tree (Prim's / Kruskal's)
- [ ] A* search
- [ ] Bellman-Ford (negative weights)# graphkit-cpp

A C++ graph library built from scratch, implementing common graph algorithms on a weighted, undirected graph structure.

> **Status:** work in progress. Graph basics, Dijkstra, DFS, BFS and `.dot` export are working. Path extraction, A*, MST and Bellman-Ford are planned.

## Features
- Weighted adjacency-list graph representation (nodes are integer IDs)
- Undirected edges; unweighted edges default to weight 1
- Dijkstra's shortest path algorithm
- Depth-first and breadth-first traversal
- Export to Graphviz `.dot` format for visualization
- More algorithms (path extraction, MST, A*, etc.) planned

## Usage
```cpp
graph g;
g.add_edge(1, 2, 7);
g.add_edge(2, 3, 5);
g.add_edge(1, 3);          // no weight given, defaults to 1

auto distances = g.dijkstra(1);
for (auto& [node, dist] : distances) {
    std::cout << "1 -> " << node << " : " << dist << "\n";
}

std::vector<int> order = g.dfs(1);   // visit order, not a path
std::vector<int> levels = g.bfs(1);  // visit order, nearest nodes first

g.export_dot(); // writes graph.dot, viewable with Graphviz
```

## Notes
- `dfs` and `bfs` return the **order in which nodes are visited**, not a route between two nodes. Consecutive entries aren't necessarily connected by an edge.
- Calling `dfs` or `bfs` with a start node that isn't in the graph prints a message and returns an empty vector.
- In `dijkstra`, nodes that can't be reached from the source keep the value `std::numeric_limits<int>::max()`.

## Roadmap
- [x] Weighted graph + adjacency list
- [x] Dijkstra's algorithm
- [x] `.dot` export
- [x] DFS traversal
- [x] BFS traversal
- [ ] Path extraction (`dfs_path` / `bfs_path`, start to goal)
- [ ] Minimum spanning tree (Prim's / Kruskal's)
- [ ] A* search
- [ ] Bellman-Ford (negative weights)
