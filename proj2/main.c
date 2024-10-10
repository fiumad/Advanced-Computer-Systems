#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

// Define the matrix struct
struct matrix {
    int **data;  // Pointer to the matrix
    int rows;    // Number of rows
    int cols;    // Number of columns
};

// Struct to pass arguments to thread function
struct thread_args {
    struct matrix *a;      // Matrix A
    struct matrix *b;      // Matrix B
    struct matrix *result; // Result matrix
    int row;               // Row number to compute
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
    mat.data = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (int *)malloc(cols * sizeof(int));
    }
    return mat;
}

// Function to print a matrix
void print_matrix(struct matrix mat) {
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            printf("%d ", mat.data[i][j]);
        }
        printf("\n");
    }
}

// Function to generate a matrix dynamically and return it as a matrix struct
struct matrix generate_matrix(int rows, int cols, float sparsity, bool print) {
    struct matrix mat;
    mat.rows = rows;
    mat.cols = cols;

    // Dynamically allocate memory for the matrix
    mat.data = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (int *)malloc(cols * sizeof(int));
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
    //start the clock
    clock_t start = clock();
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
    //stop the clock
    clock_t end = clock();
    printf("Elapsed time (no optimization): %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

void *multiply_row(void *args) {
    struct thread_args *targs = (struct thread_args *)args;
    struct matrix *a = targs->a;
    struct matrix *b = targs->b;
    struct matrix *result = targs->result;
    int row = targs->row;

    // Compute the row of the result matrix
    for (int j = 0; j < b->cols; j++) {
        result->data[row][j] = 0;
        for (int k = 0; k < a->cols; k++) {
            result->data[row][j] += a->data[row][k] * b->data[k][j];
        }
    }

    pthread_exit(NULL);
}

struct matrix multiply_matrices_multithread(struct matrix a, struct matrix b){
    // Check if multiplication is possible
    if (a.cols != b.rows) {
        printf("Error: Incompatible matrix dimensions for multiplication.\n");
        exit(1);  // Exit with error if dimensions don't match
    }

    // Allocate result matrix (a.rows x b.cols)
    struct matrix result = allocate_matrix(a.rows, b.cols);

    // Create an array of threads and thread arguments
    pthread_t threads[a.rows];
    struct thread_args targs[a.rows];

    // Create threads to compute each row of the result matrix
    for (int i = 0; i < a.rows; i++) {
        targs[i].a = &a;
        targs[i].b = &b;
        targs[i].result = &result;
        targs[i].row = i;
        pthread_create(&threads[i], NULL, multiply_row, (void *)&targs[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < a.rows; i++) {
        pthread_join(threads[i], NULL);
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
    int print_input;
    bool print, opt, multithread;

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
    scanf("%d", &print_input);
    printf("Multiply with no optimization? (0 is no, 1 is yes)\n");
    scanf("%d", &opt);
    printf("Multiply with multithreading only? (0 is no, 1 is yes)\n");
    scanf("%d", &multithread);
    //printf("Multiply with multithreading only? (0 is no, 1 is yes)\n");
    //scanf("%d", &multithread);

    if (print_input == 1) {
        print = true;
    } else {
        print = false;
    }
    // Generate two matrices based on input
    printf("# of rows: %d\n", rows);
    printf("# of cols: %d\n", cols);
    struct matrix matrix1 = generate_matrix(cols, cols, sparsity, print);
    struct matrix matrix2 = generate_matrix(cols, cols, sparsity, print);  // Note: cols of matrix1 must equal rows of matrix2

    //measure the time it takes to multiply the matrices
    struct timespec start_time, end_time;

    if(opt){
      clock_gettime(CLOCK_MONOTONIC, &start_time);  // Start time

      // Multiply the matrices with no optimization
      struct matrix result_matrix = multiply_matrices_no_opt(matrix1, matrix2);

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

      struct matrix result_matrix2 = multiply_matrices_multithread(matrix1, matrix2);

      clock_gettime(CLOCK_MONOTONIC, &end_time);  // End time

      double elapsed_time = get_elapsed_time(start_time, end_time);
      printf("Matrix multiplication took %.12f seconds (Multithreaded only).\n", elapsed_time);

      if (print) {
        printf("Result matrix:\n");
        print_matrix(result_matrix2);
      }

      free_matrix(result_matrix2);
    }

    // Free all matrices
    free_matrix(matrix1);
    free_matrix(matrix2);

    return 0;
}

