#pragma once
#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

class graph{
private:
    unordered_map<int, list<pair<int,int>>> adjlist;

public:
    void add_edge(int,int,int);
    void print();
    unordered_map<int, int> dijkstra(int source);
    void export_dot();

};
