# High-Performance Algorithm Library

A C++ algorithm library focused on performance benchmarking and multi-threaded implementations.

## Algorithms Implemented

### Data Structures
- Fenwick tree (BIT)
- Segment tree
- Sparse table
- Teque (triple-ended queue)
- Trie

### Dynamic Programming
- Knapsack
- Shortest common supersequence
- Pascal's triangle
- Combinations and permutations

### Computational Goemetry
- Line intersections
- Point-in-polygon checks

### Graph Algorithms
- BFS (directed and undirected) with distances, cycle and bipartite checks
- DFS (directed and undirected) with critical component analysis
- Dijkstra's shortest path
- Dinic's maximum flow and minimum cut
- Disjoint-set union implementation with sum and size tracking
- Hungarian algorithm for fast minimum-cost bipartite matching
- Minimum-cost, maximum-flow for capacitated, cost-minimised bipartite matching
- Kruskal's MST
- Multi-threaded graph-traversal benchmarking algorithm for CPU and memory

### Number Theory
- Fourier transforms - recursive and iterative FFT
- Modular arithmetic library, CRT and NTT
- Prime number sieves and reductions
- Linear regression ensemble for worst-case predictions using multiple time periods

## Testing

This repository uses Google Test Framework. To build and test, copy the bash aliases into the relevant location on your system and source them:
```
source ~/.bash_aliases
```


From each individual sub-project, run:
```
cmakedeb && cd ./build-debug && ctest

```

