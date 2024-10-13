# Project 2
To compile the code use the flags: -mavx2
To run the project, execute main.o

## Rubric Section 1: Software Implementation
- Arbitrary matrix size is enabled, see the generate_matrix function in main.c. This function accepts two integers as parameters (rows and cols) that control the matrix size.
- Multithreading is enabled, see the function multiply_matrices_multithread
- SIMD is enabled, see the function multiply_matrices_simd
- Cache Miss minimization is enabled, see the function multiply_matrices_block
- The multiply_matrices_multithread function accepts a parameter num_threads to control how many threads are spawned

## Rubric Section 2: Experiments
- At least 9 results with varying size and sparsity
  - In the left plot, 1000x1000 matrices were multiplied with sparsity ranging from .01 to .1
  - In the middle plot, 5000x5000 matrices were multiplied with sparsity ranging from .01 to .1
  - In the right plot, 10000x10000 matrices were multiplied with sparsity ranging from .01 to .1

![rubric_1.png](./rubric_1.png)

### Optimization Experiment
- All 8 combinations of optimization methods were applied to N1,S1 and N2,S2. The results are shown below.
  - N1,S1: 1000x1000, sparsity = .1
  - N2,S2: 2000x2000, sparsity = .2

![rubric_2.png](./rubric_2.png)
