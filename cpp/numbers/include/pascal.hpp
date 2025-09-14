#include <vector>

using vll = std::vector<long long>;
using vvll = std::vector<vll>;

/**
 * Use Pascal's triangle to compute binomial coefficients.
 */
inline vvll pascalsTriangle(int maxN, int maxR) {
    vvll pt(maxN + 1, vll(maxR + 1, 0));
    for (int i = 0; i <= maxN; i++) {
        pt[i][0] = 1;
        for (int j = 1; j <= std::min(i, maxR); j++) {
            pt[i][j] = pt[i - 1][j - 1] + pt[i - 1][j];
        }
    }
    return pt;
}
