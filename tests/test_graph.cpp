#undef NDEBUG

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <vector>
#include <limits>
#include <fstream>

#include "graph.h"

using graphkit::graph;

bool throws_invalid_argument(const std::function<void()>& action){
    try{
        action();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

void test_dijkstra_finds_lowest_cost_route() {
    graph g;
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

    assert(result1.distance == graph::unreachable);
    assert(result1.path.empty());

    assert(result2.distance == graph::unreachable);
    assert(result2.path.empty());

    assert(result3.distance == graph::unreachable);
    assert(result3.path.empty());

    assert(result4.distance == graph::unreachable);
    assert(result4.path.empty());
}

void test_start_equals_destination_for_every_route_function(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(2, 3, 2);

    std::vector<graph::NodeID> only_one = {1};

    assert(g.path_dfs(1, 1) == only_one);
    assert(g.path_bfs(1, 1) == only_one);

    auto astar = g.path_astar(1, 1);
    assert(astar.path == only_one);
    assert(astar.distance == 0);

    auto result = g.path_dijkstra(1, 1);
    assert(result.distance == 0);
    assert(result.path == only_one);
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
    assert(astar_result.path == dijkstra_result.path);
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

    assert(result1.distance == graph::unreachable);
    assert(result1.path.empty());

    assert(result2.distance == graph::unreachable);
    assert(result2.path.empty());

    assert(result3.distance == graph::unreachable);
    assert(result3.path.empty());

    assert(result4.distance == graph::unreachable);
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

    assert(throws_invalid_argument([&](){g.bfs(5);}));
    assert(throws_invalid_argument([&](){g.dfs(5);}));

    assert(!throws_invalid_argument([&](){ g.bfs(1); }));
    assert(g.dijkstra(1).size() == 4);
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

void test_dijkstra_missing_source_throws(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(2, 3, 2);

    assert(throws_invalid_argument([&](){g.dijkstra(99);}));
    assert(!throws_invalid_argument([&]{ g.dijkstra(1); }));
    assert(g.dijkstra(1).size() == 3);
}

void test_path_functions_missing_nodes_throw(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(2, 3, 2);

    assert(throws_invalid_argument([&](){g.path_dfs(99, 1);} ));
    assert(throws_invalid_argument([&](){g.path_dfs(1, 99);} ));
    assert(throws_invalid_argument([&](){g.path_bfs(99, 1);} ));
    assert(throws_invalid_argument([&](){g.path_bfs(1, 99);} ));
    assert(throws_invalid_argument([&](){g.path_dijkstra(99, 1);}));
    assert(throws_invalid_argument([&](){g.path_dijkstra(1, 99);}));
    assert(throws_invalid_argument([&](){g.path_astar(99, 1);}));
    assert(throws_invalid_argument([&](){g.path_astar(1, 99);}));

    assert(!throws_invalid_argument([&](){ g.path_dfs(1, 3); }));
    
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

    assert(throws_invalid_argument([&](){ g.dfs(99); }));
}

void test_dijkstra_distances_with_isolated_node(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_node(3);                                  // exists, but has no edges

    auto distances = g.dijkstra(1);

    assert(distances.size() == 3);
    assert(distances.at(1) == 0);
    assert(distances.at(2) == 5);
    assert(distances.at(3) == graph::unreachable);
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

void test_add_edge_rejects_invalid_weights(){
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();

    graph g;
    assert(throws_invalid_argument([&](){ g.add_edge(1, 2, -1); }));
    assert(throws_invalid_argument([&](){ g.add_edge(1, 2, nan); }));
    assert(throws_invalid_argument([&](){ g.add_edge(1, 2, inf); }));
    assert(throws_invalid_argument([&](){ g.add_edge(1, 2, -inf); }));

    // none of the rejected calls may have added a node: node 1 must still not exist
    assert(throws_invalid_argument([&](){ g.dfs(1); }));
}

void test_add_edge_accepts_valid_weights(){
    graph g;
    assert(!throws_invalid_argument([&](){ g.add_edge(1, 2, 0); }));
    assert(!throws_invalid_argument([&](){ g.add_edge(2, 3, 2.5); }));

    assert(g.path_dijkstra(1, 3).distance == 2.5);        // 0 + 2.5
}

void test_negative_node_ids_are_accepted(){
    graph g;
    assert(!throws_invalid_argument([&](){ g.add_edge(-1, 2, 1); }));
    assert(!throws_invalid_argument([&](){ g.add_edge(2, -3, 1); }));
    assert(!throws_invalid_argument([&](){ g.add_node(-9); }));

    std::vector<graph::NodeID> expected = {-1, 2, -3};
    std::vector<graph::NodeID> only_minus_nine = {-9};

    assert(g.path_bfs(-1, -3) == expected);               // -1 works as the start node
    assert(g.path_dijkstra(-1, -3).distance == 2);
    assert(g.dfs(-9) == only_minus_nine);
}

void test_explored_nodes_start_equals_destination(){
    graph g;
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);

    assert(g.path_dijkstra(1, 1).numberOfExploredNodes == 1);
    assert(g.path_astar(1, 1).numberOfExploredNodes == 1);
}

void test_explored_nodes_when_there_is_no_route(){
    graph g;
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_node(9);

    assert(g.path_dijkstra(1, 9).numberOfExploredNodes == 3);
    assert(g.path_astar(1, 9).numberOfExploredNodes == 3);
}


void test_explored_nodes_includes_outdated_queue_entries(){
    graph g;
    g.add_edge(1, 2, 3);
    g.add_edge(1, 3, 1);
    g.add_edge(2, 4, 2);
    g.add_edge(3, 4, 5);
    g.add_edge(3, 2, 1);

    // node 2 is queued twice (first at 3, then at 2), so one pop is an outdated entry
    assert(g.path_dijkstra(1, 4).numberOfExploredNodes == 5);
}

void test_astar_default_heuristic_explores_like_dijkstra(){
    graph g;
    g.add_edge(1, 2, 3);
    g.add_edge(1, 3, 1);
    g.add_edge(2, 4, 2);
    g.add_edge(3, 4, 5);
    g.add_edge(3, 2, 1);

    auto dijkstra = g.path_dijkstra(1, 4);
    auto astar = g.path_astar(1, 4);

    assert(astar.numberOfExploredNodes == dijkstra.numberOfExploredNodes);
}

void test_astar_calls_the_heuristic_it_is_given(){
    graph g;
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);

    int calls = 0;
    auto counting = [&](graph::NodeID){ ++calls; return 0.0; };

    g.path_astar(1, 3, counting);
    assert(calls > 0);
}

void test_edges_can_be_walked_in_both_directions(){
    graph g;
    g.add_edge(1, 2, 5);
    g.add_edge(3, 2, 2);                       // both edges have node 2 as the second argument

    std::vector<graph::NodeID> forward  = {1, 2, 3};
    std::vector<graph::NodeID> backward = {3, 2, 1};

    assert(g.path_bfs(1, 3) == forward);
    assert(g.path_bfs(3, 1) == backward);
    assert(g.path_dfs(1, 3) == forward);
    assert(g.path_dfs(3, 1) == backward);

    assert(g.path_dijkstra(1, 3).distance == 7);
    assert(g.path_dijkstra(3, 1).distance == 7);

    assert(g.dfs(2).size() == 3);
    assert(g.bfs(2).size() == 3);
}


void test_directed_edge_cannot_be_walked_backwards(){
    graph g;
    g.add_directed_edge(1, 2, 5);

    std::vector<graph::NodeID> forward = {1, 2};

    assert(g.path_bfs(1, 2) == forward);
    assert(g.path_dfs(1, 2) == forward);
    assert(g.path_dijkstra(1, 2).distance == 5);

    // both nodes exist, so nothing throws: there is simply no route back
    assert(g.path_bfs(2, 1).empty());
    assert(g.path_dfs(2, 1).empty());
    assert(g.path_dijkstra(2, 1).path.empty());
    assert(g.path_dijkstra(2, 1).distance == graph::unreachable);
    assert(g.path_astar(2, 1).path.empty());
    assert(g.dfs(2).size() == 1);                  // from node 2 only node 2 itself is reachable
}

void test_directed_edges_with_different_costs_each_way(){
    graph g;
    g.add_directed_edge(1, 2, 4);
    g.add_directed_edge(2, 1, 9);

    assert(g.path_dijkstra(1, 2).distance == 4);
    assert(g.path_dijkstra(2, 1).distance == 9);
    assert(g.path_astar(1, 2).distance == 4);
    assert(g.path_astar(2, 1).distance == 9);
    assert(g.dijkstra(1).at(2) == 4);
    assert(g.dijkstra(2).at(1) == 9);
}

void test_mixed_directed_and_undirected_edges(){
    graph g;
    g.add_directed_edge(1, 2, 4);                  // one-way
    g.add_directed_edge(2, 3, 1);                  // one-way
    g.add_edge(3, 4, 2);                           // two-way
    g.add_directed_edge(4, 1, 9);                  // one-way, from a higher ID back to a lower one

    std::vector<graph::NodeID> two_to_one = {2, 3, 4, 1};

    assert(g.path_dijkstra(1, 4).distance == 7);   // 1 -> 2 -> 3 -> 4
    assert(g.path_dijkstra(4, 2).distance == 13);  // 4 -> 1 -> 2
    assert(g.path_dijkstra(4, 3).distance == 2);   // the two-way edge, walked backwards
    assert(g.path_dijkstra(3, 1).distance == 11);  // 3 -> 4 -> 1
    assert(g.path_bfs(2, 1) == two_to_one);        // the only route from 2 to 1
}

void test_rejected_directed_edge_changes_nothing(){
    graph g;
    assert(throws_invalid_argument([&](){ g.add_directed_edge(1, 2, -1); }));
    assert(throws_invalid_argument([&](){ g.add_directed_edge(1, 2, std::numeric_limits<graph::Weight>::quiet_NaN()); }));
    assert(throws_invalid_argument([&](){ g.add_directed_edge(1, 2, std::numeric_limits<graph::Weight>::infinity()); }));

    // none of the rejected calls may have added a node: node 1 must still not exist
    assert(throws_invalid_argument([&](){ g.dfs(1); }));
}

void test_two_opposite_directed_edges_behave_like_one_undirected_edge(){
    graph directed_pair;
    directed_pair.add_directed_edge(1, 2, 3);
    directed_pair.add_directed_edge(2, 1, 3);

    graph undirected;
    undirected.add_edge(1, 2, 3);

    assert(directed_pair.path_dijkstra(1, 2).distance == undirected.path_dijkstra(1, 2).distance);
    assert(directed_pair.path_dijkstra(2, 1).distance == undirected.path_dijkstra(2, 1).distance);
    assert(directed_pair.path_bfs(2, 1) == undirected.path_bfs(2, 1));
    assert(directed_pair.dfs(1) == undirected.dfs(1));
}

int main(){

    test_dijkstra_finds_lowest_cost_route();
    test_dijkstra_unreachable_destination();

    test_astar_zero_heuristic_matches_dijkstra();
    test_astar_manhattan_heuristic_finds_optimal_path();
    test_astar_unreachable_destination();

    test_DFS_BFS_cycle_check();
    test_bfs_returns_path_with_fewest_edges();
    test_dfs_bfs_missing_nodes();

    test_graph_duplicate_edges_and_self_loops();
    test_dijkstra_zero_weight_edges();

    test_path_dfs_returns_a_non_shortest_route();
    test_dfs_visit_order();
    test_bfs_visit_order();
    test_dijkstra_distances();

    test_dijkstra_missing_source_throws();
    test_path_functions_missing_nodes_throw();

    test_add_node_return_value();
    test_isolated_nodes_in_searches();
    test_dijkstra_distances_with_isolated_node();
    test_add_node_keeps_existing_edges();

    test_negative_node_ids_are_accepted();
    test_add_edge_accepts_valid_weights();
    test_add_edge_rejects_invalid_weights();

    test_start_equals_destination_for_every_route_function();

    
    test_explored_nodes_includes_outdated_queue_entries();
    test_explored_nodes_when_there_is_no_route();
    test_explored_nodes_start_equals_destination();
    test_astar_calls_the_heuristic_it_is_given();
    test_astar_default_heuristic_explores_like_dijkstra();

    test_edges_can_be_walked_in_both_directions();

    test_rejected_directed_edge_changes_nothing();
    test_directed_edges_with_different_costs_each_way();
    test_directed_edge_cannot_be_walked_backwards();
    test_mixed_directed_and_undirected_edges();
    test_two_opposite_directed_edges_behave_like_one_undirected_edge();

    std::cout << "All tests passed\n";

    return 0;
}