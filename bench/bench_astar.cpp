// Compares A* (Manhattan heuristic) with Dijkstra on grid graphs:
// the routes on a small map, and the speed on a large one.

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "graph.h"

// Node ID = y * width + x. '#' is a wall; every other cell connects to its right and lower neighbor.
static graph make_grid(const std::vector<std::string>& rows) {
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

// Manhattan distance to `goal`: a valid estimate on a 4-direction grid where each step costs 1.
static std::function<int(int)> manhattan_to(int goal, int width) {
    return [goal, width](int id) {
        return std::abs(id % width - goal % width) + std::abs(id / width - goal / width);
    };
}

// True if the cell is free and has at least one free neighbor (so make_grid put it in the graph).
static bool in_graph(const std::vector<std::string>& rows, int id) {
    int w = rows[0].size(), h = rows.size(), x = id % w, y = id / w;
    if (rows[y][x] == '#') return false;
    return (x > 0 && rows[y][x - 1] != '#') || (x + 1 < w && rows[y][x + 1] != '#') ||
           (y > 0 && rows[y - 1][x] != '#') || (y + 1 < h && rows[y + 1][x] != '#');
}

static void draw(std::vector<std::string> rows, const std::vector<int>& route, int s, int d) {
    int w = rows[0].size();
    for (int id : route) rows[id / w][id % w] = '*';
    rows[s / w][s % w] = 'S';
    rows[d / w][d % w] = 'G';
    for (const auto& row : rows) std::cout << "    " << row << "\n";
}

int main() {
    // ---------- 1. Routes on a small map ----------
    std::vector<std::string> small = {
        ".....#....",
        ".###.#.##.",
        ".#...#..#.",
        ".#.###..#.",
        ".#......#.",
        ".########.",
        "..........",
    };
    graph g_small = make_grid(small);
    int s = 0, d = 49;                                        // top-left, and the cell at row 4, column 9
    auto dij  = g_small.path_dijkstra(s, d);
    auto astar = g_small.path_astar(s, d, manhattan_to(d, small[0].size()));

    std::cout << "Small map, route " << s << " -> " << d << "\n";
    std::cout << "  Dijkstra:       distance " << dij.distance << "\n";
    draw(small, dij.path, s, d);
    std::cout << "  A* (Manhattan): distance " << astar.distance << "\n";
    draw(small, astar.path, s, d);

    // ---------- 2. Speed on a large grid ----------
    const int W = 200, H = 200, QUERIES = 100;
    std::mt19937 rng(42);                                     // fixed seed: same grid and queries every run
    std::bernoulli_distribution wall(0.25);
    std::vector<std::string> big(H, std::string(W, '.'));
    for (auto& row : big) for (auto& c : row) if (wall(rng)) c = '#';
    graph g = make_grid(big);

    std::uniform_int_distribution<int> pick(0, W * H - 1);
    using Clock = std::chrono::steady_clock;
    double dijkstra_ms = 0, astar_ms = 0;
    int compared = 0, different = 0;

    while (compared < QUERIES) {
        int a = pick(rng), b = pick(rng);
        if (!in_graph(big, a) || !in_graph(big, b)) continue;

        auto t0 = Clock::now();
        auto r1 = g.path_dijkstra(a, b);
        auto t1 = Clock::now();
        auto r2 = g.path_astar(a, b, manhattan_to(b, W));
        auto t2 = Clock::now();

        if (r1.path.empty()) continue;                        // unreachable pair: skip it
        compared++;
        if (r1.distance != r2.distance) different++;
        dijkstra_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
        astar_ms    += std::chrono::duration<double, std::milli>(t2 - t1).count();
    }

    std::cout << "\nLarge grid " << W << "x" << H << ", 25% walls, " << compared << " routes\n";
    std::cout << "  routes with different distances: " << different << "\n";
    std::cout << "  Dijkstra total: " << dijkstra_ms << " ms\n";
    std::cout << "  A* total:       " << astar_ms << " ms\n";
    std::cout << "  A* is " << dijkstra_ms / astar_ms << "x faster\n";
    return 0;
}