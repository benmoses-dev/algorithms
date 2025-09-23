#include "scs.hpp"
#include <chrono>
#include <iostream>
#include <string>

using namespace algo::strings;

int main() {
    std::string a, b;
    std::cout << "Please enter the first string: ";
    std::cin >> a;
    std::cout << "Please enter the second string: ";
    std::cin >> b;

    std::string first = "";
    std::string second = "";
    for (int i = 1; i < 1000; i++) {
        first += a;
        second += b;
    }

    size_t n = first.size();
    size_t m = second.size();

    std::cout << "Length of string a: " << n << "\n";
    std::cout << "Length of string b: " << m << "\n";

    using ull = unsigned long long;
    ull p = (ull)(m + 1) * (n + 1);
    ull bytes = p * sizeof(size_t);
    ull gb = bytes >> 30;
    std::cout << "This test will use " << gb << " GB of RAM\n";
    if (gb > 4ULL) {
        std::cout << "You will use more than 4GB RAM, aborting..." << "\n";
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::string scs = shortestSupersequence(first, second);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Length of shortest supersequence: " << scs.size() << "\n";
    std::cout
        << "Execution time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n";

    std::cout << "First 100 chars of SCS: " << scs.substr(0, 100) << "\n";
    std::cout << "Last 100 chars of SCS: " << scs.substr(scs.size() - 100, 100) << "\n";

    return 0;
}
