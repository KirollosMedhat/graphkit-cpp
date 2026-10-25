// Benchmark: Dijkstra on a large random connected graph.
// Not part of the demo; build and run it separately.

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "graph.h"

using graphkit::graph;

int main() {
    const int N = 50000;             // number of nodes
    const int EXTRA_EDGES = 150000;  // extra edges beyond the spanning tree

    graph g;
    std::mt19937 rng(42);            // fixed seed, so every run builds the same graph
    std::uniform_int_distribution<int> weightDist(1, 20);

    // Step 1: random spanning tree, which guarantees the graph is connected.
    std::vector<int> connected;
    connected.push_back(1);
    std::vector<int> remaining;
    for (int i = 2; i <= N; i++) remaining.push_back(i);
    std::shuffle(remaining.begin(), remaining.end(), rng);

    for (int v : remaining) {
        std::uniform_int_distribution<int> pick(0, static_cast<int>(connected.size()) - 1);
        int u = connected[pick(rng)];
        g.add_edge(u, v, weightDist(rng));
        connected.push_back(v);
    }

    // Step 2: extra random edges for density. Duplicates are possible.
    std::uniform_int_distribution<int> nodeDist(1, N);
    for (int i = 0; i < EXTRA_EDGES; i++) {
        int u = nodeDist(rng);
        int v = nodeDist(rng);
        if (u != v) {
            g.add_edge(u, v, weightDist(rng));
        }
    }

    std::cout << "Graph built: " << N << " nodes, ~" << (N - 1 + EXTRA_EDGES)
              << " edges (approx, duplicates possible)\n";

    auto start = std::chrono::high_resolution_clock::now();
    auto distances = g.dijkstra(1);
    auto end = std::chrono::high_resolution_clock::now();

    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Dijkstra from node 1 finished (" << distances.size() << " nodes reached).\n";
    std::cout << "Time taken: " << us << " microseconds (" << ms << " ms)\n";

    return 0;
}