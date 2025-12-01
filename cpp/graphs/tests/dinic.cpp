#include "dinic.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace algo::flow;

void test_simple_flow() {
    std::cout << "Testing simple flow..." << std::endl;
    Dinic dinic(2);
    dinic.addEdge(0, 1, 10);

    ll flow = dinic.maxFlow(0, 1);
    assert(flow == 10);
    std::cout << "  [PASS] Simple flow test passed" << std::endl;
}

void test_multiple_paths() {
    std::cout << "Testing multiple paths..." << std::endl;
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 10);
    dinic.addEdge(2, 3, 10);

    ll flow = dinic.maxFlow(0, 3);
    assert(flow == 20);
    std::cout << "  [PASS] Multiple paths test passed" << std::endl;
}

void test_bottleneck() {
    std::cout << "Testing bottleneck..." << std::endl;
    // Graph with bottleneck in the middle
    //   0 --100--> 1 --5--> 2 --100--> 3
    Dinic dinic(4);
    dinic.addEdge(0, 1, 100);
    dinic.addEdge(1, 2, 5);
    dinic.addEdge(2, 3, 100);

    ll flow = dinic.maxFlow(0, 3);
    assert(flow == 5);
    std::cout << "  [PASS] Bottleneck test passed" << std::endl;
}

void test_no_path() {
    std::cout << "Testing no path..." << std::endl;
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(2, 3, 10);

    ll flow = dinic.maxFlow(0, 3);
    assert(flow == 0);
    std::cout << "  [PASS] No path test passed" << std::endl;
}

void test_complex_network() {
    std::cout << "Testing complex network..." << std::endl;
    Dinic dinic(5);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 10);
    dinic.addEdge(1, 2, 2);
    dinic.addEdge(2, 4, 9);
    dinic.addEdge(4, 3, 9);

    ll flow = dinic.maxFlow(0, 3);
    assert(flow == 19);
    std::cout << "  [PASS] Complex network test passed" << std::endl;
}

void test_undirected_edge() {
    std::cout << "Testing undirected edge..." << std::endl;
    // Undirected edge should allow flow in both directions
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10, false);
    dinic.addEdge(1, 2, 5);

    ll flow = dinic.maxFlow(0, 2);
    assert(flow == 5);
    std::cout << "  [PASS] Undirected edge test passed" << std::endl;
}

void test_self_loop() {
    std::cout << "Testing self loop..." << std::endl;
    // Self loops shouldn't affect flow
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 1, 100);
    dinic.addEdge(1, 2, 10);

    ll flow = dinic.maxFlow(0, 2);
    assert(flow == 10);
    std::cout << "  [PASS] Self loop test passed" << std::endl;
}

void test_parallel_edges() {
    std::cout << "Testing parallel edges..." << std::endl;
    Dinic dinic(2);
    dinic.addEdge(0, 1, 5);
    dinic.addEdge(0, 1, 7);
    dinic.addEdge(0, 1, 3);

    ll flow = dinic.maxFlow(0, 1);
    assert(flow == 15);
    std::cout << "  [PASS] Parallel edges test passed" << std::endl;
}

void test_reset() {
    std::cout << "Testing reset..." << std::endl;
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);

    ll flow1 = dinic.maxFlow(0, 2);
    assert(flow1 == 10);

    dinic.reset();

    ll flow2 = dinic.maxFlow(0, 2);
    assert(flow2 == 10);
    std::cout << "  [PASS] Reset test passed" << std::endl;
}

void test_get_flows() {
    std::cout << "Testing get flows..." << std::endl;
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);

    dinic.maxFlow(0, 2);

    auto flows = dinic.getFlows();

    // Should have 2 forward edges (reverse edges have cap = 0, so they're not included)
    assert(flows.size() == 2);

    // Check that forward edges have flow
    bool found_0_1 = false, found_1_2 = false;
    for (const auto &f : flows) {
        if (f.from == 0 && f.to == 1 && f.cap == 10) {
            assert(f.flow == 10);
            found_0_1 = true;
        }
        if (f.from == 1 && f.to == 2 && f.cap == 10) {
            assert(f.flow == 10);
            found_1_2 = true;
        }
    }
    assert(found_0_1 && found_1_2);
    std::cout << "  [PASS] Get flows test passed" << std::endl;
}

void test_min_cut() {
    std::cout << "Testing min cut..." << std::endl;
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 3, 10);
    dinic.addEdge(1, 2, 10);
    dinic.addEdge(3, 2, 10);

    dinic.maxFlow(0, 2);

    auto [sourceSide, sinkSide] = dinic.getMinCut(0);

    // Source side should contain 0, sink side should contain 2
    assert(std::find(sourceSide.begin(), sourceSide.end(), 0) != sourceSide.end());
    assert(std::find(sinkSide.begin(), sinkSide.end(), 2) != sinkSide.end());

    // All vertices should be accounted for
    assert(sourceSide.size() + sinkSide.size() == 4);
    std::cout << "  [PASS] Min cut test passed" << std::endl;
}

void test_min_cut_edges() {
    std::cout << "Testing min cut edges..." << std::endl;
    // 0 --5--> 1 --10--> 2
    Dinic dinic(3);
    dinic.addEdge(0, 1, 5);
    dinic.addEdge(1, 2, 10);

    ll flow = dinic.maxFlow(0, 2);
    assert(flow == 5);

    auto cutEdges = dinic.getMinCutEdges(0);

    // The cut should be at edge 0->1 (capacity 5)
    assert(cutEdges.size() == 1);
    assert(cutEdges[0].first == 0);
    assert(cutEdges[0].second == 1);
    std::cout << "  [PASS] Min cut edges test passed" << std::endl;
}

void test_min_cut_capacity() {
    std::cout << "Testing min cut capacity..." << std::endl;
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 10);
    dinic.addEdge(2, 3, 10);

    ll flow = dinic.maxFlow(0, 3);
    ll cutCapacity = dinic.getMinCutCapacity(0);

    // Max-flow min-cut theorem: flow should equal the cut capacity
    assert(flow == cutCapacity);
    assert(cutCapacity == 20);
    std::cout << "  [PASS] Min cut capacity test passed" << std::endl;
}

void test_large_capacity() {
    std::cout << "Testing large capacity..." << std::endl;
    Dinic dinic(2);
    ll large_cap = 1000000000LL;
    dinic.addEdge(0, 1, large_cap);

    ll flow = dinic.maxFlow(0, 1);
    assert(flow == large_cap);
    std::cout << "  [PASS] Large capacity test passed" << std::endl;
}

void test_zero_capacity() {
    std::cout << "Testing zero capacity..." << std::endl;
    Dinic dinic(2);
    dinic.addEdge(0, 1, 0);

    ll flow = dinic.maxFlow(0, 1);
    assert(flow == 0);
    std::cout << "  [PASS] Zero capacity test passed" << std::endl;
}

void test_source_equals_sink() {
    std::cout << "Testing source equals sink..." << std::endl;
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);

    ll flow = dinic.maxFlow(1, 1);
    assert(flow == 0);
    std::cout << "  [PASS] Source equals sink test passed" << std::endl;
}

void test_bipartite_matching() {
    std::cout << "Testing bipartite matching..." << std::endl;
    Dinic dinic(8);

    // Source to left side
    dinic.addEdge(6, 0, 1);
    dinic.addEdge(6, 1, 1);
    dinic.addEdge(6, 2, 1);

    // Matching edges
    dinic.addEdge(0, 3, 1);
    dinic.addEdge(0, 4, 1);
    dinic.addEdge(1, 3, 1);
    dinic.addEdge(1, 5, 1);
    dinic.addEdge(2, 4, 1);

    // Right side to sink
    dinic.addEdge(3, 7, 1);
    dinic.addEdge(4, 7, 1);
    dinic.addEdge(5, 7, 1);

    ll matching = dinic.maxFlow(6, 7);
    assert(matching == 3);
    std::cout << "  [PASS] Bipartite matching test passed" << std::endl;
}

void test_multiple_runs() {
    std::cout << "Testing multiple runs without reset..." << std::endl;
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);
    dinic.addEdge(2, 3, 10);

    ll flow1 = dinic.maxFlow(0, 2);
    assert(flow1 == 10);

    // Running maxFlow again without a reset should return 0
    ll flow2 = dinic.maxFlow(0, 2);
    assert(flow2 == 0);

    // After reset, it should work again
    dinic.reset();
    ll flow3 = dinic.maxFlow(0, 2);
    assert(flow3 == 10);
    std::cout << "  [PASS] Multiple runs test passed" << std::endl;
}

int main() {
    std::cout << "Running Dinic tests...\n" << std::endl;

    test_simple_flow();
    test_multiple_paths();
    test_bottleneck();
    test_no_path();
    test_complex_network();
    test_undirected_edge();
    test_self_loop();
    test_parallel_edges();
    test_reset();
    test_get_flows();
    test_min_cut();
    test_min_cut_edges();
    test_min_cut_capacity();
    test_large_capacity();
    test_zero_capacity();
    test_source_equals_sink();
    test_bipartite_matching();
    test_multiple_runs();

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
