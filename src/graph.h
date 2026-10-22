#pragma once
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <limits> 
#include <functional>
#include <fstream>

using namespace std;

class graph{
private:
    struct Edge {
        int targetNode;
        int weight;
    };
    
    struct QueueEntry {
        int priority; 
        int node; 

        auto operator<=>(const QueueEntry&) const = default;            //std::strong_ordering
    };

    unordered_map<int, list<Edge>> adjlist;

public:
    struct PathResult {
        vector<int> path;     // start ... destination; empty if unreachable
        int distance = numeric_limits<int>::max();         // total weight of the path
    };

    /// @brief Adds an undirected edge between u and v.
    /// Creates either node if it doesn't exist yet.
    /// @param u One endpoint of the edge.
    /// @param v The other endpoint.
    /// @param w Edge weight; defaults to 1 so unweighted graphs get hop counts.
    void add_edge(int u,int v,int w = 1);
    bool add_node(int id);

    //Utils
    void print() const;
    void export_dot(const string& filename) const;

    //Algorithms
    unordered_map<int, int> dijkstra(int source) const;
    vector<int> dfs(int start) const;
    vector<int> bfs(int start) const;
    vector<int> path_dfs(int start, int destination) const;
    vector<int> path_bfs(int start, int destination) const;
    PathResult path_dijkstra(int start, int destination) const;
    PathResult path_astar(int start, int destination, std::function<int(int)> heuristic = [](int){ return 0; }) const;
};
