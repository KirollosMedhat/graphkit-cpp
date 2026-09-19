#include <iostream>
#include "graph.h"
#include <chrono> // Required header


using namespace std;

// for generating the image: dot -Tpng -Grankdir=LR graph.dot -o graph.png

int main() {
    graph g;

    g.add_edge(1, 46, 10);
    g.add_edge(1, 25, 12);
    g.add_edge(1, 55, 1);
    g.add_edge(46, 13, 13);
    g.add_edge(25, 18, 2);
    g.add_edge(46, 22, 14);
    g.add_edge(1, 56, 7);
    g.add_edge(13, 59, 8);
    g.add_edge(18, 35, 3);
    g.add_edge(18, 21, 6);
    g.add_edge(55, 11, 11);
    g.add_edge(13, 48, 12);
    g.add_edge(21, 33, 11);
    g.add_edge(46, 38, 10);
    g.add_edge(21, 32, 3);
    g.add_edge(56, 44, 3);
    g.add_edge(38, 5, 7);
    g.add_edge(59, 23, 15);
    g.add_edge(5, 51, 4);
    g.add_edge(21, 6, 14);
    g.add_edge(46, 31, 4);
    g.add_edge(46, 12, 13);
    g.add_edge(21, 41, 7);
    g.add_edge(59, 24, 2);
    g.add_edge(22, 60, 15);
    g.add_edge(23, 26, 15);
    g.add_edge(12, 2, 6);
    g.add_edge(22, 50, 11);
    g.add_edge(32, 27, 7);
    g.add_edge(50, 53, 15);
    g.add_edge(6, 20, 8);
    g.add_edge(35, 30, 5);
    g.add_edge(59, 40, 4);
    g.add_edge(44, 28, 12);
    g.add_edge(2, 14, 15);
    g.add_edge(60, 37, 6);
    g.add_edge(38, 43, 3);
    g.add_edge(30, 34, 8);
    g.add_edge(18, 54, 13);
    g.add_edge(55, 15, 14);
    g.add_edge(56, 47, 3);
    g.add_edge(15, 58, 3);
    g.add_edge(2, 4, 10);
    g.add_edge(13, 29, 7);
    g.add_edge(24, 39, 10);
    g.add_edge(27, 7, 9);
    g.add_edge(44, 36, 9);
    g.add_edge(1, 52, 11);
    g.add_edge(7, 45, 2);
    g.add_edge(4, 8, 15);
    g.add_edge(28, 57, 13);
    g.add_edge(5, 10, 13);
    g.add_edge(47, 16, 6);
    g.add_edge(56, 17, 5);
    g.add_edge(2, 19, 3);
    g.add_edge(27, 49, 1);
    g.add_edge(7, 3, 15);
    g.add_edge(7, 9, 5);
    g.add_edge(30, 42, 13);
    g.add_edge(12, 33, 15);
    g.add_edge(7, 56, 11);
    g.add_edge(20, 54, 11);
    g.add_edge(33, 39, 4);
    g.add_edge(10, 24, 13);
    g.add_edge(11, 35, 13);
    g.add_edge(60, 34, 15);
    g.add_edge(1, 39, 6);
    g.add_edge(32, 2, 2);
    g.add_edge(60, 24, 15);
    g.add_edge(54, 52, 5);
    g.add_edge(16, 4, 4);
    g.add_edge(57, 37, 2);
    g.add_edge(6, 47, 8);
    g.add_edge(53, 5, 13);
    g.add_edge(35, 50, 3);
    g.add_edge(9, 43, 8);
    g.add_edge(36, 11, 5);
    g.add_edge(34, 56, 10);
    g.add_edge(28, 14, 15);
    g.add_edge(35, 49, 12);
    g.add_edge(45, 13, 12);
    g.add_edge(20, 26, 11);
    g.add_edge(42, 24, 8);
    g.add_edge(58, 34, 8);
    g.add_edge(8, 16, 4);
    g.add_edge(5, 22, 1);
    g.add_edge(38, 36, 4);
    g.add_edge(38, 15, 1);
    g.add_edge(5, 46, 11);
    g.add_edge(4, 15, 2);

    auto start = std::chrono::high_resolution_clock::now();
    auto distances = g.dijkstra(1);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    

    cout << "Shortest distances from node 1:\n";

    for (auto [node, distance] : distances) {
        cout << "1 -> " << node << " = " << distance << endl;
    }

    std::cout << "Time taken by code: " << duration.count() << " microseconds" << std::endl;

    // cout << "==========================" <<endl;
    // g.print();
    g.export_dot();
    return 0;
}





// #include "graph.h"
// #include <chrono>
// #include <random>
// #include <algorithm>
// #include <vector>

// int main(){
//     const int N = 50000;            // number of nodes
//     const int EXTRA_EDGES = 150000; // extra edges beyond spanning tree

//     graph g;
//     std::mt19937 rng(42);
//     std::uniform_int_distribution<int> weightDist(1, 20);

//     // Step 1: random spanning tree to guarantee connectivity
//     std::vector<int> connected;
//     connected.push_back(1);
//     std::vector<int> remaining;
//     for (int i = 2; i <= N; i++) remaining.push_back(i);
//     std::shuffle(remaining.begin(), remaining.end(), rng);

//     for (int v : remaining) {
//         std::uniform_int_distribution<int> pick(0, (int)connected.size() - 1);
//         int u = connected[pick(rng)];
//         g.add_edge(u, v, weightDist(rng));
//         connected.push_back(v);
//     }

//     // Step 2: extra random edges for density
//     std::uniform_int_distribution<int> nodeDist(1, N);
//     for (int i = 0; i < EXTRA_EDGES; i++) {
//         int u = nodeDist(rng);
//         int v = nodeDist(rng);
//         if (u != v) {
//             g.add_edge(u, v, weightDist(rng));
//         }
//     }

//     cout << "Graph built: " << N << " nodes, ~" << (N - 1 + EXTRA_EDGES) << " edges (approx, dupes possible)\n";

//     auto start = std::chrono::high_resolution_clock::now();
//     auto distances = g.dijkstra(1);
//     auto end = std::chrono::high_resolution_clock::now();

//     auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//     auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

//     cout << "Dijkstra from node 1 finished.\n";
//     cout << "Time taken: " << us << " microseconds (" << ms << " ms)\n";

//     g.export_dot();

//     return 0;
// }