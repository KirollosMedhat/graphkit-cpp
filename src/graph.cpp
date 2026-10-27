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

void graph::add_directed_edge(graph::NodeID u, graph::NodeID v, graph::Weight w){
    // validate first, so a rejected call changes nothing
    if(w < 0 || !std::isfinite(w)){
        throw std::invalid_argument("Weight must be finite and non-negative.");
    }
    add_node(u);
    add_node(v);
    adjlist.at(u).push_back({v,w, false});
}

void graph::add_edge(graph::NodeID u, graph::NodeID v, graph::Weight w){
    // validate first, so a rejected call changes nothing
    if(w < 0 || !std::isfinite(w)){
        throw std::invalid_argument("Weight must be finite and non-negative.");
    }
    add_node(u);
    add_node(v);
    adjlist.at(u).push_back({v, w, true});      // two-way: stored once from each end
    adjlist.at(v).push_back({u, w, true});
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

// Needs non-negative weights: a node's distance is final once it is popped.
std::unordered_map<graph::NodeID, graph::Weight> graph::dijkstra(graph::NodeID source) const{
    if(adjlist.count(source) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(source));
    }
    std::unordered_map<graph::NodeID, graph::Weight> distances;                  // best known distance per node

    // min-queue: the closest node comes out first
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pq;

    // every node starts at infinity, the source at 0
    for (const auto& i: adjlist){

        if(i.first != source){
            distances[i.first] = graph::unreachable;
        }else{
            distances[i.first] = 0;
            pq.push({0,source});            
        }
        
    }

    while(!pq.empty()){
        auto current = pq.top();                               // copy: pop() would invalidate a reference
        pq.pop();

        // outdated entry: a shorter route was found after it was queued
        if (current.priority > distances[current.node]) continue;        

        for(const auto& edge : adjlist.at(current.node)){
            
            graph::Weight newDistance = distances[current.node] + edge.weight;  
            
            // shorter route through current: record it and queue the neighbor
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

    outFile << "digraph G {\n";
    for (const auto& i : adjlist) {
        outFile << i.first << ";\n";

        for(const auto& k : i.second){
            // an undirected edge is stored twice (once from each end): write it only from the smaller ID
            if (k.undirected && i.first >= k.targetNode) continue;

            outFile << i.first << " -> " << k.targetNode << " [label=\"" << k.weight << "\"";
            if (k.undirected) outFile << ", dir=none";
            outFile << "];\n";
        }
    }
    outFile << "}\n";
}

// Time: O(V + E), where V = nodes and E = edges.
std::vector<graph::NodeID> graph::dfs(graph::NodeID start) const{
    if(adjlist.count(start) == 0){
        throw std::invalid_argument("No Node in graph with ID: " + std::to_string(start));
    }
    std::vector<graph::NodeID> order;

    // visited = processed, not queued: a node can be on the stack more than once
    std::unordered_set<graph::NodeID> visited;
    std::stack<graph::NodeID> st;
    st.push(start);

    while(!st.empty()){
        graph::NodeID currentNode = st.top();
        st.pop();

        // checked on pop, not on push: a deeper node may have queued it again meanwhile
        if(visited.count(currentNode) == 1){
            continue;
        }

        visited.insert(currentNode);
        order.push_back(currentNode);

        for(const auto& edge : adjlist.at(currentNode)){        
            // skip processed neighbors (only an optimization)
            if(visited.count(edge.targetNode) == 1){
                continue;
            }
            // the last neighbor pushed is visited first: DFS goes deep before backtracking
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
            visited.insert(edge.targetNode);                         // marked on discovery, so each node is queued once (DFS marks on pop)
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
    st.push({start, start});                         // start is its own parent: the walk back stops at start, so this is never read

    while(!st.empty()){
        auto current = st.top();
        st.pop();

        if(visited.count(current.node) == 1) continue;
        parentOf[current.node] = current.parent;        // the first copy popped decides the parent

        if(current.node == destination){
            // walk back from destination to start, then reverse
            graph::NodeID next = destination;
            while (next != start){
                result.push_back(next);
                next = parentOf[next];
            }
            result.push_back(start);
            std::reverse(result.begin(), result.end());
            break;
        }

        visited.insert(current.node);

        for(const auto& edge : adjlist.at(current.node)){
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
    q.push({start, start});                         // start is its own parent: the walk back stops at start, so this is never read

    while(!q.empty()){
        auto current = q.front();
        q.pop();

        if(visited.count(current.node) == 1) continue;
        parentOf[current.node] = current.parent;        // the first copy popped decides the parent

        if(current.node == destination){
            // walk back from destination to start, then reverse
            graph::NodeID next = destination;
            while (next != start){
                result.push_back(next);
                next = parentOf[next];
            }
            result.push_back(start);
            std::reverse(result.begin(), result.end());
            break;
        }

        visited.insert(current.node);

        for(const auto& edge : adjlist.at(current.node)){
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

    PathResult result;

    std::unordered_map<graph::NodeID, graph::Weight> distances;                 // filled lazily: no entry = not reached yet
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pq;
    std::unordered_map<graph::NodeID, graph::NodeID> parentOf;

    distances[start] = 0;
    parentOf[start] = start;
    pq.push({0, start});


    while(!pq.empty()){
        auto current = pq.top();
        pq.pop();
        result.numberOfExploredNodes++;                 // counts every pop, outdated entries included

        // outdated entry: a shorter route was found after it was queued
        if (current.priority > distances[current.node]) continue;

        if(current.node == destination){
            graph::NodeID next = destination;
            while (next != start){
                result.path.push_back(next);
                next = parentOf[next];
            }
            result.path.push_back(start);
            std::reverse(result.path.begin(), result.path.end());
            result.distance = distances[destination];
            break;

        }

        for(const auto& edge : adjlist.at(current.node)){
            graph::Weight newDistance = distances[current.node] + edge.weight;  
            // not reached yet: [] would insert 0, so insert "unreachable" explicitly
            if(distances.count(edge.targetNode) == 0){
                distances[edge.targetNode] = graph::unreachable;
            }
            if(newDistance < distances[edge.targetNode]){
                distances[edge.targetNode] = newDistance;
                pq.push({distances[edge.targetNode], edge.targetNode});
                parentOf[edge.targetNode] = current.node;
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
    PathResult result;

    std::unordered_map<graph::NodeID, graph::Weight> distances;              // g(n), filled lazily: no entry = not reached yet

    // ordered by f(n) = g(n) + h(n)
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pq;

    std::unordered_map<graph::NodeID, graph::NodeID> parentOf;

    distances[start] = 0;
    parentOf[start] = start;
    pq.push({heuristic(start),start});

    while(!pq.empty()){
        auto current = pq.top();
        pq.pop();
        result.numberOfExploredNodes++;                 // counts every pop, outdated entries included

        // outdated entry (f is recomputed, so the heuristic must return the same value every time)
        graph::Weight currentF = distances[current.node] + heuristic(current.node);
        if (current.priority > currentF) continue;

        if(current.node == destination){
            graph::NodeID next = destination;
            while (next != start){
                result.path.push_back(next);
                next = parentOf[next];
            }
            result.path.push_back(start);
            std::reverse(result.path.begin(), result.path.end());
            result.distance = distances[destination];
            break;
        }

        for(const auto& edge : adjlist.at(current.node)){
            graph::Weight newDistance = distances[current.node] + edge.weight;
            // not reached yet: [] would insert 0, so insert "unreachable" explicitly
            if(distances.count(edge.targetNode) == 0){
                distances[edge.targetNode] = graph::unreachable;
            }

            if(newDistance < distances[edge.targetNode]){
                graph::Weight neighborF = newDistance + heuristic(edge.targetNode);

                distances[edge.targetNode] = newDistance;
                pq.push({neighborF, edge.targetNode});
                parentOf[edge.targetNode] = current.node;
            }
        }
    }
    return result;
}