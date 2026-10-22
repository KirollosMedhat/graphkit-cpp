#pragma once
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <limits> 
#include <functional>

using namespace std;

class graph{
public:
    using Weight = double;
    static_assert(numeric_limits<Weight>::has_infinity, "Weight needs an infinity (use a floating-point type)");
    static constexpr Weight unreachable = numeric_limits<Weight>::infinity();

private:
    struct Edge {
        int targetNode;
        Weight weight;
    };
    
    struct QueueEntry {
        Weight priority; 
        int node; 

        auto operator<=>(const QueueEntry&) const = default;            //std::strong_ordering
    };

    unordered_map<int, list<Edge>> adjlist;

public:
    struct PathResult {
        vector<int> path;     // start ... destination; empty if unreachable
        Weight distance = unreachable;         // total weight of the path
    };

    /// @brief Adds an undirected edge between u and v.
    /// Creates either node if it doesn't exist yet.
    /// @param u One endpoint of the edge.
    /// @param v The other endpoint.
    /// @param w Edge weight; defaults to 1 so unweighted graphs get hop counts.
    void add_edge(int u,int v, Weight w = 1);
    bool add_node(int id);

    //Utils
    void print() const;
    void export_dot(const string& filename) const;

    //Algorithms
    unordered_map<int, Weight> dijkstra(int source) const;
    vector<int> dfs(int start) const;
    vector<int> bfs(int start) const;
    vector<int> path_dfs(int start, int destination) const;
    vector<int> path_bfs(int start, int destination) const;
    PathResult path_dijkstra(int start, int destination) const;
    PathResult path_astar(int start, int destination, std::function<Weight(int)> heuristic = [](int){ return 0; }) const;
};
