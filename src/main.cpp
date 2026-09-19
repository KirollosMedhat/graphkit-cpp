#include <iostream>
#include "graph.h"

using namespace std;

int main() {
    graph g;

    g.add_edge(1, 2, 7);
    g.add_edge(1, 3, 9);
    g.add_edge(1, 6, 14);

    g.add_edge(2, 3, 10);
    g.add_edge(2, 4, 15);

    g.add_edge(3, 4, 11);
    g.add_edge(3, 6, 2);
    g.add_edge(3, 5, 6);

    g.add_edge(4, 5, 6);
    g.add_edge(4, 7, 9);

    g.add_edge(5, 6, 9);
    g.add_edge(5, 8, 5);

    g.add_edge(6, 8, 12);
    g.add_edge(6, 9, 7);

    g.add_edge(7, 8, 4);
    g.add_edge(7, 10, 8);

    g.add_edge(8, 9, 3);
    g.add_edge(8, 11, 6);

    g.add_edge(9, 10, 5);
    g.add_edge(9, 12, 10);

    g.add_edge(10, 11, 2);
    g.add_edge(11, 12, 4);

    auto distances = g.dijkstra(1);

    cout << "Shortest distances from node 1:\n";

    for (auto [node, distance] : distances) {
        cout << "1 -> " << node << " = " << distance << endl;
    }

    cout << "==========================" <<endl;
    g.print();
    g.export_dot();
    return 0;
}