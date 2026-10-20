#include <cassert>
#include <cstdlib>

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

int heuristic(int id){

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

    auto result = g.path_astar(1, 9, heuristic);

    assert(result.distance == 4);
    assert(!result.path.empty());

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

int main(){

    test_dijkstra_finds_lowest_cost_route();
    test_dijkstra_unreachable_destination();
    test_dijkstra_start_equals_destination();

    test_astar_zero_heuristic_matches_dijkstra();
    test_astar_manhattan_heuristic_finds_optimal_path();
    test_astar_unreachable_destination();

    return 0;
}