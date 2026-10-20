#include "graph.h"
#include <limits> 
#include <queue>
#include <stack>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <algorithm>

void graph::add_edge(int u,int v,int w){
    adjlist[u].push_back({v,w});
    adjlist[v].push_back({u,w});
}

void graph::print(){
    for (auto i:adjlist){
        cout << i.first << " -> ";

        for(const auto& k:i.second){
            cout << "(" << k.first << ", " << k.second << ") ";
        }
        cout << endl;

    }
}


// Dijkstra's algorithm: shortest distance from `source` to every node.
// Requires non-negative edge weights. A node's distance is final the moment it
// is popped from the queue, because any other route to it goes through nodes
// that are at least as far away, and adding non-negative weights can't make a
// route shorter. A negative weight breaks that guarantee.
// Unreachable nodes keep the value numeric_limits<int>::max().
unordered_map<int, int> graph::dijkstra(int source){
    unordered_map<int, int> distances;                  // Output: best known distance from source to each node.

    // Min-priority queue of {weight, node ID} pairs. the pair is stored as <weight, node ID>, Pairs compare by their first element.
    // so that the pq can return the highest priority(closest unprocessed node) based on weight not the Node ID.
    // Having pq with <Node, weight> will cause the processing order be determined based on Node ID.
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;        //min-priority Queue.

    // Setting distances to all undiscovered nodes to infinity(or max int can hold) 
    // Except for the only known node at this point which is source.

    // No routes are known yet, so every node starts at "infinity" (the largest int).
    // The source is the exception: its distance is 0, since we are already there.
    for (const auto& i: adjlist){

        if(i.first != source){
            distances[i.first] = numeric_limits<int>::max();
        }else{
            // Weight of source is 0 as it is the current node, we are already there.
            distances[i.first] = 0;
            pq.push({0,source});            
        }
        
    }

    // Loop will run as long as there are nodes queued in the priority queue, while any node is still waiting to be processed.
    while(!pq.empty()){
        pair<int,int> current = pq.top();                               // Getting the highest priority node from the queue.
        pq.pop();                                                       // Removing it from the queue.

        // skipping duplicate entries, skipping the processing for the higher weights.
        // Stale entry: a shorter distance to this node was found after this entry was
        // queued, so the entry is outdated and the node was already processed with the
        // better distance. Skip it.
        if (current.first > distances[current.second]) continue;        

        // Processing all neighbors, calculating their distances from source.
        // current.second is the current node which we are exploring its neighbors.

        // Try to improve the distance of every neighbor of the current node.
        // current.second is the node being processed; for each neighbor,
        // neighbor.first is its ID and neighbor.second is the weight of the edge to it.
        for(const auto& neighbor : this->adjlist[current.second]){
            
            // Distances[current.second] is the known distance calculated to the current node,
            // Adding to it neighbor.second which the weight of the edge between current node and the neighbor we are currently processing.
            int newDistance = distances[current.second] + neighbor.second;  
            
            // Checking if the calculated distance is less than the previously recorded and assigning the smaller.

            // Only if this route beats the best known one: record it and queue the
            // neighbor with its improved distance. Any older entry for the neighbor
            // is now stale.
            if(newDistance < distances[neighbor.first]){
                distances[neighbor.first] = newDistance;
                // Pushing all discovered neighbors with their new weights into the priority queue.
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

// Depth-first traversal from `start`. Returns the nodes in the order they were
// visited. This is a visit order, not a route: consecutive entries aren't
// necessarily connected by an edge, because DFS backtracks after a dead end.
// Only nodes reachable from `start` appear. Returns an empty vector if `start`
// isn't in the graph. Edge weights are ignored. Time: O(V + E).
// V is the number of vertices, meaning the nodes. E is the number of edges.
vector<int> graph::dfs(int start){
    vector<int> order;                                  // Output: nodes in the order they were processed.

    // Guard: use find (not operator[]) so a missing node isn't inserted into the graph.
    if(adjlist.find(start) == adjlist.end()){
        cout<<"No such node, terminating."<<endl;
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

        auto currentList = adjlist.find(currentNode);   // Returns an Iterator to this node's entry in adjlist (its key and neighbor list).
        visited.insert(currentNode);                    // Marked as processed.
        order.push_back(currentNode);                   // Record it in the visit order(Output vector).

        // Each element is a (neighbor ID, edge weight) pair.
        for(const auto& neighbors : currentList -> second){
            // Skip neighbors that are already processed. This is only an optimization.
            // It can't catch a neighbor that is queued but not yet processed, which is
            // what the pop-time check above is for.
            if(visited.count(neighbors.first) == 1){
                continue;
            }
            // The last neighbor pushed is popped first, which is why DFS goes
            // deep before it backtracks to the earlier neighbors.
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

vector<int> graph::path_dfs(int start, int destination){
    vector<int> result;
    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        cout<<"Start or destination node are not in the graph, terminating."<<endl;
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

        auto currentList = adjlist.find(currentPair.first);   // Returns an iterator to the pair whose key is currentNode.
        visited.insert(currentPair.first);                    // Marked visited.

        for(const auto& neighbors : currentList -> second){
            if(visited.count(neighbors.first) == 1){
                continue;
            }
            st.push({neighbors.first, currentPair.first});
        }
    }
    return result;
}

vector<int> graph::path_bfs(int start, int destination){
    vector<int> result;
    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        cout<<"Start or destination node are not in the graph, terminating."<<endl;
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

        auto currentList = adjlist.find(currentPair.first);   // Returns an iterator to the pair whose key is currentNode.
        visited.insert(currentPair.first);                    // Marked visited.
        for(const auto& neighbors : currentList -> second){
            if(visited.count(neighbors.first) == 1){
                continue;
            }
            q.push({neighbors.first, currentPair.first});
        }
    }
    return result;
}

graph::PathResult graph::path_dijkstra(int start, int destination){
    PathResult result;                               // Output: best known path from start to destination and it's distance.

    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        cout << "Start or destination node are not in the graph, terminating." << endl;
        return result;
    }
    unordered_map<int, int> distances;                 
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;        //min-priority Queue.
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
        pair<int,int> current = pq.top();                               // Getting the highest priority node from the queue.
        pq.pop();                                                       // Removing it from the queue.

        if (current.first > distances[current.second]) continue;

        if(current.second == destination){
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

        for(const auto& neighbor : this->adjlist[current.second]){
            int newDistance = distances[current.second] + neighbor.second;  
            if(newDistance < distances[neighbor.first]){
                distances[neighbor.first] = newDistance;
                pq.push({distances[neighbor.first], neighbor.first});
                parent[neighbor.first] = current.second;
            }
        }
    }
    return result;
}


graph::PathResult graph::path_astar(int start, int destination, std::function<int(int)> heuristic){
    PathResult result;                               // Output: best known path from start to destination and it's distance.

    if(adjlist.count(start) == 0 || adjlist.count(destination) == 0){
        cout<<"Start or destination node are not in the graph, terminating."<<endl;
        return result;
    }

    unordered_map<int, int> distances;              // This is the g(n).

    // Min-priority queue of {f(n), node ID} pairs. Where where f(n) = g(n) + h(n), 
    // g(n) is the cost from start to current node and h(n) is the estimate from current node to the destination.
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;        //min-priority Queue.

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
        pair<int,int> current = pq.top();
        pq.pop();

        int f = distances[current.second] + heuristic(current.second);
        if (current.first > f) continue;

        if(current.second == destination){
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

        for(const auto& neighbor : this->adjlist[current.second]){
            int newDistance = distances[current.second] + neighbor.second;
            if(newDistance < distances[neighbor.first]){
                int f = newDistance + heuristic(neighbor.first);

                distances[neighbor.first] = newDistance;
                pq.push({f, neighbor.first});
                parent[neighbor.first] = current.second;
            }
        }
    }
    return result;
}