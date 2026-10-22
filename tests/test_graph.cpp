#undef NDEBUG

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <vector>
#include <limits>
#include <fstream>

#include "graph.h"


void test_dijkstra_finds_lowest_cost_route() {
    graph g;
    g.add_edge(1, 2, 3);
    g.add_edge(1, 3, 1);
    g.add_edge(2, 4, 2);
    g.add_edge(3, 4, 5);
    g.add_edge(3, 2, 1);

    auto result = g.path_dijkstra(1,4);

    std::vector<int> correct_path = {1, 3, 2, 4};

    assert(result.distance == 4);
    assert(result.path == correct_path);
}

void test_dijkstra_unreachable_destination() {
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(3, 4, 2);

    auto result1 = g.path_dijkstra(1, 3);
    auto result2 = g.path_dijkstra(1, 4);
    auto result3 = g.path_dijkstra(2, 3);
    auto result4 = g.path_dijkstra(2, 4);

    assert(result1.distance == std::numeric_limits<int>::max());
    assert(result1.path.empty());

    assert(result2.distance == std::numeric_limits<int>::max());
    assert(result2.path.empty());

    assert(result3.distance == std::numeric_limits<int>::max());
    assert(result3.path.empty());

    assert(result4.distance == std::numeric_limits<int>::max());
    assert(result4.path.empty());
}

void test_dijkstra_start_equals_destination(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(3, 4, 2);

    auto result = g.path_dijkstra(1, 1);
    std::vector<int> correct_path = {1};
    assert(result.distance == 0);
    assert(result.path == correct_path);
}

void test_astar_zero_heuristic_matches_dijkstra() {
    graph g;
    g.add_edge(1, 2, 3);
    g.add_edge(1, 3, 1);
    g.add_edge(2, 4, 2);
    g.add_edge(3, 4, 5);
    g.add_edge(3, 2, 1);

    auto dijkstra_result = g.path_dijkstra(1, 4);
    auto astar_result = g.path_astar(1,4);

    assert(astar_result.distance == dijkstra_result.distance);
    assert(!astar_result.path.empty());
}

// manhattan heuristic for A* test.
int manhattan_3x3_to_9(int id){

    int row = (id - 1) / 3;
    int col = (id - 1) % 3;

    int goal_row = 2;
    int goal_col = 2;

    return abs(row - goal_row) + abs(col - goal_col);
    
}

void test_astar_manhattan_heuristic_finds_optimal_path(){
    graph g;
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);

    g.add_edge(4, 5, 1);
    g.add_edge(5, 6, 1);

    g.add_edge(7, 8, 1);
    g.add_edge(8, 9, 1);

    g.add_edge(1, 4, 1);
    g.add_edge(4, 7, 1);

    g.add_edge(2, 5, 1);
    g.add_edge(5, 8, 1);

    g.add_edge(3, 6, 1);
    g.add_edge(6, 9, 1);

    auto result = g.path_astar(1, 9, manhattan_3x3_to_9);

    assert(result.distance == 4);
    assert(!result.path.empty());

    assert(result.path.front() == 1);
    assert(result.path.back() == 9);
    assert(result.path.size() == 5);

}

void test_astar_unreachable_destination(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(3, 4, 2);

    auto result1 = g.path_astar(1, 3);
    auto result2 = g.path_astar(1, 4);
    auto result3 = g.path_astar(2, 3);
    auto result4 = g.path_astar(2, 4);

    assert(result1.distance == std::numeric_limits<int>::max());
    assert(result1.path.empty());

    assert(result2.distance == std::numeric_limits<int>::max());
    assert(result2.path.empty());

    assert(result3.distance == std::numeric_limits<int>::max());
    assert(result3.path.empty());

    assert(result4.distance == std::numeric_limits<int>::max());
    assert(result4.path.empty());

}

//DFS/BFS: cycles and reachable nodes
void test_DFS_BFS_cycle_check(){
    graph g;
    g.add_edge(1, 2, 3);
    g.add_edge(2, 3, 3);
    g.add_edge(3, 1, 3);

    g.add_edge(4, 5, 3);

    auto dfs_result = g.dfs(1);
    auto bfs_result = g.bfs(1);

    assert( std::find(dfs_result.begin(), dfs_result.end(), 1) != dfs_result.end());
    assert( std::find(dfs_result.begin(), dfs_result.end(), 2) != dfs_result.end());
    assert( std::find(dfs_result.begin(), dfs_result.end(), 3) != dfs_result.end());

    assert( std::find(bfs_result.begin(), bfs_result.end(), 1) != bfs_result.end());
    assert( std::find(bfs_result.begin(), bfs_result.end(), 2) != bfs_result.end());
    assert( std::find(bfs_result.begin(), bfs_result.end(), 3) != bfs_result.end());

    // A set stores each value only once, so equal sizes mean there were no duplicates.
    assert( std::set<int>(dfs_result.begin(), dfs_result.end()).size() == dfs_result.size());
    assert( std::set<int>(bfs_result.begin(), bfs_result.end()).size() == bfs_result.size());
    
    for(int i : dfs_result){
        assert(i != 4);
        assert(i != 5);
    }

    for(int i : bfs_result){
        

        assert(i != 4);
        assert(i != 5);
    }
}

void test_bfs_returns_path_with_fewest_edges(){
    graph g;
    g.add_edge(1, 4, 10);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);
    
    auto result = g.path_bfs(1, 4);
    std::vector<int> expected = {1, 4};

    assert(result.size() == 2);
    assert(result == expected);
}

void test_dfs_bfs_missing_nodes(){
    graph g;
    g.add_edge(1, 4, 10);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);

    auto bfs_result = g.bfs(5);
    auto dfs_result = g.dfs(5);

    assert(bfs_result.empty());
    assert(dfs_result.empty());
}

void test_dijkstra_single_edge_graph(){
    graph g;
    g.add_edge(1, 2, 5);

    auto result = g.path_dijkstra(1, 2);

    std::vector<int> expected = {1, 2};

    assert(result.distance == 5); 
    assert(result.path == expected);
}

void test_graph_duplicate_edges_and_self_loops(){
    graph g;

    g.add_edge(1, 2, 5);
    g.add_edge(1, 2, 5);  // Duplicate edge
    g.add_edge(1, 1, 3);  // Self-loop

    auto result = g.path_dijkstra(1, 2);

    std::vector<int> expected = {1, 2};

    // Duplicate edges and a self-loop shouldn't break shortest-path search.
    assert(result.distance == 5);
    assert(result.path == expected);
}

void test_dijkstra_zero_weight_edges(){
    graph g;
    g.add_edge(1, 2, 0);
    g.add_edge(1, 3, 5);
    g.add_edge(2, 3, 2);
    g.add_edge(3, 4, 1);
    g.add_edge(2, 4, 10);

    auto result = g.path_dijkstra(1, 4);
    std::vector<int> expected = {1, 2, 3, 4};

    assert(result.path == expected);
    assert(result.distance == 3);
}

void test_path_dfs_returns_a_non_shortest_route(){  // edges 1-2, 1-3, 3-4, 3-2: path_dfs(1, 2) == {1, 3, 2}
    graph g;
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    g.add_edge(3, 2);

    // DFS reaches 2 through 3, so the route is not the direct edge 1-2.
    std::vector<int> expected = {1, 3, 2};
    assert(g.path_dfs(1, 2) == expected);
}  

void test_dfs_visit_order(){                        // edges 1-2, 1-3, 2-4, 3-4, 4-5: dfs(1) == {1, 3, 4, 5, 2}
    graph g;
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 4);
    g.add_edge(3, 4);
    g.add_edge(4, 5);

    // The last neighbor pushed is visited first, so DFS goes 1 -> 3 -> 4 -> 5, then backtracks to 2.
    std::vector<int> expected = {1, 3, 4, 5, 2};
    assert(g.dfs(1) == expected);
}

void test_bfs_visit_order(){                        // same graph: bfs(1) == {1, 2, 3, 4, 5}
    graph g;
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 4);
    g.add_edge(3, 4);
    g.add_edge(4, 5);

    // BFS visits level by level: 1, then 2 and 3, then 4, then 5.
    std::vector<int> expected = {1, 2, 3, 4, 5};
    assert(g.bfs(1) == expected);
}

void test_dijkstra_distances(){                     // edges 1-2 (10), 1-3 (1), 3-2 (2): {1: 0, 3: 1, 2: 3}
    graph g;
    g.add_edge(1, 2, 10);
    g.add_edge(1, 3, 1);
    g.add_edge(3, 2, 2);

    auto distances = g.dijkstra(1);

    assert(distances.size() == 3);
    assert(distances.at(1) == 0);
    assert(distances.at(3) == 1);
    assert(distances.at(2) == 3);       // through 3 (1 + 2), not the direct edge (10)
}

void test_dijkstra_distances_unreachable_node(){    // a node in another component holds INT_MAX
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(3, 4, 2);

    auto distances = g.dijkstra(1);

    assert(distances.size() == 4);
    assert(distances.at(1) == 0);
    assert(distances.at(2) == 5);
    assert(distances.at(3) == std::numeric_limits<int>::max());
    assert(distances.at(4) == std::numeric_limits<int>::max());
}

void test_dijkstra_missing_source_returns_all_unreachable(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(2, 3, 2);

    auto distances = g.dijkstra(99);

    // Current behavior: nothing is reachable from a node that doesn't exist.
    // This will change when missing nodes start to throw.
    assert(distances.size() == 3);
    for (const auto& [node, dist] : distances)
        assert(dist == std::numeric_limits<int>::max());
}

void test_path_functions_missing_nodes_return_empty(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(2, 3, 2);

    // missing start, then missing destination, for every route function
    assert(g.path_dfs(99, 1).empty());
    assert(g.path_dfs(1, 99).empty());
    assert(g.path_bfs(99, 1).empty());
    assert(g.path_bfs(1, 99).empty());
    assert(g.path_dijkstra(99, 1).path.empty());
    assert(g.path_dijkstra(1, 99).path.empty());
    assert(g.path_astar(99, 1).path.empty());
    assert(g.path_astar(1, 99).path.empty());

    // none of those calls may have added a node to the graph
    assert(g.dijkstra(1).size() == 3);
}

void test_add_node_return_value(){
    graph g;
    bool first_add  = g.add_node(1);        // Returns true
    bool second_add = g.add_node(1);        // Returns false
    assert(first_add);
    assert(!second_add);
}

void test_isolated_nodes_in_searches(){
    graph g;
    g.add_node(1);
    g.add_node(2);

    std::vector<int> only_one = {1};
    assert(g.dfs(1) == only_one);
    assert(g.bfs(1) == only_one);

    // start == destination is a route of one node, even when the node has no edges
    auto to_itself = g.path_dijkstra(1, 1);
    assert(to_itself.path == only_one);
    assert(to_itself.distance == 0);

    // both nodes exist, but there is no route between them
    assert(g.path_dijkstra(1, 2).path.empty());
    assert(g.path_astar(1, 2).path.empty());
    assert(g.path_bfs(1, 2).empty());
    assert(g.path_dfs(1, 2).empty());
}

void test_dijkstra_distances_with_isolated_node(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_node(3);                                  // exists, but has no edges

    auto distances = g.dijkstra(1);

    assert(distances.size() == 3);
    assert(distances.at(1) == 0);
    assert(distances.at(2) == 5);
    assert(distances.at(3) == std::numeric_limits<int>::max());
}

void test_add_node_keeps_existing_edges(){
    graph g;
    g.add_edge(1, 2, 4);

    bool added = g.add_node(1);                     // node 1 already exists
    assert(!added);

    auto result = g.path_dijkstra(1, 2);
    std::vector<int> expected = {1, 2};
    assert(result.distance == 4);
    assert(result.path == expected);

}

int main(){

    test_dijkstra_finds_lowest_cost_route();
    test_dijkstra_unreachable_destination();
    test_dijkstra_start_equals_destination();

    test_astar_zero_heuristic_matches_dijkstra();
    test_astar_manhattan_heuristic_finds_optimal_path();
    test_astar_unreachable_destination();

    test_DFS_BFS_cycle_check();
    test_bfs_returns_path_with_fewest_edges();
    test_dfs_bfs_missing_nodes();

    test_dijkstra_single_edge_graph();  
    test_graph_duplicate_edges_and_self_loops();
    test_dijkstra_zero_weight_edges();

    test_path_dfs_returns_a_non_shortest_route();
    test_dfs_visit_order();
    test_bfs_visit_order();
    test_dijkstra_distances();
    test_dijkstra_distances_unreachable_node();

    test_dijkstra_missing_source_returns_all_unreachable();
    test_path_functions_missing_nodes_return_empty();

    test_add_node_return_value();
    test_isolated_nodes_in_searches();
    test_dijkstra_distances_with_isolated_node();
    test_add_node_keeps_existing_edges();

    std::cout << "All tests passed\n";

    return 0;
}