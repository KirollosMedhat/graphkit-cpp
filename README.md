# graphkit-cpp

A C++ graph library built from scratch, implementing common graph algorithms on a weighted, undirected graph structure.

## Features
- Weighted adjacency-list graph representation
- Dijkstra's shortest path algorithm
- Export to Graphviz `.dot` format for visualization
- More algorithms (BFS, DFS, MST, etc.) planned

## Usage
\`\`\`cpp
graph g;
g.add_edge(1, 2, 7);
g.add_edge(2, 3, 5);

auto distances = g.dijkstra(1);
for (auto& [node, dist] : distances) {
    std::cout << "1 -> " << node << " : " << dist << "\n";
}

g.export_dot(); // writes graph.dot, viewable with Graphviz
\`\`\`

## Roadmap
- [x] Weighted graph + adjacency list
- [x] Dijkstra's algorithm
- [x] `.dot` export
- [ ] BFS / DFS
- [ ] Minimum spanning tree (Prim's / Kruskal's)
- [ ] A* search
- [ ] Bellman-Ford (negative weights)