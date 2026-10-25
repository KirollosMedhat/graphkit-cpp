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
#include <cmath>
#include <string>

using graphkit::graph;

bool graph::add_node(graph::NodeID id){
    return adjlist.try_emplace(id).second;
}

void graph::add_edge(graph::NodeID u, graph::NodeID v, graph::Weight w){
    if(w < 0 || !std::isfinite(w)){
        throw std::invalid_argument("Weight must be finite and non-negative.");
    }
    add_node(u);
    add_node(v);
    adjlist.at(u).push_back({v,w});
    adjlist.at(v).push_back({u,w});
}

void graph::print() const{
    for (const auto& i:adjlist){
        std::cout << i.first << " -> ";

        for(const auto& k:i.second){
            std::cout << "(" << k.targetNode << ", " << k.weight << ") ";
        }
        std::cout << std::endl;
    }
}

// Dijkstra's algorithm: shortest distance from `source` to every node.
// Requires non-negative weights: that is what makes a node's distance final once it is popped.
// Unreachable nodes keep graph::unreachable.
std::unordered_map<graph::NodeID, graph::Weight> graph::dijkstra(graph::NodeID source) const{
    if(adjlist.count(source) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(source));
    }
    std::unordered_map<graph::NodeID, graph::Weight> distances;                  // Best known distance from source to each node.

    // Min-queue of {priority, node}. Here the priority is the distance from source,
    // so the closest unprocessed node comes out first.
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pq;        //min-priority Queue.

    // Every node starts at "infinity"; the source starts at 0.
    for (const auto& i: adjlist){

        if(i.first != source){
            distances[i.first] = graph::unreachable;
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
            
            graph::Weight newDistance = distances[current.node] + edge.weight;  
            
            // Better route: record it and queue the neighbor (any older entry is now stale).
            if(newDistance < distances[edge.targetNode]){
                distances[edge.targetNode] = newDistance;
                pq.push({distances[edge.targetNode], edge.targetNode});
            }
        }
    }
    return distances;
}

void graph::export_dot(const std::string& filename) const{
    std::ofstream outFile(filename + ".dot", std::ios::out | std::ios::trunc);

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
std::vector<graph::NodeID> graph::dfs(graph::NodeID start) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }
    std::vector<graph::NodeID> order;                                  // Output: nodes in the order they were processed.

    // visited means "already processed", not "already queued". A node can sit on the
    // stack several times; only the first copy that is popped gets processed.
    std::unordered_set<graph::NodeID> visited;
    std::stack<graph::NodeID> st;                                      // Nodes waiting to be processed; the most recently pushed comes out first (LIFO).
    st.push(start);

    while(!st.empty()){
        graph::NodeID currentNode = st.top();
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

std::vector<graph::NodeID> graph::bfs(graph::NodeID start) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }
    std::vector<graph::NodeID> order;
    std::unordered_set<graph::NodeID> visited;
    std::queue<graph::NodeID> q;
    q.push(start);
    visited.insert(start);

    while(!q.empty()){
        graph::NodeID currentNode = q.front();
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

std::vector<graph::NodeID> graph::path_dfs(graph::NodeID start, graph::NodeID destination) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }

    if(adjlist.count(destination) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(destination));
    }

    std::vector<graph::NodeID> result;

    std::unordered_set<graph::NodeID> visited;
    std::stack<graph::Visit> st;
    std::unordered_map<graph::NodeID,graph::NodeID> parentOf;
    st.push({start, start});                         // Start is its own parent.

    while(!st.empty()){
        auto current = st.top();
        st.pop();

        if(visited.count(current.node) == 1) continue;
        parentOf[current.node] = current.parent;

        if(current.node == destination){
            graph::NodeID next = destination;
            while (next != start){
                result.push_back(next);
                next = parentOf[next];
            }
            result.push_back(start);
            std::reverse(result.begin(), result.end());
            break;
        }

        visited.insert(current.node);                    // Marked visited.

        for(const auto& edge : adjlist.at(current.node)){  // Returns an iterator to the pair whose key is currentNode.
            if(visited.count(edge.targetNode) == 1){
                continue;
            }
            st.push({edge.targetNode, current.node});
        }
    }
    return result;
}

std::vector<graph::NodeID> graph::path_bfs(graph::NodeID start, graph::NodeID destination) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }

    if(adjlist.count(destination) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(destination));
    }

    std::vector<graph::NodeID> result;

    std::unordered_set<graph::NodeID> visited;
    std::queue<graph::Visit> q;
    std::unordered_map<graph::NodeID,graph::NodeID> parentOf;
    q.push({start, start});                         // Start is its own parent.

    while(!q.empty()){
        auto current = q.front();
        q.pop();

        if(visited.count(current.node) == 1) continue;
        parentOf[current.node] = current.parent;

        if(current.node == destination){
            graph::NodeID next = destination;
            while (next != start){
                result.push_back(next);
                next = parentOf[next];
            }
            result.push_back(start);
            std::reverse(result.begin(), result.end());
            break;
        }

        visited.insert(current.node);                    // Marked visited.

        for(const auto& edge : adjlist.at(current.node)){      // Returns an iterator to the pair whose key is currentNode.
            if(visited.count(edge.targetNode) == 1){
                continue;
            }
            q.push({edge.targetNode, current.node});
        }
    }
    return result;
}

graph::PathResult graph::path_dijkstra(graph::NodeID start, graph::NodeID destination) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }

    if(adjlist.count(destination) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(destination));
    }

    PathResult result;                               // Output: best known path from start to destination and its distance.

    std::unordered_map<graph::NodeID, graph::Weight> distances;                 
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pq;        //min-priority Queue.
    // <Node ID, Parent ID>
    std::unordered_map<graph::NodeID, graph::NodeID> parent;

    distances[start] = 0;
    parent[start] = start;
    pq.push({0, start});


    while(!pq.empty()){
        auto current = pq.top();                               // Getting the highest priority node from the queue.
        pq.pop();                                                       // Removing it from the queue.
        result.numberOfExploredNodes++;

        if (current.priority > distances[current.node]) continue;

        if(current.node == destination){
            graph::NodeID next = destination;
            while (next != start){
                result.path.push_back(next);
                next = parent[next];
            }
            result.path.push_back(start);
            std::reverse(result.path.begin(), result.path.end());
            result.distance = distances[destination];
            break;

        }

        for(const auto& edge : adjlist.at(current.node)){
            graph::Weight newDistance = distances[current.node] + edge.weight;  
            if(distances.count(edge.targetNode) == 0){
                distances[edge.targetNode] = graph::unreachable;
            }
            if(newDistance < distances[edge.targetNode]){
                distances[edge.targetNode] = newDistance;
                pq.push({distances[edge.targetNode], edge.targetNode});
                parent[edge.targetNode] = current.node;
            }
        }
    }
    return result;
}


graph::PathResult graph::path_astar(graph::NodeID start, graph::NodeID destination, std::function<graph::Weight(NodeID)> heuristic) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }

    if(adjlist.count(destination) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(destination));
    }
    PathResult result;                               // Output: best known path from start to destination and it's distance.

    std::unordered_map<graph::NodeID, graph::Weight> distances;              // This is the g(n).

    // Min-priority queue of {f(n), node ID} pairs. Where f(n) = g(n) + h(n), 
    // g(n) is the cost from start to current node and h(n) is the estimate from current node to the destination.
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pq;

    std::unordered_map<graph::NodeID, graph::NodeID> parent;

    distances[start] = 0;
    parent[start] = start;
    pq.push({heuristic(start),start});

    while(!pq.empty()){
        auto current = pq.top();
        pq.pop();
        result.numberOfExploredNodes++;

        graph::Weight currentF = distances[current.node] + heuristic(current.node);
        if (current.priority > currentF) continue;

        if(current.node == destination){
            graph::NodeID next = destination;
            while (next != start){
                result.path.push_back(next);
                next = parent[next];
            }
            result.path.push_back(start);
            std::reverse(result.path.begin(), result.path.end());
            result.distance = distances[destination];
            break;
        }

        for(const auto& edge : adjlist.at(current.node)){
            graph::Weight newDistance = distances[current.node] + edge.weight;
            if(distances.count(edge.targetNode) == 0){
                distances[edge.targetNode] = graph::unreachable;
            }

            if(newDistance < distances[edge.targetNode]){
                graph::Weight neighborF = newDistance + heuristic(edge.targetNode);

                distances[edge.targetNode] = newDistance;
                pq.push({neighborF, edge.targetNode});
                parent[edge.targetNode] = current.node;
            }
        }
    }
    return result;
}