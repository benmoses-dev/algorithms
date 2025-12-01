#include "hungarian.hpp"
#include <cassert>
#include <iostream>

int main() {
    using namespace algo::flow;

    std::vector<std::vector<int>> grid1 = {{4, 1}, {2, 3}};

    HungarianResult res1 = hungarian(grid1);

    assert(res1.assignments.size() == 2);
    assert(res1.assignments[0] == 1);
    assert(res1.assignments[1] == 0);
    assert(res1.cost == 3);
    assert(res1.totalCols == 2);

    std::vector<std::vector<int>> grid2 = {{5, 8, 7}, {6, 4, 3}};

    HungarianResult res2 = hungarian(grid2);
    int expectedCost2 = 5 + 3;
    assert(res2.assignments.size() == 2);
    assert(res2.cost == expectedCost2);
    assert(res2.totalCols == 2);

    std::vector<std::vector<int>> grid3 = {{90, 75, 75}, {35, 85, 55}, {125, 95, 90}};

    HungarianResult res3 = hungarian(grid3);
    assert(res3.assignments[0] == 1);
    assert(res3.assignments[1] == 0);
    assert(res3.assignments[2] == 2);
    assert(res3.cost == 75 + 35 + 90);
    assert(res3.totalCols == 3);

    std::vector<std::vector<int>> grid4 = {{10, 20, 5}};
    HungarianResult res4 = hungarian(grid4);
    assert(res4.assignments.size() == 1);
    assert(res4.assignments[0] == 2); // minimal cost = 5
    assert(res4.cost == 5);
    assert(res4.totalCols == 1);

    bool exceptionCaught = false;
    try {
        std::vector<std::vector<int>> grid5 = {{1}, {2}};
        HungarianResult res5 = hungarian(grid5);
    } catch (const std::invalid_argument &e) {
        exceptionCaught = true;
    }
    assert(exceptionCaught);

    std::cout << "All tests passed!\n";
    return 0;
}
