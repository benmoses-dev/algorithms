#include "mst.hpp"
#include <cassert>
#include <iostream>

using namespace algo::mst;

void test_simple_tree() {
    std::cout << "Testing simple tree..." << std::endl;
    // Triangle: 0-1 (weight 1), 1-2 (weight 2), and 0-2 (weight 3)
    // MST should use the edges with weights 1 and 2
    std::vector<Edge> edges = {{0, 1, 1}, {1, 2, 2}, {0, 2, 3}};

    auto [mstEdges, weight] = mst(edges, 3);

    assert(mstEdges.size() == 2);
    assert(weight == 3);
    std::cout << "  [PASS] Simple tree test passed" << std::endl;
}

void test_line_graph() {
    std::cout << "Testing line graph..." << std::endl;
    // 0 -- 1 -- 2 -- 3
    std::vector<Edge> edges = {{0, 1, 5}, {1, 2, 3}, {2, 3, 7}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 15);
    std::cout << "  [PASS] Line graph test passed" << std::endl;
}

void test_single_vertex() {
    std::cout << "Testing single vertex..." << std::endl;
    std::vector<Edge> edges = {};

    auto [mstEdges, weight] = mst(edges, 1);

    assert(mstEdges.size() == 0);
    assert(weight == 0);
    std::cout << "  [PASS] Single vertex test passed" << std::endl;
}

void test_two_vertices() {
    std::cout << "Testing two vertices..." << std::endl;
    std::vector<Edge> edges = {{0, 1, 10}};

    auto [mstEdges, weight] = mst(edges, 2);

    assert(mstEdges.size() == 1);
    assert(weight == 10);
    assert(mstEdges[0].w == 10);
    std::cout << "  [PASS] Two vertices test passed" << std::endl;
}

void test_complete_graph() {
    std::cout << "Testing complete graph..." << std::endl;
    // K4: complete graph with 4 vertices
    std::vector<Edge> edges = {{0, 1, 1}, {0, 2, 4}, {0, 3, 3},
                               {1, 2, 2}, {1, 3, 5}, {2, 3, 6}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 6); // 1 + 2 + 3
    std::cout << "  [PASS] Complete graph test passed" << std::endl;
}

void test_parallel_edges() {
    std::cout << "Testing parallel edges..." << std::endl;
    // Multiple edges between the same vertices
    std::vector<Edge> edges = {{0, 1, 10},
                               {0, 1, 5}, // Better edge
                               {0, 1, 15},
                               {1, 2, 3}};

    auto [mstEdges, weight] = mst(edges, 3);

    assert(mstEdges.size() == 2);
    assert(weight == 8); // Should use 5 and 3
    std::cout << "  [PASS] Parallel edges test passed" << std::endl;
}

void test_negative_weights() {
    std::cout << "Testing negative weights..." << std::endl;
    std::vector<Edge> edges = {{0, 1, -5}, {1, 2, -3}, {0, 2, 10}};

    auto [mstEdges, weight] = mst(edges, 3);

    assert(mstEdges.size() == 2);
    assert(weight == -8);
    std::cout << "  [PASS] Negative weights test passed" << std::endl;
}

void test_equal_weights() {
    std::cout << "Testing equal weights..." << std::endl;
    std::vector<Edge> edges = {{0, 1, 5}, {1, 2, 5}, {2, 3, 5}, {0, 3, 5}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 15);
    std::cout << "  [PASS] Equal weights test passed" << std::endl;
}

void test_large_weights() {
    std::cout << "Testing large weights..." << std::endl;
    std::vector<Edge> edges = {
        {0, 1, 1000000000LL}, {1, 2, 2000000000LL}, {0, 2, 5000000000LL}};

    auto [mstEdges, weight] = mst(edges, 3);

    assert(mstEdges.size() == 2);
    assert(weight == 3000000000LL);
    std::cout << "  [PASS] Large weights test passed" << std::endl;
}

void test_self_loops() {
    std::cout << "Testing self loops..." << std::endl;
    // Self loops should be ignored
    std::vector<Edge> edges = {{0, 0, 100}, // Self loop
                               {0, 1, 5},
                               {1, 1, 50}, // Self loop
                               {1, 2, 3}};

    auto [mstEdges, weight] = mst(edges, 3);

    assert(mstEdges.size() == 2);
    assert(weight == 8);
    std::cout << "  [PASS] Self loops test passed" << std::endl;
}

void test_already_sorted() {
    std::cout << "Testing already sorted edges..." << std::endl;
    std::vector<Edge> edges = {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {0, 3, 10}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 6);
    std::cout << "  [PASS] Already sorted test passed" << std::endl;
}

void test_reverse_sorted() {
    std::cout << "Testing reverse sorted edges..." << std::endl;
    std::vector<Edge> edges = {{0, 3, 10}, {2, 3, 3}, {1, 2, 2}, {0, 1, 1}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 6);
    std::cout << "  [PASS] Reverse sorted test passed" << std::endl;
}

void test_star_graph() {
    std::cout << "Testing star graph..." << std::endl;
    // Central node 0 connected to all other nodes
    std::vector<Edge> edges = {{0, 1, 1}, {0, 2, 2}, {0, 3, 3}, {0, 4, 4}};

    auto [mstEdges, weight] = mst(edges, 5);

    assert(mstEdges.size() == 4);
    assert(weight == 10);
    std::cout << "  [PASS] Star graph test passed" << std::endl;
}

void test_cycle_graph() {
    std::cout << "Testing cycle graph..." << std::endl;
    // Cycle: 0-1-2-3-0 with increasing weights
    std::vector<Edge> edges = {
        {0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 0, 4} // This edge should be excluded
    };

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 6);
    std::cout << "  [PASS] Cycle graph test passed" << std::endl;
}

void test_disconnected_components() {
    std::cout << "Testing disconnected graph..." << std::endl;
    std::vector<Edge> edges = {{0, 1, 5}, {2, 3, 7}};

    // This creates a forest, not a tree
    bool failed = false;
    try {
        auto [mstEdges, weight] = mst(edges, 4);
    } catch (...) {
        failed = true;
    }
    assert(failed == true);
    std::cout << "  [PASS] Disconnected graph test passed" << std::endl;
}

void test_zero_weight_edges() {
    std::cout << "Testing zero weight edges..." << std::endl;
    std::vector<Edge> edges = {{0, 1, 0}, {1, 2, 0}, {2, 3, 5}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 5);
    std::cout << "  [PASS] Zero weight edges test passed" << std::endl;
}

void test_bidirectional_edges() {
    std::cout << "Testing bidirectional edges..." << std::endl;
    // Same edge added in both directions
    std::vector<Edge> edges = {{0, 1, 5},
                               {1, 0, 5}, // Same edge, reverse direction
                               {1, 2, 3}};

    auto [mstEdges, weight] = mst(edges, 3);

    // Should only use one direction of the 0-1 edge
    assert(mstEdges.size() == 2);
    assert(weight == 8);
    std::cout << "  [PASS] Bidirectional edges test passed" << std::endl;
}

void test_edge_ordering_preserved() {
    std::cout << "Testing edge ordering in result..." << std::endl;
    std::vector<Edge> edges = {{2, 3, 1}, {0, 1, 2}, {1, 2, 3}};

    auto [mstEdges, weight] = mst(edges, 4);

    assert(mstEdges.size() == 3);
    assert(weight == 6);

    // Edges should be in sorted order by weight
    assert(mstEdges[0].w <= mstEdges[1].w);
    assert(mstEdges[1].w <= mstEdges[2].w);
    std::cout << "  [PASS] Edge ordering test passed" << std::endl;
}

void test_empty_graph() {
    std::cout << "Testing empty graph (n=0)..." << std::endl;
    std::vector<Edge> edges = {};

    auto [mstEdges, weight] = mst(edges, 0);

    assert(mstEdges.size() == 0);
    assert(weight == 0);
    std::cout << "  [PASS] Empty graph test passed" << std::endl;
}

int main() {
    std::cout << "Running MST tests...\n" << std::endl;

    test_simple_tree();
    test_line_graph();
    test_single_vertex();
    test_two_vertices();
    test_complete_graph();
    test_parallel_edges();
    test_negative_weights();
    test_equal_weights();
    test_large_weights();
    test_self_loops();
    test_already_sorted();
    test_reverse_sorted();
    test_star_graph();
    test_cycle_graph();
    test_disconnected_components();
    test_zero_weight_edges();
    test_bidirectional_edges();
    test_edge_ordering_preserved();
    test_empty_graph();

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
