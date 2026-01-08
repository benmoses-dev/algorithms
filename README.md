# High-Performance Algorithm Library

A C++ algorithm library focused on performance benchmarking and multi-threaded implementations.

## Algorithms Implemented

### Data Structures
- Fenwick tree (BIT)
- Segment tree
- Sparse table
- Suffix array and Burrows-Wheeler transform
- Teque (triple-ended queue)
- Trie

### Dynamic Programming
- Knapsack
- Shortest common supersequence
- Longest common subsequence
- Longest increasing subsequence
- Pascal's triangle
- Combinations and permutations

### Computational Goemetry
- Line intersections
- Point-in-polygon checks
- Polygon area and convex hull (monotonic chain)

### Graph Algorithms
- BFS (directed and undirected) with distances, cycle and bipartite checks
- Kahn's algorithm for topological sort and longest path (DAG)
- DFS (directed and undirected) with critical component analysis and shortest path (DAG)
- Dijkstra's shortest path with Johnson's re-weighting
- Dinic's maximum flow and minimum cut
- Disjoint-set union implementation with sum and size tracking
- Floyd-Warshall all pairs shortest path
- Bellman-Ford and SPFA for negative-weight shortest-path
- Hungarian algorithm for fast minimum-cost bipartite matching
- Minimum-cost, maximum-flow for capacitated, cost-minimised bipartite matching
- Kruskal's MST
- Multi-threaded graph-traversal benchmarking algorithm for CPU and memory

### Number Theory
- Fourier transforms - recursive and iterative radix-2 Cooley-Tukey FFT
- Modular arithmetic library - CRT, NTT, and matrix exponentiation
- Prime number sieves and reductions
- Linear regression ensemble for worst-case predictions using multiple time periods

### String Algorithms
- Aho Corasick automaton for multi-pattern string matching
- KMP/Z algorithms for single-pattern string matching
- Rolling polynomial hash algorithms, including longest common substring for multiple strings

## Testing

This repository uses Google Test Framework. You will need to install this on your system to run tests.

To build and test, copy the bash aliases into the relevant location on your system and source them:
```
source ~/.bash_aliases
```

From each individual sub-project, run:
```
cmakedeb && cd ./build-debug && ctest

```

