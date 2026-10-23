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
    using NodeID = int;
    static_assert(numeric_limits<Weight>::has_infinity, "Weight needs an infinity (use a floating-point type)");
    static constexpr Weight unreachable = numeric_limits<Weight>::infinity();

private:
    struct Edge {
        NodeID targetNode;
        Weight weight;
    };
    
    struct QueueEntry {
        Weight priority; 
        NodeID node; 

        auto operator<=>(const QueueEntry&) const = default;            //std::strong_ordering for int, std::partial_ordering for double.
    };

    unordered_map<NodeID, list<Edge>> adjlist;

public:
    struct PathResult {
        vector<NodeID> path;     // start ... destination; empty if unreachable
        Weight distance = unreachable;         // total weight of the path
    };

    /// @brief Adds an undirected edge between u and v.
    /// Creates either node if it doesn't exist yet.
    /// @param u One endpoint of the edge.
    /// @param v The other endpoint.
    /// @param w Edge weight; defaults to 1 so unweighted graphs get hop counts.
    void add_edge(NodeID u, NodeID v, Weight w = 1);
    bool add_node(NodeID id);

    //Utils
    void print() const;
    void export_dot(const string& filename) const;

    //Algorithms
    unordered_map<NodeID, Weight> dijkstra(NodeID source) const;
    vector<NodeID> dfs(NodeID start) const;
    vector<NodeID> bfs(NodeID start) const;
    vector<NodeID> path_dfs(NodeID start, NodeID destination) const;
    vector<NodeID> path_bfs(NodeID start, NodeID destination) const;
    PathResult path_dijkstra(NodeID start, NodeID destination) const;
    PathResult path_astar(NodeID start, NodeID destination, std::function<Weight(NodeID)> heuristic = [](NodeID){ return 0; }) const;
};
