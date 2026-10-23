#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "graph.h"
#include "sample_graph.h"

using namespace std;

// To render the exported graph as an image:
//   dot -Tpng -Grankdir=LR graph.dot -o graph.png

// Prints nodes in the order given, e.g. a visit order.
void print_nodes(const string& label, const vector<int>& nodes) {
    cout << label;
    for (int n : nodes) cout << n << " ";
    cout << endl;
}

// Prints a route as "a -> b -> END (n edges)"; the distance is added when it is known.
void print_route(const string& label, const vector<int>& route, int distance = -1) {
    cout << label;
    for (int n : route) cout << n << " -> ";
    cout << "END";
    if (!route.empty()) {
        cout << "  (" << route.size() - 1 << " edges";
        if (distance >= 0) cout << ", distance " << distance;
        cout << ")";
    }
    cout << endl;
}

// Builds a graph from an ASCII map: '#' is a wall, anything else is free.
// Node ID = y * width + x; each cell connects to its right and lower neighbor.
graph make_grid(const vector<string>& rows) {
    graph g;
    int height = rows.size(), width = rows[0].size();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (rows[y][x] == '#') continue;
            if (x + 1 < width && rows[y][x + 1] != '#') g.add_edge(y * width + x, y * width + x + 1);
            if (y + 1 < height && rows[y + 1][x] != '#') g.add_edge(y * width + x, (y + 1) * width + x);
        }
    }
    return g;
}

// Traversal: DFS goes deep first, BFS goes level by level.
void traversal_demo() {
    graph g;
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    g.add_edge(3, 2);

    cout << "=== Traversal from node 1 (visit order) ===" << endl;
    print_nodes("  DFS: ", g.dfs(1));
    print_nodes("  BFS: ", g.bfs(1));
}

// Routes from node 1 to node 19 on the 60-node graph: each algorithm optimizes something different.
void routes_demo() {
    graph g = make_sample_graph();

    cout << endl << "=== Routes 1 -> 19 on the 60-node graph ===" << endl;
    print_route("  DFS (any route):          ", g.path_dfs(1, 19));
    print_route("  BFS (fewest edges):       ", g.path_bfs(1, 19));
    auto dijkstra_route = g.path_dijkstra(1, 19);
    print_route("  Dijkstra (lowest weight): ", dijkstra_route.path, dijkstra_route.distance);
    auto astar_route = g.path_astar(1, 19);                 // no heuristic: behaves like Dijkstra
    print_route("  A* (no heuristic):        ", astar_route.path, astar_route.distance);

    auto distances = g.dijkstra(1);
    cout << "  Dijkstra distances from node 1: to 19 = " << distances[19]
         << ", to 36 = " << distances[36] << endl;
    g.export_dot("test");                                         // writes graph.dot
}

// A* on a grid: same distance as without a heuristic, but far fewer nodes examined.
void astar_demo() {
    vector<string> rows = {
        "....................",
        "....................",
        "....................",
        "....................",
        "..........#.........",
        "...S......#......G..",
        "..........#.........",
        "....................",
        "....................",
        "....................",
        "....................",
    };
    graph g = make_grid(rows);
    int width = rows[0].size();
    int s = 103, d = 117;                                   // 'S' and 'G' on the map (y * 20 + x)

    // The graph has no coordinates, so we supply the estimate: Manhattan distance to the goal.
    auto manhattan = [=](int id) { return abs(id % width - d % width) + abs(id / width - d / width); };

    long calls_zero = 0, calls_manhattan = 0;
    auto zero     = [&calls_zero](int)               { calls_zero++;      return 0; };
    auto estimate = [&calls_manhattan, manhattan](int id) { calls_manhattan++; return manhattan(id); };

    auto dijkstra_route = g.path_dijkstra(s, d);
    auto blind  = g.path_astar(s, d, zero);
    auto guided = g.path_astar(s, d, estimate);

    cout << endl << "=== A* on a grid: S (node " << s << ") to G (node " << d << ") ===" << endl;
    cout << "  Dijkstra:    distance " << dijkstra_route.distance << endl;
    cout << "  no estimate: distance " << blind.distance  << ", heuristic calls " << calls_zero << endl;
    cout << "  Manhattan:   distance " << guided.distance << ", heuristic calls " << calls_manhattan << endl;

    for (int id : guided.path)                              // draw the route on the map
        if (rows[id / width][id % width] == '.') rows[id / width][id % width] = '*';
    for (const auto& row : rows) cout << "    " << row << endl;
}

int main() {
    traversal_demo();
    routes_demo();
    astar_demo();
    return 0;
}