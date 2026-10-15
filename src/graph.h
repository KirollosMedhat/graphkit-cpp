#pragma once
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <limits> 

using namespace std;

class graph{
private:
    unordered_map<int, list<pair<int,int>>> adjlist;

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

    //Utils
    void print();
    void export_dot();

    //Algorithms
    unordered_map<int, int> dijkstra(int source);
    vector<int> dfs(int start);
    vector<int> bfs(int start);
    vector<int> path_dfs(int start, int destination);
    vector<int> path_bfs(int start, int destination);
    PathResult path_dijkstra(int start, int destination);
};
