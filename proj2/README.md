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
![rubric_1.png](./rubric_1.png)
### 
