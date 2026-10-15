#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <limits>

#include "graph.h"
#include "sample_graph.h"

using namespace std;

// To render the exported graph as an image:
//   dot -Tpng -Grankdir=LR graph.dot -o graph.png


void print_result(const string& label, const graph::PathResult& r){
    cout << label;
    for (int n : r.path) cout << n << " -> ";
    cout << "END";
    if (!r.path.empty())
        cout << "  (distance " << r.distance << ", " << r.path.size() - 1 << " edges)";
    cout << endl;
}

/// Prints a route as "a -> b -> c -> END (n edges)". An empty route prints just "END".
void print_route(const string& label, const vector<int>& route) {
    cout << label;
    for (int n : route) cout << n << " -> ";
    cout << "END";
    if (!route.empty()) cout << "  (" << route.size() - 1 << " edges)";
    cout << endl;
}

// Dijkstra's lowest-weight route next to BFS's fewest-edges route.
void compare_weighted(graph& g, int s, int d){
    cout << endl << "Route " << s << " -> " << d << endl;
    print_result("  Dijkstra: ", g.path_dijkstra(s, d));
    print_route ("  BFS:      ", g.path_bfs(s, d));
}

// For every node: path_dijkstra's distance must equal what dijkstra() reports.
void cross_check(graph& g, int source){
    auto distances = g.dijkstra(source);
    int bad = 0;
    for (const auto& [node, dist] : distances){
        graph::PathResult r = g.path_dijkstra(source, node);
        if (dist == numeric_limits<int>::max()) {            // unreachable
            if (!r.path.empty()) bad++;
        } else if (r.path.empty() || r.path.front() != source ||
                   r.path.back() != node || r.distance != dist) {
            bad++;
        }
    }
    cout << endl << "Cross-check from node " << source << ": "
         << distances.size() << " nodes, " << bad << " mismatches" << endl;
}


/// Prints the DFS route and the BFS route from s to d, one after the other.
void compare_paths(graph& g, int s, int d) {
    cout << endl << "Route " << s << " -> " << d << endl;
    print_route("  DFS: ", g.path_dfs(s, d));
    print_route("  BFS: ", g.path_bfs(s, d));
}

/// Prints Dijkstra distances from `source` in node order.
void show_distances(graph& g, int source) {
    auto distances = g.dijkstra(source);
    map<int, int> sorted(distances.begin(), distances.end());

    cout << endl << "Shortest distances from node " << source << ":" << endl;
    for (const auto& [node, distance] : sorted) {
        cout << "  " << source << " -> " << node << " = " << distance << endl;
    }
}

int main() {
    // Small graph: the two algorithms pick different routes from 1 to 2.
    graph small;
    small.add_edge(1, 2);
    small.add_edge(1, 3);
    small.add_edge(3, 4);
    small.add_edge(3, 2);
    compare_paths(small, 1, 2);
    compare_paths(small, 1, 4);
    compare_paths(small, 1, 1);

    // Two components: node 5 exists but can't be reached from node 1.
    graph two;
    two.add_edge(1, 2);
    two.add_edge(2, 3);
    two.add_edge(4, 5);
    compare_paths(two, 1, 3);
    compare_paths(two, 1, 5);
    compare_paths(two, 1, 9);   // 9 is not in the graph

    // The 60-node sample graph.
    graph g = make_sample_graph();
    compare_paths(g, 1, 36);
    compare_paths(g, 1, 19);
    compare_paths(g, 1, 57);
    compare_paths(g, 1, 26);

    show_distances(g, 1);
    g.export_dot();   // writes graph.dot


    // weighted graph where the direct edge is NOT the cheapest route
    graph w;
    w.add_edge(1, 2, 10);
    w.add_edge(1, 3, 1);
    w.add_edge(3, 2, 2);
    compare_weighted(w, 1, 2);
    compare_weighted(w, 1, 3);
    compare_weighted(w, 1, 1);

    // two components (reusing the `two` graph from earlier in main)
    compare_weighted(two, 1, 3);
    compare_weighted(two, 1, 5);   // exists, but unreachable
    compare_weighted(two, 1, 9);   // not in the graph

    // the 60-node graph (reusing `g`)
    compare_weighted(g, 1, 36);
    compare_weighted(g, 1, 19);
    compare_weighted(g, 1, 57);
    compare_weighted(g, 1, 26);
    cross_check(g, 1);


    graph t;
    t.add_edge(1, 2);
    t.path_dijkstra(9, 1);   // 9 doesn't exist
    t.print();               // does node 9 now appear?

    return 0;
}
