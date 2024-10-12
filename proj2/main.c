#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <immintrin.h>  // For AVX intrinsics

// Define the matrix struct
struct matrix {
    float **data;  // Pointer to the matrix
    int rows;    // Number of rows
    int cols;    // Number of columns
};

// Struct to pass arguments to thread function
struct thread_args {
    struct matrix *a;      // Matrix A
    struct matrix *b;      // Matrix B
    struct matrix *result; // Result matrix
    int start_row;         // Row number to compute
    int end_row;
};

// Function to get the elapsed time in seconds
double get_elapsed_time(struct timespec start, struct timespec end) {
    double start_sec = start.tv_sec + start.tv_nsec / 1e9;
    double end_sec = end.tv_sec + end.tv_nsec / 1e9;
    return end_sec - start_sec;
}

struct matrix allocate_matrix(int rows, int cols) {
    struct matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (float *)malloc(cols * sizeof(float));
    }
    return mat;
}

// Function to print a matrix
void print_matrix(struct matrix mat) {
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            printf("%f ", mat.data[i][j]);
        }
        printf("\n");
    }
}

bool compare_matrices(struct matrix a, struct matrix b, float tolerance) {
    // Check if the dimensions are the same
    if (a.rows != b.rows || a.cols != b.cols) {
        printf("Matrices have different dimensions.\n");
        return false;
    }

    // Compare each element within the tolerance range
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            if (fabs(a.data[i][j] - b.data[i][j]) > tolerance) {
                printf("Matrices differ at element [%d][%d]: %.2f vs %.2f\n", i, j, a.data[i][j], b.data[i][j]);
                return false;
            }
        }
    }

    return true;
}

// Function to generate a matrix dynamically and return it as a matrix struct
struct matrix generate_matrix(int rows, int cols, float sparsity, bool print) {
    struct matrix mat;
    mat.rows = rows;
    mat.cols = cols;

    // Dynamically allocate memory for the matrix
    mat.data = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (float *)malloc(cols * sizeof(float));
    }

    // Fill matrix with values based on sparsity
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float random_val = (float)rand() / RAND_MAX;
            if (random_val < sparsity) {
                mat.data[i][j] = 0; // Set to 0 based on sparsity
            } else {
                mat.data[i][j] = rand() % 100 + 1; // Random value between 1 and 100
            }
        }
    }

    // Print the matrix if requested
    if (print) {
        printf("Generated matrix (%d x %d) with sparsity %.2f:\n", rows, cols, sparsity);
        print_matrix(mat);
    }

    return mat;
}

// Function to multiply two matrices without optimizations
struct matrix multiply_matrices_no_opt(struct matrix a, struct matrix b) {
    // Check if the multiplication is valid
    if (a.cols != b.rows) {
        printf("Error: Incompatible matrix dimensions for multiplication.\n");
        exit(1);  // Exit with error if dimensions don't match
    }

    // Result matrix (a.rows x b.cols)
    struct matrix result;
    result = allocate_matrix(a.rows, b.cols);

    // Multiply matrices
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result.data[i][j] = 0;  // Initialize result matrix element
            for (int k = 0; k < a.cols; k++) {
                result.data[i][j] += a.data[i][k] * b.data[k][j];
            }
        }
    }
    return result;
}

void *multiply_thread(void *args) {
    struct thread_args *targs = (struct thread_args *)args;
    struct matrix *a = targs->a;
    struct matrix *b = targs->b;
    struct matrix *result = targs->result;
    int start_row = targs->start_row;
    int end_row = targs->end_row;
 
    // Compute the row of the result matrix
    for (int i = start_row; i < end_row; i++) {
      for (int j = 0; j < b->cols; j++) {
          result->data[i][j] = 0;
          for (int k = 0; k < a->cols; k++) {
              result->data[i][j] += a->data[i][k] * b->data[k][j];
          }
      }
    }

    pthread_exit(NULL);
}

// Thread function for matrix multiplication
void *multiply_thread_simd(void *arg) {
    struct thread_args *data = (struct thread_args *)arg;
    struct matrix *a = data->a;
    struct matrix *b = data->b;
    struct matrix *result = data->result;
    int start_row = data->start_row;
    int end_row = data->end_row;

    // Perform SIMD-based matrix multiplication for the rows assigned to this thread
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < b->cols; j++) {
            __m256 sum = _mm256_setzero_ps();  // Initialize sum as 0 for SIMD accumulation
            int k;

            // Perform the matrix multiplication in blocks of 8
            for (k = 0; k <= a->cols - 8; k += 8) {
                __m256 a_vals = _mm256_loadu_ps(&a->data[i][k]);
                __m256 b_vals = _mm256_loadu_ps(&b->data[k][j]);
                __m256 mul_vals = _mm256_mul_ps(a_vals, b_vals);
                sum = _mm256_add_ps(sum, mul_vals);
            }

            // Horizontal sum of the 8 floats in the AVX register
            float result_vals[8];
            _mm256_storeu_ps(result_vals, sum);
            result->data[i][j] = result_vals[0] + result_vals[1] + result_vals[2] +
                                 result_vals[3] + result_vals[4] + result_vals[5] +
                                 result_vals[6] + result_vals[7];

            // Handle the remaining elements if cols is not divisible by 8
            for (; k < a->cols; k++) {
                result->data[i][j] += a->data[i][k] * b->data[k][j];
            }
        }
    }
    return NULL;
}

struct matrix multiply_matrices_multithread(struct matrix a, struct matrix b, int num_threads, int simd) {
    // Check if multiplication is possible
    if (a.cols != b.rows) {
        printf("Error: Incompatible matrix dimensions for multiplication.\n");
        exit(1);  // Exit with error if dimensions don't match
    }

    // Allocate result matrix (a.rows x b.cols)
    struct matrix result = allocate_matrix(a.rows, b.cols);

    int rows_per_thread = a.rows / num_threads;
    int remaining_rows = a.rows % num_threads;


    // Create an array of threads and thread arguments
    pthread_t threads[num_threads];
    struct thread_args targs[num_threads];

    // Create threads to compute each row of the result matrix
    for (int i = 0; i < num_threads; i++) {
        int start_row = i * rows_per_thread;
        int end_row = start_row + rows_per_thread;

        if (i == num_threads - 1) {  // Last thread handles remaining rows
            end_row += remaining_rows;
        }

        targs[i].a = &a;
        targs[i].b = &b;
        targs[i].result = &result;
        targs[i].start_row = start_row;
        targs[i].end_row = end_row;
        if (simd) {pthread_create(&threads[i], NULL, multiply_thread_simd, (void *)&targs[i]);}
        else{pthread_create(&threads[i], NULL, multiply_thread, (void *)&targs[i]);}
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return result;
}

// SIMD-based matrix multiplication (for matrices with float data type)
struct matrix multiply_matrices_simd(struct matrix a, struct matrix b) {
    // Check if multiplication is possible
    if (a.cols != b.rows) {
        printf("Error: Incompatible matrix dimensions for multiplication.\n");
        exit(1);
    }

    // Allocate result matrix (a.rows x b.cols)
    struct matrix result = allocate_matrix(a.rows, b.cols);

    // Perform matrix multiplication using AVX2 intrinsics (float data type)
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < b.cols; j++) {
            // Initialize the result value to 0
            __m256 sum = _mm256_setzero_ps();

            // Multiply in blocks of 8 floats using SIMD
            for (int k = 0; k < a.cols; k += 8) {
                // Load 8 elements from matrix A and B
                __m256 a_vals = _mm256_loadu_ps(&a.data[i][k]);
                __m256 b_vals = _mm256_loadu_ps(&b.data[k][j]);

                // Perform the multiplication and accumulate the result
                __m256 mul_vals = _mm256_mul_ps(a_vals, b_vals);
                sum = _mm256_add_ps(sum, mul_vals);
            }

            // Store the result back to the result matrix
            // (Summing the 8 packed floats to a single float result)
            float result_vals[8];
            _mm256_storeu_ps(result_vals, sum);
            result.data[i][j] = result_vals[0] + result_vals[1] + result_vals[2] + result_vals[3] +
                                result_vals[4] + result_vals[5] + result_vals[6] + result_vals[7];
        }
    }

    return result;
}

// Function to free the memory of a matrix struct
void free_matrix(struct matrix mat) {
    for (int i = 0; i < mat.rows; i++) {
        free(mat.data[i]);
    }
    free(mat.data);
}

// Main function
int main() {
    int rows, cols;
    float sparsity;
    int print, opt, multithread, simd, num_threads;

    srand(time(NULL));  // Seed random number generator

    struct matrix a = generate_matrix(3, 3, 0.5, true);

    // Input for matrix dimensions and sparsity
    printf("Enter the number of rows: ");
    scanf("%d", &rows);
    printf("Enter the number of columns: ");
    scanf("%d", &cols);
    printf("Enter the sparsity (0 to 1, where 0 is dense and 1 is all zeros): ");
    scanf("%f", &sparsity);
    printf("Would you like to print matrices? (0 is no, 1 is yes)\n");
    scanf("%d", &print);
    printf("Use Multithreading? (0 is no, 1 is yes)\n");
    scanf("%d", &multithread);
    printf("How many threads? 1 to %d\n", rows);
    scanf("%d", &num_threads);
    printf("Use SIMD? (0 is no, 1 is yes)\n");
    scanf("%d", &simd);

    // Generate two matrices based on input
    printf("# of rows: %d\n", rows);
    printf("# of cols: %d\n", cols);
    struct matrix matrix1 = generate_matrix(rows, cols, sparsity, print);
    struct matrix matrix2 = generate_matrix(rows, cols, sparsity, print);  // Note: cols of matrix1 must equal rows of matrix2
    struct matrix result_matrix;
    struct matrix result_matrix2;
    struct matrix result_matrix3;
    //measure the time it takes to multiply the matrices
    struct timespec start_time, end_time;

    if(!multithread && !simd){
      clock_gettime(CLOCK_MONOTONIC, &start_time);  // Start time

      // Multiply the matrices with no optimization
      result_matrix = multiply_matrices_no_opt(matrix1, matrix2);

      clock_gettime(CLOCK_MONOTONIC, &end_time);  // End time

      printf("Matrix multiplication took %.12f seconds (No optimization).\n", get_elapsed_time(start_time, end_time));

      // Print the result matrix
      if (print) {
        printf("Result matrix:\n");
        print_matrix(result_matrix);
      }
      free_matrix(result_matrix);
    }

    if (multithread){
      clock_gettime(CLOCK_MONOTONIC, &start_time);  // Start time

      result_matrix2 = multiply_matrices_multithread(matrix1, matrix2, num_threads, simd);

      clock_gettime(CLOCK_MONOTONIC, &end_time);  // End time

      double elapsed_time = get_elapsed_time(start_time, end_time);
      printf("Matrix multiplication took %.12f seconds (Multithreaded only).\n", elapsed_time);

      if (print) {
        printf("Result matrix:\n");
        print_matrix(result_matrix2);
      }
      free_matrix(result_matrix2);
    }

    if (simd && !multithread){
      clock_gettime(CLOCK_MONOTONIC, &start_time);  // Start time

      result_matrix3 = multiply_matrices_simd(matrix1, matrix2);

      clock_gettime(CLOCK_MONOTONIC, &end_time);  // End time

      double elapsed_time = get_elapsed_time(start_time, end_time);
      printf("Matrix multiplication took %.12f seconds (SIMD only).\n", elapsed_time);

      if (print) {
        printf("Result matrix:\n");
        print_matrix(result_matrix3);
      }
      free_matrix(result_matrix3);
    }


    // Free all matrices
    free_matrix(matrix1);
    free_matrix(matrix2);

    return 0;
}

