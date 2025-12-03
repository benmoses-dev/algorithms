#include "mcmf.hpp"
#include <cassert>
#include <iostream>

using namespace algo::flow;

int tests_run = 0;
int tests_passed = 0;

void print_test(const char *name) {
    tests_run++;
    std::cout << "Running " << name << "... ";
}

void pass_test() {
    tests_passed++;
    std::cout << "PASSED\n";
}

void test_simple_path() {
    print_test("test_simple_path");

    // s --[cap=10, cost=2]--> t
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 10, 2);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == 10);
    assert(cost == 20);

    pass_test();
}

void test_two_paths_choose_cheaper() {
    print_test("test_two_paths_choose_cheaper");

    // s --[cap=5, cost=1]--> t
    //  \--[cap=5, cost=3]--/
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 5, 1);
    mcmf.addEdge(0, 1, 5, 3);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == 10);
    assert(cost == 20); // 5*1 + 5*3

    pass_test();
}

void test_diamond_graph() {
    print_test("test_diamond_graph");

    /**
     *      1
     *    /   \
     *   s     t
     *    \   /
     *      2
     */
    MinCostMaxFlow mcmf(4);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(0, 2, 10, 2);
    mcmf.addEdge(1, 3, 10, 2);
    mcmf.addEdge(2, 3, 10, 1);

    auto [flow, cost] = mcmf.solve(0, 3);

    assert(flow == 20);
    assert(cost == 60);

    pass_test();
}

void test_no_path() {
    print_test("test_no_path");

    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);

    auto [flow, cost] = mcmf.solve(0, 2);

    assert(flow == 0);
    assert(cost == 0);

    pass_test();
}

void test_bottleneck() {
    print_test("test_bottleneck");

    // s --[cap=10, cost=1]--> a --[cap=5, cost=1]--> t
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 2, 5, 1);

    auto [flow, cost] = mcmf.solve(0, 2);

    assert(flow == 5);
    assert(cost == 10);

    pass_test();
}

void test_multiple_paths() {
    print_test("test_multiple_paths");

    MinCostMaxFlow mcmf(4);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 3, 10, 1);
    mcmf.addEdge(0, 2, 5, 2);
    mcmf.addEdge(2, 3, 5, 2);

    auto [flow, cost] = mcmf.solve(0, 3);

    assert(flow == 15);
    assert(cost == 40);

    pass_test();
}

void test_self_loop() {
    print_test("test_self_loop");

    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 0, 10, 1);
    mcmf.addEdge(0, 1, 5, 2);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == 5);
    assert(cost == 10);

    pass_test();
}

void test_zero_capacity() {
    print_test("test_zero_capacity");

    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 0, 1);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == 0);
    assert(cost == 0);

    pass_test();
}

void test_large_capacity() {
    print_test("test_large_capacity");

    MinCostMaxFlow mcmf(2);
    ll large_cap = 1000000000LL;
    mcmf.addEdge(0, 1, large_cap, 1);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == large_cap);
    assert(cost == large_cap);

    pass_test();
}

void test_zero_cost() {
    print_test("test_zero_cost");

    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 10, 0);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == 10);
    assert(cost == 0);

    pass_test();
}

void test_complex_network() {
    print_test("test_complex_network");

    MinCostMaxFlow mcmf(5);
    mcmf.addEdge(0, 1, 5, 2);
    mcmf.addEdge(0, 2, 5, 3);
    mcmf.addEdge(1, 3, 10, 1);
    mcmf.addEdge(2, 3, 10, 1);
    mcmf.addEdge(3, 4, 10, 2);

    auto [flow, cost] = mcmf.solve(0, 4);

    assert(flow == 10);
    assert(cost == 55);

    pass_test();
}

void test_source_equals_sink() {
    print_test("test_source_equals_sink");

    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);

    auto [flow, cost] = mcmf.solve(0, 0);

    assert(flow == 0);
    assert(cost == 0);

    pass_test();
}

void test_reset() {
    print_test("test_reset");

    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 10, 2);

    auto [flow1, cost1] = mcmf.solve(0, 1);
    assert(flow1 == 10);
    assert(cost1 == 20);

    mcmf.reset();
    auto [flow2, cost2] = mcmf.solve(0, 1);
    assert(flow2 == 10);
    assert(cost2 == 20);

    pass_test();
}

void test_get_flows() {
    print_test("test_get_flows");

    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 2, 5, 2);

    mcmf.solve(0, 2);

    auto flows = mcmf.getFlows();

    std::size_t positive_flows = 0;
    for (const auto &f : flows) {
        if (f.flow > 0)
            positive_flows++;
    }

    assert(positive_flows == 2);

    pass_test();
}

void test_negative_costs() {
    print_test("test_negative_costs");

    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, -5);
    mcmf.addEdge(1, 2, 10, 3);

    mcmf.initPotentials(0);
    auto [flow, cost] = mcmf.solve(0, 2);

    assert(flow == 10);
    assert(cost == -20);

    pass_test();
}

void test_multiple_resets() {
    print_test("test_multiple_resets");

    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 2, 10, 2);

    for (std::size_t i = 0; i < 10; i++) {
        auto [flow, cost] = mcmf.solve(0, 2);
        assert(flow == 10);
        assert(cost == 30);
        mcmf.reset();
    }

    pass_test();
}

void test_assignment_problem() {
    print_test("test_assignment_problem");

    // 2 workers, 2 jobs
    // Optimal: W0->J0 (5) + W1->J1 (3) = 8

    std::size_t source = 0;
    std::size_t w0 = 1, w1 = 2;
    std::size_t j0 = 3, j1 = 4;
    std::size_t sink = 5;

    MinCostMaxFlow mcmf(6);

    mcmf.addEdge(source, w0, 1, 0);
    mcmf.addEdge(source, w1, 1, 0);

    mcmf.addEdge(w0, j0, 1, 5);
    mcmf.addEdge(w0, j1, 1, 10);
    mcmf.addEdge(w1, j0, 1, 8);
    mcmf.addEdge(w1, j1, 1, 3);

    mcmf.addEdge(j0, sink, 1, 0);
    mcmf.addEdge(j1, sink, 1, 0);

    auto [flow, cost] = mcmf.solve(source, sink);

    assert(flow == 2);
    assert(cost == 8);

    pass_test();
}

void test_capacitated_assignment() {
    print_test("test_capacitated_assignment");

    // Workers can take multiple jobs

    std::size_t source = 0;
    std::size_t w0 = 1, w1 = 2;
    std::size_t j0 = 3, j1 = 4, j2 = 5;
    std::size_t sink = 6;

    MinCostMaxFlow mcmf(7);

    mcmf.addEdge(source, w0, 2, 0);
    mcmf.addEdge(source, w1, 1, 0);

    mcmf.addEdge(w0, j0, 1, 5);
    mcmf.addEdge(w0, j1, 1, 3);
    mcmf.addEdge(w0, j2, 1, 7);
    mcmf.addEdge(w1, j0, 1, 2);
    mcmf.addEdge(w1, j1, 1, 4);
    mcmf.addEdge(w1, j2, 1, 6);

    mcmf.addEdge(j0, sink, 1, 0);
    mcmf.addEdge(j1, sink, 1, 0);
    mcmf.addEdge(j2, sink, 1, 0);

    auto [flow, cost] = mcmf.solve(source, sink);

    assert(flow == 3);
    assert(cost == 12);

    pass_test();
}

void test_parallel_edges() {
    print_test("test_parallel_edges");

    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 5, 1);
    mcmf.addEdge(0, 1, 5, 2);
    mcmf.addEdge(0, 1, 5, 3);

    auto [flow, cost] = mcmf.solve(0, 1);

    assert(flow == 15);
    assert(cost == 30);

    pass_test();
}

void test_long_path() {
    print_test("test_long_path");

    MinCostMaxFlow mcmf(6);
    for (std::size_t i = 0; i < 5; i++) {
        mcmf.addEdge(i, i + 1, 10, 1);
    }

    auto [flow, cost] = mcmf.solve(0, 5);

    assert(flow == 10);
    assert(cost == 50);

    pass_test();
}

void test_wide_graph() {
    print_test("test_wide_graph");

    std::size_t n = 102;
    MinCostMaxFlow mcmf(n);

    for (std::size_t i = 1; i < n - 1; i++) {
        mcmf.addEdge(0, i, 1, static_cast<ll>(i));
        mcmf.addEdge(i, n - 1, 1, 1);
    }

    auto [flow, cost] = mcmf.solve(0, n - 1);

    assert(flow == 100);
    assert(cost == 5150);

    pass_test();
}

int main() {
    std::cout << "-- Basic Functionality --\n";
    test_simple_path();
    test_two_paths_choose_cheaper();
    test_diamond_graph();
    test_no_path();
    test_bottleneck();
    test_multiple_paths();
    test_self_loop();
    test_zero_capacity();
    test_large_capacity();
    test_zero_cost();
    test_complex_network();
    test_source_equals_sink();

    std::cout << "\n-- API Tests --\n";
    test_reset();
    test_get_flows();
    test_negative_costs();
    test_multiple_resets();

    std::cout << "\n-- Real-World Applications --\n";
    test_assignment_problem();
    test_capacitated_assignment();
    test_parallel_edges();
    test_long_path();
    test_wide_graph();

    std::cout << "\n=== Test Summary ===\n";
    std::cout << "Total tests: " << tests_run << "\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << (tests_run - tests_passed) << "\n";

    if (tests_passed == tests_run) {
        std::cout << "\n[PASS] All tests passed!\n";
        return 0;
    } else {
        std::cout << "\n[FAIL] Some tests failed\n";
        return 1;
    }
}
