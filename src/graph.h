#pragma once
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <limits> 
#include <functional>
#include <string>

namespace graphkit{

class graph{
public:
    using Weight = double;
    using NodeID = int;
    static_assert(std::numeric_limits<Weight>::has_infinity, "Weight needs an infinity (use a floating-point type)");
    static constexpr Weight unreachable = std::numeric_limits<Weight>::infinity();      // "no route" marker

private:
    struct Edge {
        NodeID targetNode;
        Weight weight;
        bool undirected = false;        // true if added with add_edge (stored once from each end)
    };
    
    struct QueueEntry {
        Weight priority; 
        NodeID node; 

        auto operator<=>(const QueueEntry&) const = default;            // compares priority first, then node
    };

    struct Visit {                      // a node waiting in a stack or queue, and the node that added it
        NodeID node;
        NodeID parent;
    };

    std::unordered_map<NodeID, std::list<Edge>> adjlist;        // node -> its outgoing edges

public:
    struct PathResult {
        std::vector<NodeID> path;     // start ... destination; empty if there is no route
        Weight distance = unreachable;         // total weight of the path
        int numberOfExploredNodes = 0;         // queue pops, outdated entries included
    };

    /// Adds a two-way edge (w defaults to 1). Creates missing nodes.
    /// Throws std::invalid_argument if w is negative or not finite.
    void add_edge(NodeID u, NodeID v, Weight w = 1);

    /// Adds a one-way edge from u to v. Same rules as add_edge.
    void add_directed_edge(NodeID u, NodeID v, Weight w = 1);

    /// Adds a node with no edges. Returns false if it already exists.
    bool add_node(NodeID id);

    //Utils
    void print() const;
    
    /// Writes <filename>.dot, a Graphviz digraph. Two-way edges are drawn once, without arrowheads.
    /// Two-way self-loops are not drawn.
    void export_dot(const std::string& filename) const;





    //Algorithms
    // All of these throw std::invalid_argument if a given node isn't in the graph.

    /// Lowest distance from source to every node. Unreachable nodes hold graph::unreachable.
    std::unordered_map<NodeID, Weight> dijkstra(NodeID source) const;

    /// Visit order, not a route. Only nodes reachable from start. Ignores weights.
    std::vector<NodeID> dfs(NodeID start) const;
    std::vector<NodeID> bfs(NodeID start) const;

    /// A route, not necessarily short. Empty if there is none.
    std::vector<NodeID> path_dfs(NodeID start, NodeID destination) const;

    /// A route with the fewest edges. Empty if there is none.
    std::vector<NodeID> path_bfs(NodeID start, NodeID destination) const;

    /// Lowest-weight route. If there is none: empty path, distance == graph::unreachable.
    PathResult path_dijkstra(NodeID start, NodeID destination) const;

    /// Same result as path_dijkstra, guided by the heuristic (default 0: same as Dijkstra).
    /// The heuristic estimates the remaining cost to the destination. It must never overestimate,
    /// must return the same value for the same node, and must be 0 at the destination.
    PathResult path_astar(NodeID start, NodeID destination, std::function<Weight(NodeID)> heuristic = [](NodeID){ return 0; }) const;
};
}       // namespace graphkit