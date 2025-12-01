#include "dsu.hpp"
#include <cassert>
#include <iostream>

using namespace algo::dsu;

void test_initialization() {
    std::cout << "Testing initialization..." << std::endl;
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    assert(dsu.getComponents() == 5);

    for (std::size_t i = 0; i < values.size(); i++) {
        assert(dsu.find(i) == i);
        assert(dsu.getSize(i) == 1);
        assert(dsu.getMaxVal(i) == values[i]);
        assert(dsu.getSum(i) == values[i]);
    }
    std::cout << "  [PASS] Initialization test passed" << std::endl;
}

void test_simple_union() {
    std::cout << "Testing simple union..." << std::endl;
    std::vector<int> values = {1, 2, 3, 4};
    DSU dsu(values);

    dsu.unite(0, 1);

    assert(dsu.find(0) == dsu.find(1));
    assert(dsu.getSize(0) == 2);
    assert(dsu.getSize(1) == 2);
    assert(dsu.getMaxVal(0) == 2);
    assert(dsu.getSum(0) == 3);
    assert(dsu.getComponents() == 3);
    std::cout << "  [PASS] Simple union test passed" << std::endl;
}

void test_union_same_root() {
    std::cout << "Testing union with same root..." << std::endl;
    std::vector<int> values = {1, 2, 3};
    DSU dsu(values);

    dsu.unite(0, 1);
    std::size_t componentsBefore = dsu.getComponents();
    std::size_t sizeBefore = dsu.getSize(0);

    dsu.unite(0, 1);

    assert(dsu.getComponents() == componentsBefore);
    assert(dsu.getSize(0) == sizeBefore);
    std::cout << "  [PASS] Union same root test passed" << std::endl;
}

void test_chain_unions() {
    std::cout << "Testing chain unions..." << std::endl;
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(2, 3);

    assert(dsu.find(0) == dsu.find(3));
    assert(dsu.getSize(0) == 4);
    assert(dsu.getMaxVal(0) == 4);
    assert(dsu.getSum(0) == 10);
    assert(dsu.getComponents() == 2);
    std::cout << "  [PASS] Chain unions test passed" << std::endl;
}

void test_path_compression() {
    std::cout << "Testing path compression..." << std::endl;
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(2, 3);

    std::size_t root = dsu.find(3);

    assert(dsu.find(3) == root);
    assert(dsu.find(2) == root);
    assert(dsu.find(1) == root);
    assert(dsu.find(0) == root);
    std::cout << "  [PASS] Path compression test passed" << std::endl;
}

void test_max_value_tracking() {
    std::cout << "Testing max value tracking..." << std::endl;
    std::vector<int> values = {5, 10, 3, 15, 7};
    DSU dsu(values);

    dsu.unite(0, 1);
    assert(dsu.getMaxVal(0) == 10);

    dsu.unite(0, 3);
    assert(dsu.getMaxVal(0) == 15);

    dsu.unite(2, 4);
    assert(dsu.getMaxVal(2) == 7);
    std::cout << "  [PASS] Max value tracking test passed" << std::endl;
}

void test_sum_tracking() {
    std::cout << "Testing sum tracking..." << std::endl;
    std::vector<int> values = {10, 20, 30, 40};
    DSU dsu(values);

    dsu.unite(0, 1);
    assert(dsu.getSum(0) == 30);

    dsu.unite(2, 3);
    assert(dsu.getSum(2) == 70);

    dsu.unite(0, 2);
    assert(dsu.getSum(0) == 100);
    std::cout << "  [PASS] Sum tracking test passed" << std::endl;
}

void test_component_counting() {
    std::cout << "Testing component counting..." << std::endl;
    std::vector<int> values = {1, 2, 3, 4, 5, 6};
    DSU dsu(values);

    assert(dsu.getComponents() == 6);

    dsu.unite(0, 1);
    assert(dsu.getComponents() == 5);

    dsu.unite(2, 3);
    assert(dsu.getComponents() == 4);

    dsu.unite(4, 5);
    assert(dsu.getComponents() == 3);

    dsu.unite(0, 2);
    assert(dsu.getComponents() == 2);

    dsu.unite(0, 4);
    assert(dsu.getComponents() == 1);
    std::cout << "  [PASS] Component counting test passed" << std::endl;
}

void test_negative_values() {
    std::cout << "Testing negative values..." << std::endl;
    std::vector<int> values = {-5, -10, 3, -2};
    DSU dsu(values);

    assert(dsu.getMaxVal(0) == -5);
    assert(dsu.getMaxVal(1) == -10);
    assert(dsu.getMaxVal(2) == 3);
    assert(dsu.getSum(0) == -5);
    assert(dsu.getSum(1) == -10);
    std::cout << "  [PASS] Negative values test passed" << std::endl;
}

void test_mixed_sign_values() {
    std::cout << "Testing mixed sign values..." << std::endl;
    std::vector<int> values = {-10, 5, -3, 8, -1};
    DSU dsu(values);

    dsu.unite(0, 1);
    assert(dsu.getSum(0) == -5);
    assert(dsu.getMaxVal(0) == 5);

    dsu.unite(2, 3);
    assert(dsu.getSum(2) == 5);
    assert(dsu.getMaxVal(2) == 8);
    std::cout << "  [PASS] Mixed sign values test passed" << std::endl;
}

void test_large_values() {
    std::cout << "Testing large values..." << std::endl;
    std::vector<int> values = {1000000, 2000000, 3000000};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(1, 2);

    assert(dsu.getSum(0) == 6000000);
    assert(dsu.getMaxVal(0) == 3000000);
    std::cout << "  [PASS] Large values test passed" << std::endl;
}

void test_transitive_unions() {
    std::cout << "Testing transitive unions..." << std::endl;
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(3, 4);
    dsu.unite(1, 3);

    assert(dsu.find(0) == dsu.find(4));
    assert(dsu.getSize(0) == 4);
    assert(dsu.getComponents() == 2);
    std::cout << "  [PASS] Transitive unions test passed" << std::endl;
}

int main() {
    std::cout << "Running DSU tests...\n" << std::endl;

    test_initialization();
    test_simple_union();
    test_union_same_root();
    test_chain_unions();
    test_path_compression();
    test_max_value_tracking();
    test_sum_tracking();
    test_component_counting();
    test_negative_values();
    test_mixed_sign_values();
    test_large_values();
    test_transitive_unions();

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
