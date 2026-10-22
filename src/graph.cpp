#include "graph.h"
#include <limits> 
#include <queue>
#include <stack>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <compare>
#include <optional>
#include <stdexcept>

bool graph::add_node(int id){
    return adjlist.try_emplace(id).second;
}

void graph::add_edge(int u,int v,int w){
    add_node(u);
    add_node(v);
    adjlist.at(u).push_back({v,w});
    adjlist.at(v).push_back({u,w});
}

void graph::print() const{
    for (const auto& i:adjlist){
        cout << i.first << " -> ";

        for(const auto& k:i.second){
            cout << "(" << k.targetNode << ", " << k.weight << ") ";
        }
        cout << endl;
    }
}

// Dijkstra's algorithm: shortest distance from `source` to every node.
// Requires non-negative weights: that is what makes a node's distance final once it is popped.
// Unreachable nodes keep numeric_limits<int>::max().
unordered_map<int, int> graph::dijkstra(int source) const{
    unordered_map<int, int> distances;                  // Best known distance from source to each node.

    // Min-queue of {priority, node}. Here the priority is the distance from source,
    // so the closest unprocessed node comes out first.
    priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> pq;        //min-priority Queue.

    // Every node starts at "infinity"; the source starts at 0.
    for (const auto& i: adjlist){

        if(i.first != source){
            distances[i.first] = numeric_limits<int>::max();
        }else{
            distances[i.first] = 0;
            pq.push({0,source});            
        }
        
    }

    // Loop will run as long as there are nodes queued in the priority queue, while any node is still waiting to be processed.
    while(!pq.empty()){
        auto current = pq.top();                               // A copy: pop() would invalidate a reference.
        pq.pop();

        // Stale entry: a shorter route to this node was found after it was queued. Skip it.
        if (current.priority > distances[current.node]) continue;        

        // Try to shorten the distance to each neighbor by going through the current node.
        for(const auto& edge : adjlist.at(current.node)){
            
            int newDistance = distances[current.node] + edge.weight;  
            
            // Better route: record it and queue the neighbor (any older entry is now stale).
            if(newDistance < distances[edge.targetNode]){
                distances[edge.targetNode] = newDistance;
                pq.push({distances[edge.targetNode], edge.targetNode});
            }
        }
    }
    return distances;
}

void graph::export_dot(const string& filename) const{
    ofstream outFile(filename + ".dot", std::ios::out | std::ios::trunc);

    outFile << "graph G {\n";
    for (const auto& i : adjlist) {
        outFile << i.first << ";\n";                     // declares the node, even if it has no edges

        for(const auto& k : i.second){
            if (i.first < k.targetNode) {
                outFile << i.first << " -- " << k.targetNode << " [label=\"" << k.weight << "\"];\n";
            }
        }
    }
    outFile << "}\n";
}

// Depth-first traversal from `start`. Returns the nodes in the order they were
// visited. This is a visit order, not a route: consecutive entries aren't
// necessarily connected by an edge, because DFS backtracks after a dead end.
// Only nodes reachable from `start` appear. Returns an empty vector if `start`
// isn't in the graph. Edge weights are ignored. Time: O(V + E).
// V is the number of vertices, meaning the nodes. E is the number of edges.
vector<int> graph::dfs(int start) const{
    vector<int> order;                                  // Output: nodes in the order they were processed.

    // Guard: use find (not operator[]) so a missing node isn't inserted into the graph.
    if(adjlist.find(start) == adjlist.end()){
        //cout<<"No such node, terminating."<<endl;
        return order;
    }

    // visited means "already processed", not "already queued". A node can sit on the
    // stack several times; only the first copy that is popped gets processed.
    unordered_set<int> visited;
    stack<int> st;                                      // Nodes waiting to be processed; the most recently pushed comes out first (LIFO).
    st.push(start);

    while(!st.empty()){
        int currentNode = st.top();
        st.pop();                                       // Always remove the entry first, so every iteration makes progress.

        // A copy of this node was already processed earlier, so this entry is a
        // leftover. This is why the check happens on pop, not on push: a node can be
        // pushed again by a deeper node before it has been processed.
        if(visited.count(currentNode) == 1){
            continue;
        }

        visited.insert(currentNode);                    // Marked as processed.
        order.push_back(currentNode);                   // Record it in the visit order(Output vector).

        // Each element is a (neighbor ID, edge weight) pair.
        for(const auto& edge : adjlist.at(currentNode)){        
            // Skip neighbors that are already processed. This is only an optimization.
            // It can't catch a neighbor that is queued but not yet processed, which is
            // what the pop-time check above is for.
            if(visited.count(edge.targetNode) == 1){
                continue;
            }
            // The last neighbor pushed is popped first, which is why DFS goes
            // deep before it backtracks to the earlier neighbors.
            st.push(edge.targetNode);
        }
    }
    return order;
}

vector<int> graph::bfs(int start) const{
    vector<int> order;
    if(adjlist.count(start) == 0){
        //cout<<"No such node, terminating."<<endl;
        return order;
    }
    unordered_set<int> visited;
    queue<int> q;
    q.push(start);
    visited.insert(start);

    while(!q.empty()){
        int currentNode = q.front();
        q.pop();

        order.push_back(currentNode);                           

        for(const auto& edge : adjlist.at(currentNode)){
            if(visited.count(edge.targetNode) == 1) continue;
            q.push(edge.targetNode);
            visited.insert(edge.targetNode);                         //Mark visited on discovery, so a node is queued only once. Differs from the DFS.
        }
    }
    return order;
}

vector<int> graph::path_dfs(int start, int destination) const{
    vector<int> result;
    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        return result;
    }

    unordered_set<int> visited;
    stack<pair<int,int>> st;
    unordered_map<int,int> parent;
    st.push({start, -1});                         //-1 for the starting node with no parents.

    while(!st.empty()){
        auto currentPair = st.top();
        st.pop();

        if(visited.count(currentPair.first) == 1) continue;
        parent[currentPair.first] = currentPair.second;

        if(currentPair.first == destination){
            int next = destination;
            while (next != start){
                result.push_back(next);
                next = parent[next];
            }
            result.push_back(start);
            reverse(result.begin(), result.end());
            break;
        }

        visited.insert(currentPair.first);                    // Marked visited.

        for(const auto& edge : adjlist.at(currentPair.first)){  // Returns an iterator to the pair whose key is currentNode.
            if(visited.count(edge.targetNode) == 1){
                continue;
            }
            st.push({edge.targetNode, currentPair.first});
        }
    }
    return result;
}

vector<int> graph::path_bfs(int start, int destination) const{
    vector<int> result;
    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        return result;
    }

    unordered_set<int> visited;
    queue<pair<int,int>> q;
    unordered_map<int,int> parent;
    q.push({start, -1});                         //-1 for the starting node with no parents.

    while(!q.empty()){
        auto currentPair = q.front();
        q.pop();

        if(visited.count(currentPair.first) == 1) continue;
        parent[currentPair.first] = currentPair.second;

        if(currentPair.first == destination){
            int next = destination;
            while (next != start){
                result.push_back(next);
                next = parent[next];
            }
            result.push_back(start);
            reverse(result.begin(), result.end());
            break;
        }

        visited.insert(currentPair.first);                    // Marked visited.

        for(const auto& edge : adjlist.at(currentPair.first)){      // Returns an iterator to the pair whose key is currentNode.
            if(visited.count(edge.targetNode) == 1){
                continue;
            }
            q.push({edge.targetNode, currentPair.first});
        }
    }
    return result;
}

graph::PathResult graph::path_dijkstra(int start, int destination) const{
    PathResult result;                               // Output: best known path from start to destination and its distance.

    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        return result;
    }
    unordered_map<int, int> distances;                 
    priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> pq;        //min-priority Queue.
    // <Node ID, Parent ID>
    unordered_map<int,int> parent;

    

    for (const auto& i: adjlist){
        if(i.first != start){
            distances[i.first] = numeric_limits<int>::max();
        }else{
            // Weight of start is 0 as it is the current node, we are already there.
            distances[i.first] = 0;
            pq.push({0,start});
            parent[start] = -1;                 // -1 is for a node with no parent.
        }
    }

    while(!pq.empty()){
        auto current = pq.top();                               // Getting the highest priority node from the queue.
        pq.pop();                                                       // Removing it from the queue.

        if (current.priority > distances[current.node]) continue;

        if(current.node == destination){
            int next = destination;
            while (next != start){
                result.path.push_back(next);
                next = parent[next];
            }
            result.path.push_back(start);
            reverse(result.path.begin(), result.path.end());
            result.distance = distances[destination];
            break;

        }

        for(const auto& edge : adjlist.at(current.node)){
            int newDistance = distances[current.node] + edge.weight;  
            if(newDistance < distances[edge.targetNode]){
                distances[edge.targetNode] = newDistance;
                pq.push({distances[edge.targetNode], edge.targetNode});
                parent[edge.targetNode] = current.node;
            }
        }
    }
    return result;
}


graph::PathResult graph::path_astar(int start, int destination, std::function<int(int)> heuristic) const{
    PathResult result;                               // Output: best known path from start to destination and it's distance.

    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        //cout<<"Start or destination node are not in the graph, terminating."<<endl;
        return result;
    }

    unordered_map<int, int> distances;              // This is the g(n).

    // Min-priority queue of {f(n), node ID} pairs. Where f(n) = g(n) + h(n), 
    // g(n) is the cost from start to current node and h(n) is the estimate from current node to the destination.
    priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> pq;

    unordered_map<int,int> parent;

    for (const auto& i: adjlist){
        if(i.first != start){
            distances[i.first] = numeric_limits<int>::max();
        }else{
            distances[i.first] = 0;
            pq.push({heuristic(start),start});
            parent[start] = -1;                 // -1 is for a node with no parent.
        }
    }

    while(!pq.empty()){
        auto current = pq.top();
        pq.pop();

        int currentF = distances[current.node] + heuristic(current.node);
        if (current.priority > currentF) continue;

        if(current.node == destination){
            int next = destination;
            while (next != start){
                result.path.push_back(next);
                next = parent[next];
            }
            result.path.push_back(start);
            reverse(result.path.begin(), result.path.end());
            result.distance = distances[destination];
            break;
        }

        for(const auto& edge : adjlist.at(current.node)){
            int newDistance = distances[current.node] + edge.weight;
            if(newDistance < distances[edge.targetNode]){
                int neighborF = newDistance + heuristic(edge.targetNode);

                distances[edge.targetNode] = newDistance;
                pq.push({neighborF, edge.targetNode});
                parent[edge.targetNode] = current.node;
            }
        }
    }
    return result;
}