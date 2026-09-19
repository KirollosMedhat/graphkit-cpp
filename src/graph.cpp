#include "graph.h"
#include <limits> 
#include <queue>
#include <fstream>

void graph::add_edge(int u,int v,int w){
    adjlist[u].push_back({v,w});
    adjlist[v].push_back({u,w});
}

void graph::print(){
    for (auto i:adjlist){
        cout << i.first << " -> ";

        for(auto k:i.second){
            cout << "(" << k.first << ", " << k.second << ") ";
        }
        cout << endl;

    }
}


unordered_map<int, int> graph::dijkstra(int source){
    unordered_map<int, int> distances;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    for (auto i: adjlist){

        
        if(i.first != source){
            distances[i.first] = numeric_limits<int>::max();
        }else{
            distances[i.first] = 0;
            pq.push({0,source});
        }
    }

    while(!pq.empty()){
        pair<int,int> current = pq.top();
        pq.pop();
        //int node = current.second; //current.second is the node.
        //list<pair<int,int>> neighbors = this->adjlist[node];

        for(auto neighbor : this->adjlist[current.second]){
            //pq.push({neighbor.second, neighbor.first});
            int newDistance = distances[current.second] + neighbor.second;
            if(newDistance < distances[neighbor.first]){
                distances[neighbor.first] = newDistance;
                pq.push({distances[neighbor.first], neighbor.first});
            }
        }
    }
    return distances;
}

void graph::export_dot() {
    ofstream outFile("graph.dot");

    outFile << "graph G {\n";
    for (auto i : adjlist) {
        for(auto k : i.second){
            if (i.first < k.first) {
                outFile << i.first << " -- " << k.first << " [label=\"" << k.second << "\"];\n";
            }
        }
    }
    outFile << "}\n";
    
}