#include "graph.h"
#include <limits> 
#include <queue>
#include <stack>
#include <fstream>
#include <filesystem>
#include <unordered_set>

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
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;        //min-priority Queue.

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

        if (current.first > distances[current.second]) continue; // skip stale entry


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
    ofstream outFile("graph.dot", std::ios::out | std::ios::trunc);
    //cout << "export_dot() called, writing to: " << filesystem::current_path() << "/graph.dot" << endl;

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

vector<int> graph::dfs(int start){
    vector<int> order;
    if(adjlist.find(start) == adjlist.end()){
        cout<<"No such node, terminating."<<endl;
        return order;
    }
    unordered_set<int> visited;
    stack<int> st;
    st.push(start);

    while(!st.empty()){
        int currentNode = st.top();
        st.pop();

        if(visited.count(currentNode) == 1){
            continue;
        }

        auto currentList = adjlist.find(currentNode);   // Returns an iterator to the pair whose key is currentNode.
        visited.insert(currentNode);                    // Marked visited.
        order.push_back(currentNode);                   // Pushing current node to the output vector.

        for(const auto& neighbors : currentList -> second){
            if(visited.count(neighbors.first) == 1){
                continue;
            }
            st.push(neighbors.first);
        }
    }
    return order;
}

vector<int> graph::bfs(int start){
    vector<int> order;
    if(adjlist.count(start) == 0){
        cout<<"No such node, terminating."<<endl;
        return order;
    }
    unordered_set<int> visited;
    queue<int> q;
    q.push(start);
    visited.insert(start);

    while(!q.empty()){
        int currentNode = q.front();
        q.pop();

        //if(visited.count(currentNode) == 1) continue;
        //visited.insert(currentNode);

        order.push_back(currentNode);                           
        auto currentList = adjlist.find(currentNode);

        for(const auto& neighbor : currentList -> second){
            if(visited.count(neighbor.first) == 1) continue;
            q.push(neighbor.first);
            visited.insert(neighbor.first);                         //Mark visited on discovery, so a node is queued only once. Differs from the DFS.
        }
    }
    return order;
}