# Reference benchmarks - i7-12700H 32GB DDR4

## Configuration
-O3 compiler optimisations, LTO, -march=native
vector<int> for result distances (flattened for grid)
Vector<bool> for visited

### Using max threads = 1
multiGridBFS (1000 starts, 1000000 cells) took 7.51363 seconds
multiBFS (1000 starts, 1000000 nodes) took 6.51823 seconds

multiGridBFS (1000 starts, 1000000 cells) took 8.79978 seconds
multiBFS (1000 starts, 1000000 nodes) took 6.54568 seconds

multiGridBFS (1000 starts, 1000000 cells) took 8.82326 seconds
multiBFS (1000 starts, 1000000 nodes) took 6.01461 seconds

### Using max threads = 2
multiGridBFS (1000 starts, 1000000 cells) took 4.34154 seconds
multiBFS (1000 starts, 1000000 nodes) took 3.37577 seconds

multiGridBFS (1000 starts, 1000000 cells) took 4.40443 seconds
multiBFS (1000 starts, 1000000 nodes) took 3.44539 seconds

multiGridBFS (1000 starts, 1000000 cells) took 4.4317 seconds
multiBFS (1000 starts, 1000000 nodes) took 3.44969 seconds

### Using max threads = 4
multiGridBFS (1000 starts, 1000000 cells) took 2.4623 seconds
multiBFS (1000 starts, 1000000 nodes) took 1.88793 seconds

multiGridBFS (1000 starts, 1000000 cells) took 2.55089 seconds
multiBFS (1000 starts, 1000000 nodes) took 1.93266 seconds

multiGridBFS (1000 starts, 1000000 cells) took 2.49645 seconds
multiBFS (1000 starts, 1000000 nodes) took 1.89686 seconds

### Using max threads = 8
multiGridBFS (1000 starts, 1000000 cells) took 1.55365 seconds
multiBFS (1000 starts, 1000000 nodes) took 1.1719 seconds

multiGridBFS (1000 starts, 1000000 cells) took 1.58196 seconds
multiBFS (1000 starts, 1000000 nodes) took 1.17222 seconds

multiGridBFS (1000 starts, 1000000 cells) took 1.57019 seconds
multiBFS (1000 starts, 1000000 nodes) took 1.16774 seconds

### Using max threads = 16
multiGridBFS (1000 starts, 1000000 cells) took 1.13768 seconds
multiBFS (1000 starts, 1000000 nodes) took 0.920846 seconds

multiGridBFS (1000 starts, 1000000 cells) took 1.14484 seconds
multiBFS (1000 starts, 1000000 nodes) took 0.932346 seconds

multiGridBFS (1000 starts, 1000000 cells) took 1.15502 seconds
multiBFS (1000 starts, 1000000 nodes) took 0.889665 seconds

